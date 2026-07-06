// avatar.js
#!/usr/bin/env node
'use strict';

const { createCanvas, loadImage } = require('canvas');
const fs = require('fs');
const crypto = require('crypto');

function hashString(str) {
    return crypto.createHash('md5').update(str).digest('hex');
}

function hashToColor(hash) {
    const r = parseInt(hash.substring(0, 2), 16);
    const g = parseInt(hash.substring(2, 4), 16);
    const b = parseInt(hash.substring(4, 6), 16);
    return { r, g, b };
}

function parseHexColor(hex) {
    if (hex.length === 7 && hex[0] === '#') {
        const r = parseInt(hex.substring(1, 3), 16);
        const g = parseInt(hex.substring(3, 5), 16);
        const b = parseInt(hex.substring(5, 7), 16);
        return { r, g, b };
    }
    return { r: 0, g: 0, b: 0 };
}

function generateAvatar(name, output, size, format, mainColor, bgColor, addText) {
    const hash = hashString(name);
    let mainCol = hashToColor(hash);
    let bgCol = hashToColor(hashString(name + 'bg'));

    if (mainColor) mainCol = parseHexColor(mainColor);
    if (bgColor) bgCol = parseHexColor(bgColor);

    const canvas = createCanvas(size, size);
    const ctx = canvas.getContext('2d');

    // Фон
    ctx.fillStyle = `rgb(${bgCol.r},${bgCol.g},${bgCol.b})`;
    ctx.fillRect(0, 0, size, size);

    // Круг
    const margin = size / 20;
    ctx.beginPath();
    ctx.arc(size/2, size/2, size/2 - margin, 0, 2 * Math.PI);
    ctx.fillStyle = `rgb(${mainCol.r},${mainCol.g},${mainCol.b})`;
    ctx.fill();

    // Текст
    if (addText) {
        const initials = name.split(' ')
            .filter(w => w.length > 0)
            .map(w => w[0].toUpperCase())
            .join('');
        const fontsize = size / 3;
        ctx.fillStyle = '#ffffff';
        ctx.textAlign = 'center';
        ctx.textBaseline = 'middle';
        ctx.font = `bold ${fontsize}px sans-serif`;
        ctx.fillText(initials || name[0].toUpperCase(), size/2, size/2);
    }

    // Сохранение
    const stream = canvas.createStream(format === 'png' ? 'image/png' : 'image/jpeg');
    const out = fs.createWriteStream(output);
    stream.pipe(out);
    out.on('finish', () => console.log(`✅ Аватарка сохранена в ${output}`));
    out.on('error', (err) => console.error(`❌ Ошибка: ${err.message}`));
}

function main() {
    const args = process.argv.slice(2);
    let name = '', output = 'avatar.png', size = 256, format = 'png';
    let mainColor = '', bgColor = '', addText = false, random = false;

    for (let i = 0; i < args.length; i++) {
        if (args[i] === '-o' && i+1 < args.length) output = args[++i];
        else if (args[i] === '-s' && i+1 < args.length) size = parseInt(args[++i], 10);
        else if (args[i] === '-f' && i+1 < args.length) format = args[++i];
        else if (args[i] === '-c' && i+1 < args.length) mainColor = args[++i];
        else if (args[i] === '-b' && i+1 < args.length) bgColor = args[++i];
        else if (args[i] === '-t') addText = true;
        else if (args[i] === '-r') random = true;
        else if (args[i] === '-h' || args[i] === '--help') {
            console.log(`Usage: node avatar.js [name] [options]
  -o <file>   Output file
  -s <N>      Size
  -f <fmt>    Format (png,jpg,webp)
  -c <color>  Main color
  -b <color>  Background color
  -t          Add text
  -r          Random`);
            process.exit(0);
        } else if (!name) name = args[i];
    }

    if (random) {
        name = `User${Math.floor(Math.random() * 10000)}`;
    }
    if (!name) {
        console.error('Укажите имя или используйте --random');
        process.exit(1);
    }

    generateAvatar(name, output, size, format, mainColor, bgColor, addText);
}

main();
