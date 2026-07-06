// avatar.cpp
#include <opencv2/opencv.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <random>

using namespace std;
using namespace cv;

vector<int> hashToColor(unsigned int hash) {
    return {(hash >> 16) & 0xFF, (hash >> 8) & 0xFF, hash & 0xFF};
}

unsigned int hashString(const string& str) {
    unsigned int hash = 0;
    for (char c : str) {
        hash = hash * 31 + c;
    }
    return hash;
}

void generateAvatar(const string& name, const string& output, int size=256,
                    const string& fmt="png", const string& mainColor="",
                    const string& bgColor="", bool addText=false) {
    unsigned int nameHash = hashString(name);
    vector<int> mainCol = hashToColor(nameHash);
    vector<int> bgCol = hashToColor(nameHash + 12345);

    // Парсинг цветов, если заданы
    if (!mainColor.empty()) {
        // упрощённо: только HEX
        if (mainColor[0] == '#') {
            int r = stoi(mainColor.substr(1,2), nullptr, 16);
            int g = stoi(mainColor.substr(3,2), nullptr, 16);
            int b = stoi(mainColor.substr(5,2), nullptr, 16);
            mainCol = {r, g, b};
        }
    }
    if (!bgColor.empty()) {
        if (bgColor[0] == '#') {
            int r = stoi(bgColor.substr(1,2), nullptr, 16);
            int g = stoi(bgColor.substr(3,2), nullptr, 16);
            int b = stoi(bgColor.substr(5,2), nullptr, 16);
            bgCol = {r, g, b};
        }
    }

    Mat img(size, size, CV_8UC3, Scalar(bgCol[0], bgCol[1], bgCol[2]));
    int margin = size / 20;
    circle(img, Point(size/2, size/2), size/2 - margin, Scalar(mainCol[0], mainCol[1], mainCol[2]), -1);

    if (addText) {
        // Добавляем инициалы
        string initials;
        stringstream ss(name);
        string word;
        while (ss >> word) {
            if (!word.empty()) initials += toupper(word[0]);
        }
        if (initials.empty()) initials = toupper(name[0]);
        int fontFace = FONT_HERSHEY_SIMPLEX;
        double fontScale = size / 200.0;
        int thickness = size / 50;
        int baseline;
        Size textSize = getTextSize(initials, fontFace, fontScale, thickness, &baseline);
        Point org((size - textSize.width)/2, (size + textSize.height)/2);
        putText(img, initials, org, fontFace, fontScale, Scalar(255,255,255), thickness, LINE_AA);
    }

    vector<int> params;
    if (fmt == "jpg" || fmt == "jpeg") {
        params.push_back(IMWRITE_JPEG_QUALITY);
        params.push_back(90);
    }
    imwrite(output, img, params);
    cout << "✅ Аватарка сохранена в " << output << endl;
}

int main(int argc, char* argv[]) {
    string name, output = "avatar.png", mainColor, bgColor, fmt = "png";
    int size = 256;
    bool addText = false, randomGen = false;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-o" && i+1 < argc) output = argv[++i];
        else if (arg == "-s" && i+1 < argc) size = stoi(argv[++i]);
        else if (arg == "-f" && i+1 < argc) fmt = argv[++i];
        else if (arg == "-c" && i+1 < argc) mainColor = argv[++i];
        else if (arg == "-b" && i+1 < argc) bgColor = argv[++i];
        else if (arg == "-t") addText = true;
        else if (arg == "-r") randomGen = true;
        else if (arg == "-h" || arg == "--help") {
            cout << "Usage: avatar [name] [options]\n"
                 << "  -o <file>   Output file\n"
                 << "  -s <N>      Size\n"
                 << "  -f <fmt>    Format (png,jpg,webp)\n"
                 << "  -c <color>  Main color\n"
                 << "  -b <color>  Background color\n"
                 << "  -t          Add text\n"
                 << "  -r          Random\n";
            return 0;
        } else if (name.empty()) name = arg;
    }

    if (randomGen) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<> dis(1000, 9999);
        name = "User" + to_string(dis(gen));
    }
    if (name.empty()) {
        cerr << "Укажите имя или используйте --random" << endl;
        return 1;
    }

    generateAvatar(name, output, size, fmt, mainColor, bgColor, addText);
    return 0;
}
