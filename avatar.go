// avatar.go
package main

import (
	"flag"
	"fmt"
	"hash/fnv"
	"image"
	"image/color"
	"image/draw"
	"image/jpeg"
	"image/png"
	"math/rand"
	"os"
	"strconv"
	"strings"
	"time"

	"github.com/fogleman/gg"
)

func hashString(s string) uint32 {
	h := fnv.New32a()
	h.Write([]byte(s))
	return h.Sum32()
}

func hashToColor(h uint32) color.RGBA {
	return color.RGBA{
		R: uint8((h >> 16) & 0xFF),
		G: uint8((h >> 8) & 0xFF),
		B: uint8(h & 0xFF),
		A: 255,
	}
}

func parseHexColor(c string) color.RGBA {
	if len(c) == 7 && c[0] == '#' {
		r, _ := strconv.ParseInt(c[1:3], 16, 8)
		g, _ := strconv.ParseInt(c[3:5], 16, 8)
		b, _ := strconv.ParseInt(c[5:7], 16, 8)
		return color.RGBA{uint8(r), uint8(g), uint8(b), 255}
	}
	return color.RGBA{0, 0, 0, 255}
}

func generateAvatar(name, output string, size int, fmt string, mainColor, bgColor string, addText bool) {
	hash := hashString(name)
	mainCol := hashToColor(hash)
	bgCol := hashToColor(hash + 12345)

	if mainColor != "" {
		mainCol = parseHexColor(mainColor)
	}
	if bgColor != "" {
		bgCol = parseHexColor(bgColor)
	}

	dc := gg.NewContext(size, size)
	dc.SetColor(bgCol)
	dc.Clear()

	// Рисуем круг
	margin := float64(size) / 20.0
	dc.SetColor(mainCol)
	dc.DrawCircle(float64(size)/2, float64(size)/2, float64(size)/2-margin)
	dc.Fill()

	if addText {
		// Инициалы
		initials := ""
		parts := strings.Split(name, " ")
		for _, p := range parts {
			if len(p) > 0 {
				initials += string(p[0])
			}
		}
		if initials == "" {
			initials = string(name[0])
		}
		initials = strings.ToUpper(initials)
		fontSize := float64(size) / 3.0
		dc.SetColor(color.RGBA{255, 255, 255, 255})
		if err := dc.LoadFontFace("/usr/share/fonts/truetype/liberation/LiberationSans-Bold.ttf", fontSize); err != nil {
			// fallback
			dc.SetRGB(1, 1, 1)
		}
		dc.DrawStringAnchored(initials, float64(size)/2, float64(size)/2, 0.5, 0.5)
	}

	// Сохранение
	var err error
	switch fmt {
	case "png":
		err = dc.SavePNG(output)
	case "jpg", "jpeg":
		err = dc.SaveJPG(output, 90)
	default:
		err = dc.SavePNG(output)
	}
	if err == nil {
		fmt.Printf("✅ Аватарка сохранена в %s\n", output)
	} else {
		fmt.Printf("❌ Ошибка: %v\n", err)
	}
}

func main() {
	var (
		name      string
		output    string
		size      int
		format    string
		mainColor string
		bgColor   string
		addText   bool
		random    bool
	)
	flag.StringVar(&name, "name", "", "Имя")
	flag.StringVar(&output, "o", "avatar.png", "Выходной файл")
	flag.IntVar(&size, "s", 256, "Размер")
	flag.StringVar(&format, "f", "png", "Формат (png,jpg,webp)")
	flag.StringVar(&mainColor, "c", "", "Основной цвет (HEX)")
	flag.StringVar(&bgColor, "b", "", "Цвет фона (HEX)")
	flag.BoolVar(&addText, "t", false, "Добавить текст")
	flag.BoolVar(&random, "r", false, "Случайная генерация")
	flag.Parse()

	if flag.NArg() > 0 {
		name = flag.Arg(0)
	}

	if random {
		rand.Seed(time.Now().UnixNano())
		name = fmt.Sprintf("User%d", rand.Intn(10000))
	}
	if name == "" {
		fmt.Println("Укажите имя или используйте --random")
		os.Exit(1)
	}

	generateAvatar(name, output, size, format, mainColor, bgColor, addText)
}
