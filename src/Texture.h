#include <iostream>
#include <vector>
#include <algorithm>
#include <HDR/HDRLoader.h>
#include <glad/glad.h>

class Texture
{
public:
    Texture() : width(0), height(0), components(0) {};
    Texture(std::string texName, unsigned char* data, int w, int h, int c);
    ~Texture() { }

    bool LoadTexture(const std::string& filename);

    int width;
    int height;
    int components;
    std::vector<unsigned char> texData;
    std::string name;
};

class HDRMap
{
public:
    HDRMap(const std::string& fileName);

    bool LoadHDRMap(const std::string& fileName);

    int width;
    int height;
    HDRLoaderResult hdrRes;
    GLuint hdrTex;
};