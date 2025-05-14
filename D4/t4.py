from PIL import Image
from t4_dct_codec import encode_video_dct, decode_video_dct

# Criar frames simulados
base = Image.open("lebronjames.webp").convert("L")
paths = []
for i in range(5):
    shifted = base.transform(base.size, Image.AFFINE, (1, 0, i, 0, 1, 0))
    p = f"frame_{i}.png"
    shifted.save(p)
    paths.append(p)

# Codificar
encode_video_dct(paths, output_path="video_dct.npz", q_level=20)

# Decodificar
decode_video_dct("video_dct.npz", output_folder="frames_dct")
