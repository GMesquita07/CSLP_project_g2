import numpy as np
from PIL import Image
import math
import os

def predict(A, B, C):
    if C >= max(A, B):
        return min(A, B)
    elif C <= min(A, B):
        return max(A, B)
    else:
        return A + B - C

def residual_mapping(residual):
    return 2 * residual if residual >= 0 else -2 * residual - 1

def golomb_encode(value, m):
    q = value // m
    r = value % m
    unary = [1] * q + [0]
    b = math.ceil(math.log2(m))
    if (2 ** b - m) > r:
        r_bin = [int(x) for x in format(r, f'0{b-1}b')]
    else:
        r += 2 ** b - m
        r_bin = [int(x) for x in format(r, f'0{b}b')]
    return unary + r_bin

def optimal_m(residuals):
    mean = np.mean(residuals)
    return max(1, 2 ** int(np.ceil(math.log2(mean + 1e-5))))

def encode_block_intra(block):
    residuals = []
    h, w, _ = block.shape
    for y in range(h):
        for x in range(w):
            for c in range(3):
                A = int(block[y, x-1, c]) if x > 0 else 0
                B = int(block[y-1, x, c]) if y > 0 else 0
                C = int(block[y-1, x-1, c]) if x > 0 and y > 0 else 0
                P = predict(A, B, C)
                R = int(block[y, x, c]) - P
                residuals.append(residual_mapping(R))
    return residuals

def encode_block_inter(curr_block, ref_block):
    diff = curr_block.astype(int) - ref_block.astype(int)
    return [residual_mapping(v) for v in diff.flatten()]

def encode_intra_frame(img):
    width, height = img.size
    pixels = np.array(img.convert('RGB'))
    residuals = encode_block_intra(pixels)
    m = optimal_m(residuals)
    bitstream = []
    for v in residuals:
        bitstream.extend(golomb_encode(v, m))

    padded_len = ((len(bitstream) + 7) // 8) * 8
    bitstream += [0] * (padded_len - len(bitstream))
    byte_arr = bytearray()
    for i in range(0, len(bitstream), 8):
        byte = 0
        for bit in bitstream[i:i+8]:
            byte = (byte << 1) | bit
        byte_arr.append(byte)

    return width, height, m, byte_arr

def encode_inter_frame(curr_img, ref_img, block_size=8, search_range=4):
    curr = np.array(curr_img.convert('RGB'))
    ref = np.array(ref_img.convert('RGB'))
    height, width, _ = curr.shape

    modes = []
    mv_list = []
    all_residuals = []

    for y in range(0, height, block_size):
        for x in range(0, width, block_size):
            curr_block = curr[y:y+block_size, x:x+block_size]
            best_mv = (0, 0)
            min_error = float('inf')
            best_block = np.zeros_like(curr_block)

            for dy in range(-search_range, search_range + 1):
                for dx in range(-search_range, search_range + 1):
                    ry, rx = y + dy, x + dx
                    if ry < 0 or rx < 0 or ry + block_size > height or rx + block_size > width:
                        continue
                    ref_block = ref[ry:ry+block_size, rx:rx+block_size]
                    error = np.sum(np.abs(curr_block.astype(int) - ref_block.astype(int)))
                    if error < min_error:
                        min_error = error
                        best_mv = (dy, dx)
                        best_block = ref_block

            intra_res = encode_block_intra(curr_block)
            inter_res = encode_block_inter(curr_block, best_block)
            m_intra = optimal_m(intra_res)
            m_inter = optimal_m(inter_res)

            intra_bits = sum(len(golomb_encode(val, m_intra)) for val in intra_res)
            inter_bits = sum(len(golomb_encode(val, m_inter)) for val in inter_res)

            if intra_bits <= inter_bits:
                modes.append(b'I')
                mv_list.append((0, 0))
                all_residuals.extend(intra_res)
            else:
                modes.append(b'P')
                mv_list.append(best_mv)
                all_residuals.extend(inter_res)

    m = optimal_m(all_residuals)
    bitstream = []
    for v in all_residuals:
        bitstream.extend(golomb_encode(v, m))

    padded_len = ((len(bitstream) + 7) // 8) * 8
    bitstream += [0] * (padded_len - len(bitstream))
    byte_arr = bytearray()
    for i in range(0, len(bitstream), 8):
        byte = 0
        for bit in bitstream[i:i+8]:
            byte = (byte << 1) | bit
        byte_arr.append(byte)

    return modes, mv_list, m, byte_arr

def encode_video_T3(image_paths, output_path="encoded_video_T3.bin", iframe_interval=5, block_size=8, search_range=4):
    with open(output_path, "wb") as f:
        f.write(len(image_paths).to_bytes(2, 'big'))
        for i, path in enumerate(image_paths):
            img = Image.open(path)

            if i % iframe_interval == 0:
                width, height, m, bytes_intra = encode_intra_frame(img)
                f.write(b'I')
                f.write(width.to_bytes(2, 'big'))
                f.write(height.to_bytes(2, 'big'))
                f.write(m.to_bytes(2, 'big'))
                f.write(len(bytes_intra).to_bytes(4, 'big'))
                f.write(bytes_intra)
                last_img = img.copy()
            else:
                modes, mv_list, m, bytes_p = encode_inter_frame(img, last_img, block_size, search_range)
                f.write(b'P')
                f.write(len(modes).to_bytes(2, 'big'))
                for mode, (dy, dx) in zip(modes, mv_list):
                    f.write(mode)
                    f.write(int(dy).to_bytes(1, 'big', signed=True))
                    f.write(int(dx).to_bytes(1, 'big', signed=True))
                f.write(m.to_bytes(2, 'big'))
                f.write(len(bytes_p).to_bytes(4, 'big'))
                f.write(bytes_p)
                last_img = img.copy()
    return output_path