//
// Created by lc on 2022/4/7.
//

#ifndef CRD_TEXTURELOADER_HPP
#define CRD_TEXTURELOADER_HPP

enum TextureType {
    TEXTURE_2D
};

class TextureLoader {
protected:
    TextureType _textureType;
public:
    virtual const unsigned char* data() const = 0;
    TextureType textureType() const { return _textureType; }

    TextureLoader(TextureType textureType): _textureType(textureType) {}
};

#endif //CRD_TEXTURELOADER_HPP
