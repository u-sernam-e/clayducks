#ifndef FONTSTORAGE
#define FONTSTORAGE
#include "rayextended.h"
#include <string>
#include <vector>
#include <array>

class FontStorage
{
private:
    std::vector<std::pair<std::array<Font, 2>, std::string>> m_fonts;
public:
    void init(const std::vector<std::string>& fontStrs) // call this AFTER you call InitWindow()
    {
        for (int i{}; i < fontStrs.size(); ++i)
        {
            m_fonts.push_back({{LoadFontEx(fontStrs[i].c_str(), 256, 0, 0), LoadFontEx(fontStrs[i].c_str(), 12, 0, 0)}, fontStrs[i]});
            SetTextureFilter(m_fonts[i].first[0].texture, TEXTURE_FILTER_BILINEAR);
            SetTextureFilter(m_fonts[i].first[1].texture, TEXTURE_FILTER_BILINEAR);
        }
    }

    ~FontStorage()
    {
        for (auto& f : m_fonts)
        {
            UnloadFont(f.first[0]);
            UnloadFont(f.first[1]);
        }
    }

    const Font& get(std::string s, bool lowQuality) const
    {
        for (auto& t : m_fonts)
        {
            if (t.second == s)
                return (lowQuality ? t.first[1] : t.first[0]);
        }
        return m_fonts[0].first[0];
    }
};

FontStorage& fontStrg(); // im a sneaky snitch, i use a function instead of a pure global variable to get past c++ being stupid asf
                            // also you probably shouldn't make a local FontStorage, i think it would just waste memory

#endif