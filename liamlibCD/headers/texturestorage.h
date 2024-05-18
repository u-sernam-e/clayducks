#ifndef TEXTURESTORAGE
#define TEXTURESTORAGE
#include "rayextended.h"
#include <string>
#include <vector>
#include <array>

class TextureStorage
{
private:
    std::vector<std::pair<Texture2D, std::string>> m_txtrs;
public:
    void init(const std::vector<std::string>& txtrStrs) // call this AFTER you call InitWindow()
    {                                                   // the first texture is recommended to be an error texture
        for (int i{}; i < txtrStrs.size(); ++i)
        {
            m_txtrs.push_back({LoadTexture(txtrStrs[i].c_str()), txtrStrs[i]});
            SetTextureFilter(m_txtrs[i].first, TEXTURE_FILTER_BILINEAR);
        }
    }

    ~TextureStorage()
    {
        for (auto& t : m_txtrs)
        {
            UnloadTexture(t.first);
        }
    }

    const Texture2D& get(std::string s) const
    {
        for (auto& t : m_txtrs)
        {
            if (t.second == s)
                return t.first;
        }
        return m_txtrs[0].first;
    }
};

TextureStorage& txtrStrg(); // im a sneaky snitch, i use a function instead of a pure global variable to get past c++ being stupid asf
                            // also you probably shouldn't make a local TextureStorage, i think it would just waste memory

#endif