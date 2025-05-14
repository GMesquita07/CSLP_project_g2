import os
import math
import numpy as np
from PIL import Image

# --- Funções auxiliares ---

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

# --- Codificação de um frame (imagem) ---

def encode_frame(img: Image.Image):
    width, height = img.size
    pixels = np.array(img.convert('RGB'))

    residuals = []
    for y in range(height):
        for x in range(width):
            for c in range(3):  # canais RGB
                A = int(pixels[y, x-1, c]) if x > 0 else 0
                B = int(pixels[y-1, x, c]) if y > 0 else 0
                C = int(pixels[y-1, x-1, c]) if x > 0 and y > 0 else 0
                P = predict(A, B, C)
                R = int(pixels[y, x, c]) - P
                mapped = residual_mapping(R)
                residuals.append(mapped)

    m = optimal_m(residuals)

    # Codificação Golomb
    bitstream = []
    for value in residuals:
        bitstream.extend(golomb_encode(value, m))

    # Padding e conversão para bytes
    padded_len = ((len(bitstream) + 7) // 8) * 8
    bitstream += [0] * (padded_len - len(bitstream))
    byte_arr = bytearray()
    for i in range(0, len(bitstream), 8):
        byte = 0
        for bit in bitstream[i:i+8]:
            byte = (byte << 1) | bit
        byte_arr.append(byte)

    return width, height, m, byte_arr

# --- Codificação de vídeo (sequência de frames) ---

def encode_video_frame_sequence(image_paths, output_path="encoded_video.bin"):
    with open(output_path, "wb") as f:
        f.write(len(image_paths).to_bytes(2, 'big'))  # número de frames
        for path in image_paths:
            img = Image.open(path)
            width, height, m, byte_arr = encode_frame(img)

            f.write(width.to_bytes(2, 'big'))
            f.write(height.to_bytes(2, 'big'))
            f.write(m.to_bytes(2, 'big'))
            f.write(len(byte_arr).to_bytes(4, 'big'))
            f.write(byte_arr)

    return output_path

# --- Exemplo de uso ---

if __name__ == "__main__":
    from PIL import Image

    # Cria 3 frames de teste com a mesma imagem
    base_image = Image.open("lebronjames.webp").convert("RGB")
    frame_paths = []
    for i in range(3):
        path = f"frame_{i}.png"
        base_image.save(path)
        frame_paths.append(path)

    # Codifica o "vídeo"
    output = encode_video_frame_sequence(frame_paths)
    print(f"Vídeo codificado em: {output}")
