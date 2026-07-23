import serial
import struct
from PIL import Image

PORT = "COM5"
BAUD = 921600

SCREEN_WIDTH = 320
SCREEN_HEIGHT = 240

MAGIC = 0xA55A
END = 0x5AA5
FRAME_END = 0x55AA

ser = serial.Serial(PORT, BAUD, timeout=5)

img = Image.new("RGB", (SCREEN_WIDTH, SCREEN_HEIGHT), (0, 0, 0))

def read_exact(n):
    data = bytearray()
    while len(data) < n:
        chunk = ser.read(n - len(data))
        if not chunk:
            raise TimeoutError("Serial timeout")
        data.extend(chunk)
    return bytes(data)

def rgb565_to_rgb(raw):
    pixels = []

    for i in range(0, len(raw), 2):
        pixel = raw[i + 1] | (raw[i] << 8)

        r = ((pixel >> 11) & 0x1F) * 255 // 31
        g = ((pixel >> 5) & 0x3F) * 255 // 63
        b = (pixel & 0x1F) * 255 // 31

        pixels.append((r, g, b))

    return pixels

while True:
    marker = struct.unpack("<H", read_exact(2))[0]

    if marker == MAGIC:
        x, y, w, h = struct.unpack("<HHHH", read_exact(8))
        length = struct.unpack("<I", read_exact(4))[0]

        raw = read_exact(length)

        if struct.unpack("<H", read_exact(2))[0] != END:
            raise RuntimeError("Bad slice end")

        slice_img = Image.new("RGB", (w, h))
        slice_img.putdata(rgb565_to_rgb(raw))

        img.paste(slice_img, (x, y))

    elif marker == FRAME_END:
        break

img.save("full_ui.png")
print("Saved full_ui.png")