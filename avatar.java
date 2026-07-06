// avatar.java
import java.awt.*;
import java.awt.image.*;
import java.io.*;
import java.nio.file.*;
import java.security.*;
import javax.imageio.*;

public class avatar {
    public static void main(String[] args) throws Exception {
        String name = null, output = "avatar.png", mainColor = null, bgColor = null;
        int size = 256;
        String format = "png";
        boolean addText = false, random = false;

        for (int i = 0; i < args.length; i++) {
            if (args[i].equals("-o") && i+1 < args.length) output = args[++i];
            else if (args[i].equals("-s") && i+1 < args.length) size = Integer.parseInt(args[++i]);
            else if (args[i].equals("-f") && i+1 < args.length) format = args[++i];
            else if (args[i].equals("-c") && i+1 < args.length) mainColor = args[++i];
            else if (args[i].equals("-b") && i+1 < args.length) bgColor = args[++i];
            else if (args[i].equals("-t")) addText = true;
            else if (args[i].equals("-r")) random = true;
            else if (args[i].equals("-h") || args[i].equals("--help")) {
                System.out.println("Usage: avatar [name] [options]\n  -o <file>   Output file\n  -s <N>      Size\n  -f <fmt>    Format\n  -c <color>  Main color\n  -b <color>  Background color\n  -t          Add text\n  -r          Random");
                return;
            } else if (name == null) name = args[i];
        }

        if (random) {
            name = "User" + (int)(Math.random() * 9000 + 1000);
        }
        if (name == null) {
            System.err.println("Укажите имя или используйте --random");
            System.exit(1);
        }

        generateAvatar(name, output, size, format, mainColor, bgColor, addText);
    }

    static byte[] hashString(String s) throws Exception {
        MessageDigest md = MessageDigest.getInstance("MD5");
        return md.digest(s.getBytes());
    }

    static Color hashToColor(byte[] hash) {
        return new Color(hash[0] & 0xFF, hash[1] & 0xFF, hash[2] & 0xFF);
    }

    static Color parseHexColor(String hex) {
        if (hex.length() == 7 && hex.charAt(0) == '#') {
            int r = Integer.parseInt(hex.substring(1, 3), 16);
            int g = Integer.parseInt(hex.substring(3, 5), 16);
            int b = Integer.parseInt(hex.substring(5, 7), 16);
            return new Color(r, g, b);
        }
        return new Color(0, 0, 0);
    }

    static void generateAvatar(String name, String output, int size, String format,
                               String mainColor, String bgColor, boolean addText) throws Exception {
        byte[] hash = hashString(name);
        Color mainCol = hashToColor(hash);
        Color bgCol = hashToColor(hashString(name + "bg"));

        if (mainColor != null) mainCol = parseHexColor(mainColor);
        if (bgColor != null) bgCol = parseHexColor(bgColor);

        BufferedImage img = new BufferedImage(size, size, BufferedImage.TYPE_INT_RGB);
        Graphics2D g = img.createGraphics();

        // Фон
        g.setColor(bgCol);
        g.fillRect(0, 0, size, size);

        // Круг
        int margin = size / 20;
        g.setColor(mainCol);
        g.fillOval(margin, margin, size - 2*margin, size - 2*margin);

        // Текст
        if (addText) {
            String initials = "";
            for (String w : name.split(" ")) {
                if (w.length() > 0) initials += Character.toUpperCase(w.charAt(0));
            }
            if (initials.isEmpty()) initials = Character.toUpperCase(name.charAt(0)) + "";
            g.setColor(Color.WHITE);
            g.setFont(new Font("Arial", Font.BOLD, size/3));
            FontMetrics fm = g.getFontMetrics();
            int x = (size - fm.stringWidth(initials)) / 2;
            int y = (size - fm.getHeight()) / 2 + fm.getAscent();
            g.drawString(initials, x, y);
        }
        g.dispose();

        // Сохранение
        ImageIO.write(img, format.toUpperCase(), new File(output));
        System.out.println("✅ Аватарка сохранена в " + output);
    }
}
