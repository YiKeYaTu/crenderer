//
// Created by lc on 2022/2/21.
//

#ifndef CRENDERER_OBJLOADER_HPP
#define CRENDERER_OBJLOADER_HPP

#include "../Vector.hpp"
#include "./util.hpp"
#include <string>
#include <fstream>

class OBJLoader {
public:
    OBJLoader(const std::string &fileName):
            _fileName(fileName), _ifs(fileName) {}

    const std::vector<Vec3f> vertexes() { return _vertexes; }
    const std::vector<std::size_t> indexes() { return _indexes; }

    void loadOBJFile() {
        if (!_ifs.is_open()) {
            throw std::runtime_error("can not open file: " + _fileName);
        }

        std::string line;
        std::vector<std::string> tokens;

        while (std::getline(_ifs, line)) {
            line = util::trim(line);
            if (line[0] == '#') {
                continue;
            }

            tokens = util::split(line, " ");
            assert(tokens.size() == 4);
            switch (line[0]) {
                case 'v':
                    _vertexes.push_back(
                            Vec3f(
                                    std::stod(tokens[1]),
                                    std::stod(tokens[2]),
                                    std::stod(tokens[3])
                                    )
                            );
                    break;
                case 'f':
                    _indexes.push_back(std::stoi(tokens[1]) - 1);
                    _indexes.push_back(std::stoi(tokens[2]) - 1);
                    _indexes.push_back(std::stoi(tokens[3]) - 1);
                    break;
                default:
                    throw std::runtime_error("undefined symbol at the front of: " + line);
            }
        }
    }

private:
    std::ifstream _ifs;
    std::string _fileName;

    std::vector<Vec3f> _vertexes;
    std::vector<std::size_t> _indexes;
};

#endif //CRENDERER_OBJLOADER_HPP
