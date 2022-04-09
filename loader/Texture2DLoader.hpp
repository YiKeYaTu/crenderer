//
// Created by lc on 2022/4/7.
//

#ifndef CRD_Texture2DLoader_HPP
#define CRD_Texture2DLoader_HPP

#include <unordered_map>
#include <string>
#include <stb_image/stb_image.hpp>
#include <loader/TextureLoader.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.hpp>

class Texture2DLoader: public TextureLoader {
private:
    inline static std::unordered_map<std::string, Texture2DLoader> _loaders = std::unordered_map<std::string, Texture2DLoader>();

    int _width, _height, _channel;
    unsigned char* _data;

public:
    Texture2DLoader(const std::string& path): TextureLoader(TextureType::TEXTURE_2D) {
        _data = stbi_load(path.c_str(), &_width, &_height, &_channel, 0);
    }

    ~Texture2DLoader() {
        stbi_image_free(_data);
    }

    int width() const { return _width; }
    int height() const { return _height; }
    int channel() const { return _channel; }
    virtual const unsigned char* data() const override { return _data; }

    Texture2DLoader(const Texture2DLoader& rhs) = delete;
    Texture2DLoader& operator=(const Texture2DLoader& rhs) = delete;

    static const Texture2DLoader& load(const std::string& path) {
        if (_loaders.count(path) == 0) {
            _loaders.emplace(path, path);
        }
        return _loaders.find(path)->second;
    }
    static void unload(const std::string& path) {
        if (_loaders.count(path)) {
            _loaders.erase(path);
        }
    }
};

#endif //CRD_Texture2DLoader_HPP
