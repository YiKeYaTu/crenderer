//
// Created by lc on 2022/2/24.
//

#ifndef CRENDERER_UTIL_HPP
#define CRENDERER_UTIL_HPP

#include <vector>
#include <cmath>
#include <string>

namespace util {
std::vector<std::string> split(std::string& s, const std::string& separator) {
    std::vector<std::string> tokens;

    for (int i = 0, j; ; i = j + separator.length()) {
        j = s.find(separator, i);
        if (j == -1) {
            tokens.push_back(s.substr(i, s.size() - i));
            break;
        } else {
            tokens.push_back(s.substr(i, j - i));
        }
    }

    return tokens;
}

std::string trim(std::string& s) {
    int i = 0, j = s.size() - 1;

    while (i < s.size()) {
        if (s[i] != ' ') break;
        i ++;
    }
    while (j > i) {
        if (s[j] != ' ') break;
        j --;
    }

    return s.substr(i, j - i + 1);
}

double clamp(double min, double max, double val) {
    return std::fmax(min, std::fmin(max, val));
}

void saveFrameBufferAsPPM(std::vector<Vec3f>& frameBuffer, int width, int height) {
    FILE* fp = fopen("binary.ppm", "wb");
    (void)fprintf(fp, "P6\n%d %d\n255\n", width, height);
    for (auto i = 0; i < height * width; ++i) {
        static unsigned char color[3];
        color[0] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].x()), 0.6f));
        color[1] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].y()), 0.6f));
        color[2] = (unsigned char)(255 * std::pow(util::clamp(0, 1, frameBuffer[i].z()), 0.6f));
        fwrite(color, 1, 3, fp);
    }
    fclose(fp);
}
}

#endif //CRENDERER_UTIL_HPP
