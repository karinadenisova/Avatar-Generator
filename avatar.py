# avatar.py
#!/usr/bin/env python3
# -*- coding: utf-8 -*-

import sys
import os
import hashlib
import argparse
import random
from PIL import Image, ImageDraw, ImageFont, ImageColor
import math

def hash_to_color(hash_val):
    """Преобразует хеш в цвет (HEX)."""
    r = (hash_val >> 16) & 0xFF
    g = (hash_val >> 8) & 0xFF
    b = hash_val & 0xFF
    return (r, g, b)

def generate_avatar(name, size=256, output="avatar.png", fmt="png", main_color=None, bg_color=None, add_text=False):
    """Генерирует аватарку на основе имени."""
    if not main_color:
        hash_val = int(hashlib.md5(name.encode()).hexdigest()[:8], 16)
        main_color = hash_to_color(hash_val)
    else:
        main_color = ImageColor.getrgb(main_color)

    if not bg_color:
        bg_hash = int(hashlib.md5((name + "bg").encode()).hexdigest()[:8], 16)
        bg_color = hash_to_color(bg_hash)
    else:
        bg_color = ImageColor.getrgb(bg_color)

    # Создаём изображение
    img = Image.new('RGB', (size, size), bg_color)
    draw = ImageDraw.Draw(img)

    # Рисуем круг
    margin = size // 20
    draw.ellipse((margin, margin, size - margin, size - margin), fill=main_color)

    # Добавляем инициалы
    if add_text:
        initials = ''.join(word[0].upper() for word in name.split() if word)
        if not initials:
            initials = name[0].upper()
        try:
            font_size = size // 3
            font = ImageFont.truetype("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", font_size)
        except:
            font = ImageFont.load_default()
        bbox = draw.textbbox((0, 0), initials, font=font)
        text_w = bbox[2] - bbox[0]
        text_h = bbox[3] - bbox[1]
        x = (size - text_w) // 2
        y = (size - text_h) // 2
        draw.text((x, y), initials, fill=(255, 255, 255), font=font)

    # Сохраняем
    img.save(output, format=fmt.upper())

def batch_generate(names, **kwargs):
    """Генерирует аватарки для списка имён."""
    for name in names:
        output = f"avatar_{name}.png"
        generate_avatar(name, **kwargs, output=output)
        print(f"✅ {name} -> {output}")

def main():
    parser = argparse.ArgumentParser(description="Avatar Generator")
    parser.add_argument('name', nargs='?', default=None, help='Имя для генерации')
    parser.add_argument('-o', '--output', default='avatar.png', help='Выходной файл')
    parser.add_argument('-s', '--size', type=int, default=256, help='Размер в пикселях')
    parser.add_argument('-f', '--format', choices=['png', 'jpg', 'webp'], default='png', help='Формат')
    parser.add_argument('-c', '--color', help='Основной цвет (HEX)')
    parser.add_argument('-b', '--bg', help='Цвет фона (HEX)')
    parser.add_argument('-t', '--text', action='store_true', help='Добавить текст (инициалы)')
    parser.add_argument('-r', '--random', action='store_true', help='Случайная генерация')
    parser.add_argument('--batch', help='Пакетная генерация (имена через запятую)')
    parser.add_argument('--batch-file', help='Файл со списком имён (по одному на строку)')
    args = parser.parse_args()

    if args.random:
        names = [f"User{random.randint(1000, 9999)}"]
    elif args.batch:
        names = [n.strip() for n in args.batch.split(',') if n.strip()]
    elif args.batch_file:
        with open(args.batch_file, 'r') as f:
            names = [line.strip() for line in f if line.strip()]
    elif args.name:
        names = [args.name]
    else:
        print("Укажите имя или используйте --random", file=sys.stderr)
        sys.exit(1)

    kwargs = {
        'size': args.size,
        'fmt': args.format,
        'main_color': args.color,
        'bg_color': args.bg,
        'add_text': args.text,
    }

    if len(names) == 1:
        output = args.output or f"avatar_{names[0]}.png"
        generate_avatar(names[0], output=output, **kwargs)
        print(f"✅ Аватарка сохранена в {output}")
    else:
        batch_generate(names, **kwargs)

if __name__ == '__main__':
    main()
