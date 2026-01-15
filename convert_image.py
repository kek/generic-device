#!/usr/bin/env python3
"""
Convert JPG image to RGB565 format for ST7735 display
"""

from PIL import Image
import sys

def rgb888_to_rgb565(r, g, b):
    """Convert RGB888 to RGB565"""
    r5 = (r >> 3) & 0x1F
    g6 = (g >> 2) & 0x3F
    b5 = (b >> 3) & 0x1F
    return (r5 << 11) | (g6 << 5) | b5

def convert_image(input_file, output_file, width, height):
    """Convert image to RGB565 C header file"""

    # Open and resize image
    img = Image.open(input_file)
    print(f"Original image size: {img.size}")

    # Resize to display dimensions
    img = img.resize((width, height), Image.Resampling.LANCZOS)
    img = img.convert('RGB')

    # Convert to RGB565
    pixels = list(img.getdata())
    rgb565_data = []

    for r, g, b in pixels:
        rgb565 = rgb888_to_rgb565(r, g, b)
        # Store as big-endian (high byte first, low byte second)
        rgb565_data.append((rgb565 >> 8) & 0xFF)
        rgb565_data.append(rgb565 & 0xFF)

    # Write C header file
    with open(output_file, 'w') as f:
        f.write("#ifndef IMAGE_DATA_H\n")
        f.write("#define IMAGE_DATA_H\n\n")
        f.write("#include <stdint.h>\n\n")
        f.write(f"#define IMAGE_WIDTH {width}\n")
        f.write(f"#define IMAGE_HEIGHT {height}\n\n")
        f.write(f"// Image size: {len(rgb565_data)} bytes ({width}x{height} pixels in RGB565)\n")
        f.write("static const uint8_t image_data[] = {\n")

        # Write data in rows of 16 bytes for readability
        for i in range(0, len(rgb565_data), 16):
            chunk = rgb565_data[i:i+16]
            hex_values = ', '.join(f'0x{b:02X}' for b in chunk)
            f.write(f"    {hex_values},\n")

        f.write("};\n\n")
        f.write("#endif // IMAGE_DATA_H\n")

    print(f"Converted {input_file} to {output_file}")
    print(f"Resolution: {width}x{height}")
    print(f"Data size: {len(rgb565_data)} bytes")

if __name__ == "__main__":
    # ST7735 display in landscape mode (rotation 1)
    WIDTH = 160
    HEIGHT = 128

    input_file = "PXL_20260115_211503040.jpg"
    output_file = "main/image_data.h"

    convert_image(input_file, output_file, WIDTH, HEIGHT)
    print("Done!")
