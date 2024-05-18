#ifndef MUSICSTORAGE
#define MUSICSTORAGE
#include "rayextended.h"
#include <string>
#include <vector>
#include <array>

class MusicStorage
{
private:
    std::vector<std::pair<Music, std::string>> m_mscs;
public:
    void init(const std::vector<std::string>& mscStrs) // call this AFTER you call InitWindow()
    {                                                   // the first music is recommended to be an error music
        for (int i{}; i < mscStrs.size(); ++i)
        {
            m_mscs.push_back({LoadMusicStream(mscStrs[i].c_str()), mscStrs[i]});
        }
    }

    ~MusicStorage()
    {
        for (auto& t : m_mscs)
        {
            UnloadMusicStream(t.first);
        }
    }

    Music& get(std::string s)
    {
        for (auto& t : m_mscs)
        {
            if (t.second == s)
                return t.first;
        }
        return m_mscs[0].first;
    }
};

MusicStorage& mscStrg(); // im a sneaky snitch, i use a function instead of a pure global variable to get past c++ being stupid asf
                            // also you probably shouldn't make a local MusicStorage, i think it would just waste memory

#endif