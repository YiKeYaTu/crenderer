//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_OBJLOADER_HPP
#define CRENDERER_OBJLOADER_HPP

#include <string>
#include <fstream>

class OBJLoader {
public:
    OBJLoader(const std::string &fileName):
        fileName(fileName), ifs(fileName) {}

private:
    std::ifstream ifs;
    std::string fileName;

    void loadOBJFile() {
        if (!ifs.is_open()) {
            throw std::runtime_error("can not open file: " + fileName);
        }

        std::string line;

        while (std::getline(ifs, line)) {

        }
    }
};

#endif //CRENDERER_OBJLOADER_HPP
