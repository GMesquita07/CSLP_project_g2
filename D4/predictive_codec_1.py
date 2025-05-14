import numpy as np
from PIL import Image
import math

# Preditor LOCO-I (JPEG-LS)
def predict(A, B, C):
    if C >= max(A, B):
        return min(A, B)
    elif C <= min(A, B):
        return max(A, B)
    else:
        return A + B - C

# Mapeamento de inteiro para não-negativo (Rice mapping)
def residual_mapping(residual):
    return 2 * residual if residual >= 0 else -2 * residual - 1

# Codificação Golomb simples
def golomb_encode(value, m):
    q = value // m
    r = value % m
    unary = '1' * q + '0'

    b = math.ceil(math.log2(m))
    if (2 ** b - m) > r:
        r_bin = format(r, f'0{b-1}b')
    else:
        r += 2 ** b - m
        r_bin = format(r, f'0{b}b')

    return unary + r_bin

# Codificador principal
def encode_image(image_path, m=5):
    img = Image.open(image_path).convert('RGB')
    width, height = img.size
    pixels = np.array(img)

    bitstream = ''
    for y in range(height):
        for x in range(width):
            for c in range(3):  # R, G, B
                A = int(pixels[y, x-1, c]) if x > 0 else 0
                B = int(pixels[y-1, x, c]) if y > 0 else 0
                C = int(pixels[y-1, x-1, c]) if x > 0 and y > 0 else 0
                P = predict(A, B, C)
                R = int(pixels[y, x, c]) - P
                mapped = residual_mapping(R)
                bitstream += golomb_encode(mapped, m)
    return bitstream

# Exemplo de uso
if __name__ == "__main__":
    path = "lebronjames.webp"  # Caminho para a imagem testada
    m = 5  # Parâmetro Golomb
    stream = encode_image(path, m)
    print("Primeiros bits:", stream[:200])
    print("Comprimento total do bitstream:", len(stream))
