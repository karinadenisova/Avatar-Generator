#!/usr/bin/env ruby
# avatar.rb
# encoding: UTF-8

require 'rmagick'
include Magick
require 'digest'
require 'optparse'

def hash_string(str)
  Digest::MD5.hexdigest(str)
end

def hash_to_color(hash)
  r = hash[0..1].to_i(16)
  g = hash[2..3].to_i(16)
  b = hash[4..5].to_i(16)
  { r: r, g: g, b: b }
end

def parse_hex_color(hex)
  if hex.length == 7 && hex[0] == '#'
    r = hex[1..2].to_i(16)
    g = hex[3..4].to_i(16)
    b = hex[5..6].to_i(16)
    return { r: r, g: g, b: b }
  end
  { r: 0, g: 0, b: 0 }
end

def generate_avatar(name, output, size, format, main_color, bg_color, add_text)
  hash = hash_string(name)
  main_col = hash_to_color(hash)
  bg_col = hash_to_color(hash_string(name + 'bg'))

  main_col = parse_hex_color(main_color) if main_color
  bg_col = parse_hex_color(bg_color) if bg_color

  img = Image.new(size, size) { self.background_color = "rgb(#{bg_col[:r]},#{bg_col[:g]},#{bg_col[:b]})" }
  draw = Draw.new

  # Круг
  margin = size / 20
  draw.fill("rgb(#{main_col[:r]},#{main_col[:g]},#{main_col[:b]})")
  draw.ellipse(size/2, size/2, size/2 - margin, size/2 - margin, 0, 360)
  draw.draw(img)

  # Текст
  if add_text
    initials = name.split.map { |w| w[0].upcase }.join
    initials = name[0].upcase if initials.empty?
    font_size = size / 3
    draw.annotate(img, 0, 0, 0, 0, initials) do
      self.gravity = CenterGravity
      self.pointsize = font_size
      self.font = 'Arial'
      self.fill = 'white'
      self.font_weight = BoldWeight
    end
  end

  img.write(output) { self.quality = 90 if format =~ /jpe?g/ }
  puts "✅ Аватарка сохранена в #{output}"
end

def main
  name = nil
  output = 'avatar.png'
  size = 256
  format = 'png'
  main_color = nil
  bg_color = nil
  add_text = false
  random = false

  OptionParser.new do |opts|
    opts.banner = "Usage: avatar.rb [name] [options]"
    opts.on('-o FILE', 'Output file') { |v| output = v }
    opts.on('-s N', Integer, 'Size') { |v| size = v }
    opts.on('-f FMT', 'Format (png,jpg,webp)') { |v| format = v }
    opts.on('-c COLOR', 'Main color') { |v| main_color = v }
    opts.on('-b COLOR', 'Background color') { |v| bg_color = v }
    opts.on('-t', 'Add text') { add_text = true }
    opts.on('-r', 'Random') { random = true }
    opts.on('-h', 'Help') { puts opts; exit }
  end.parse!

  name = ARGV[0] if ARGV[0]

  if random
    name = "User#{rand(1000..9999)}"
  end
  unless name
    puts "Укажите имя или используйте --random"
    exit 1
  end

  generate_avatar(name, output, size, format, main_color, bg_color, add_text)
end

main if __FILE__ == $0
