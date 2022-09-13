#ifndef TEXTURE_H
#define TEXTURE_H

#include <glad/glad.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <HDR/HDRLoader.h>

class Texture
{
public:
    Texture() : width(0), height(0), components(0), texId(-1), texLoc(0) {};
    Texture(std::string texName, unsigned char* data, int w, int h, int c);
    Texture(std::string fileName) { LoadTexture(fileName); }
    ~Texture() { }

    bool LoadTexture(const std::string& filename);
    void bindGPU(int _texLoc);
    int width;
    int height;
    int components;
    std::vector<unsigned char> texData;
    std::string name;
    GLuint texId;      // 生成和绑定纹理单元
    int texLoc; // GL_TEXTURE0 ~ 32;
};

class HDRMap
{
public:
    HDRMap(const std::string& fileName);

    bool LoadHDRMap(const std::string& fileName);

    HDRLoaderResult hdrRes;
};

#endif // !TEXTURE_H
