#ifndef SOUNDSTORAGE
#define SOUNDSTORAGE
#include "rayextended.h"
#include <string>
#include <vector>
#include <array>

class SoundStorage
{
private:
    std::vector<std::pair<Sound, std::string>> m_snds;
public:
    void init(const std::vector<std::string>& sndStrs) // call this AFTER you call InitWindow()
    {                                                   // the first sound is recommended to be an error sound
        for (int i{}; i < sndStrs.size(); ++i)
        {
            m_snds.push_back({LoadSound(sndStrs[i].c_str()), sndStrs[i]});
        }
    }

    ~SoundStorage()
    {
        for (auto& t : m_snds)
        {
            UnloadSound(t.first);
        }
    }

    const Sound& get(std::string s) const
    {
        for (auto& t : m_snds)
        {
            if (t.second == s)
                return t.first;
        }
        return m_snds[0].first;
    }
};

SoundStorage& sndStrg(); // im a sneaky snitch, i use a function instead of a pure global variable to get past c++ being stupid asf
                            // also you probably shouldn't make a local SoundStorage, i think it would just waste memory

#endif