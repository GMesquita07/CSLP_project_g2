import numpy as np
from PIL import Image
from scipy.fftpack import dct, idct
import os

# ---------- UTILS ----------
def blockify(img_array, blk):
    h, w = img_array.shape
    return img_array.reshape(h // blk, blk, -1, blk).swapaxes(1, 2)  # (by, bx, blk, blk)

def unblockify(blocks, blk):
    """Reconstrói explicitamente – evita erros de reshape."""
    by, bx, _, _ = blocks.shape
    out = np.zeros((by * blk, bx * blk), dtype=np.float32)
    for i in range(by):
        for j in range(bx):
            out[i*blk:(i+1)*blk, j*blk:(j+1)*blk] = blocks[i, j]
    return out

def zigzag_order(block):
    order = sorted(((x, y) for x in range(8) for y in range(8)),
                   key=lambda s: (s[0]+s[1], -s[1] if (s[0]+s[1]) & 1 else s[1]))
    return [block[i, j] for i, j in order]

def inverse_zigzag(lst):
    block = np.zeros((8, 8), np.float32)
    order = sorted(((x, y) for x in range(8) for y in range(8)),
                   key=lambda s: (s[0]+s[1], -s[1] if (s[0]+s[1]) & 1 else s[1]))
    for (i, j), v in zip(order, lst):
        block[i, j] = v
    return block

quantize   = lambda b, q: np.round(b/q).astype(np.int16)
dequantize = lambda b, q: (b*q).astype(np.float32)

# ---------- FRAME CODING ----------
def encode_frame_dct(img, blk=8, q=10):
    img  = img.convert("L")
    arr  = np.array(img, np.float32) - 128
    h, w = arr.shape
    arr  = arr[:h - h%blk, :w - w%blk]           # garante múltiplos de blk
    by, bx = arr.shape[0]//blk, arr.shape[1]//blk

    zzc = []
    for b in blockify(arr, blk).reshape(-1, blk, blk):
        q_blk = quantize(dct(dct(b.T, norm='ortho').T, norm='ortho'), q)
        zzc.append(zigzag_order(q_blk))
    return zzc, arr.shape         # shape real usado

def decode_frame_dct(zzc, shape, blk=8, q=10):
    h, w = shape
    by, bx = h//blk, w//blk
    blocks = np.zeros((by, bx, blk, blk), np.float32)
    for k, zz in enumerate(zzc):
        i, j = divmod(k, bx)
        deq = dequantize(inverse_zigzag(zz), q)
        blocks[i, j] = idct(idct(deq.T, norm='ortho').T, norm='ortho')
    frame = unblockify(blocks, blk) + 128
    return np.clip(frame, 0, 255).astype(np.uint8)

# ---------- VIDEO CODING ----------
def encode_video_dct(paths, out="video_dct.npz", blk=8, q=10):
    video, shape = [], None
    for p in paths:
        zzc, s = encode_frame_dct(Image.open(p), blk, q)
        video.append(zzc)
        shape = shape or s
    np.savez_compressed(out, video=video, shape=shape, q=q, blk=blk)
    return out

def decode_video_dct(npz, out_dir="frames_dct"):
    data       = np.load(npz, allow_pickle=True)
    video      = data["video"]
    shape      = tuple(data["shape"])
    q, blk     = int(data["q"]), int(data["blk"])
    os.makedirs(out_dir, exist_ok=True)
    for i, zzc in enumerate(video):
        img = decode_frame_dct(zzc, shape, blk, q)
        Image.fromarray(img).save(f"{out_dir}/frame_{i}.png")

# ---------- TESTE RÁPIDO ----------
if __name__ == "__main__":
    base   = Image.open("lebronjames.webp").convert("L")
    paths  = []
    for i in range(5):
        p = f"frame_{i}.png"
        base.transform(base.size, Image.AFFINE, (1,0,i, 0,1,0)).save(p)
        paths.append(p)

    encode_video_dct(paths, "video_dct.npz", q=20)
    decode_video_dct("video_dct.npz", "frames_dct")
    print("✅ Compressão e reconstrução concluídas em frames_dct/")
