from PIL import Image
from t3_codec import encode_video_T3

# Cria sequência de 6 frames (com deslocamento horizontal simulado)
base = Image.open("lebronjames.webp").convert("RGB")
frame_paths = []
for i in range(6):
    path = f"frame_{i}.png"
    shifted = base.transform(base.size, Image.AFFINE, (1, 0, i, 0, 1, 0))  # desloca i px
    shifted.save(path)
    frame_paths.append(path)

# Codifica o vídeo
encode_video_T3(
    image_paths=frame_paths,
    output_path="encoded_video_T3.bin",
    iframe_interval=2,
    block_size=8,
    search_range=4
)
