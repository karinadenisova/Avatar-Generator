// avatar.cs
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Security.Cryptography;
using System.Text;
using SixLabors.ImageSharp;
using SixLabors.ImageSharp.Processing;
using SixLabors.ImageSharp.Drawing.Processing;
using SixLabors.ImageSharp.PixelFormats;
using SixLabors.ImageSharp.Formats.Jpeg;

class Avatar
{
    static byte[] HashString(string s)
    {
        using var md5 = MD5.Create();
        return md5.ComputeHash(Encoding.UTF8.GetBytes(s));
    }

    static Rgb24 HashToColor(byte[] hash)
    {
        return new Rgb24(hash[0], hash[1], hash[2]);
    }

    static Rgb24 ParseHexColor(string hex)
    {
        if (hex.Length == 7 && hex[0] == '#')
        {
            int r = Convert.ToInt32(hex.Substring(1, 2), 16);
            int g = Convert.ToInt32(hex.Substring(3, 2), 16);
            int b = Convert.ToInt32(hex.Substring(5, 2), 16);
            return new Rgb24((byte)r, (byte)g, (byte)b);
        }
        return new Rgb24(0, 0, 0);
    }

    static void GenerateAvatar(string name, string output, int size, string format,
                               string mainColor, string bgColor, bool addText)
    {
        var hash = HashString(name);
        var mainCol = HashToColor(hash);
        var bgCol = HashToColor(HashString(name + "bg"));

        if (!string.IsNullOrEmpty(mainColor)) mainCol = ParseHexColor(mainColor);
        if (!string.IsNullOrEmpty(bgColor)) bgCol = ParseHexColor(bgColor);

        using var img = new Image<Rgb24>(size, size);
        img.Mutate(ctx =>
        {
            ctx.Fill(bgCol);
            var margin = size / 20;
            ctx.Fill(mainCol, new EllipsePolygon(size/2f, size/2f, size/2f - margin));
            if (addText)
            {
                var initials = string.Join("", name.Split(' ')
                    .Where(w => w.Length > 0)
                    .Select(w => char.ToUpper(w[0])));
                if (initials.Length == 0) initials = char.ToUpper(name[0]).ToString();
                var font = SystemFonts.CreateFont("Arial", size/3f);
                ctx.DrawText(initials, font, Color.White, new PointF(size/2f, size/2f));
            }
        });

        var encoder = format.ToLower() == "jpg" || format.ToLower() == "jpeg"
            ? new JpegEncoder { Quality = 90 }
            : null;
        img.Save(output, encoder);
        Console.WriteLine($"✅ Аватарка сохранена в {output}");
    }

    static void Main(string[] args)
    {
        string name = null, output = "avatar.png", mainColor = null, bgColor = null;
        int size = 256;
        string format = "png";
        bool addText = false, random = false;

        for (int i = 0; i < args.Length; i++)
        {
            if (args[i] == "-o" && i+1 < args.Length) output = args[++i];
            else if (args[i] == "-s" && i+1 < args.Length) size = int.Parse(args[++i]);
            else if (args[i] == "-f" && i+1 < args.Length) format = args[++i];
            else if (args[i] == "-c" && i+1 < args.Length) mainColor = args[++i];
            else if (args[i] == "-b" && i+1 < args.Length) bgColor = args[++i];
            else if (args[i] == "-t") addText = true;
            else if (args[i] == "-r") random = true;
            else if (args[i] == "-h" || args[i] == "--help")
            {
                Console.WriteLine("Usage: avatar [name] [options]\n  -o <file>   Output file\n  -s <N>      Size\n  -f <fmt>    Format\n  -c <color>  Main color\n  -b <color>  Background color\n  -t          Add text\n  -r          Random");
                return;
            }
            else if (name == null) name = args[i];
        }

        if (random)
        {
            var rnd = new Random();
            name = $"User{rnd.Next(1000, 9999)}";
        }
        if (string.IsNullOrEmpty(name))
        {
            Console.Error.WriteLine("Укажите имя или используйте --random");
            Environment.Exit(1);
        }

        GenerateAvatar(name, output, size, format, mainColor, bgColor, addText);
    }
}
