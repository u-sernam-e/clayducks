#include "texturestorage.h"
#include "fontstorage.h"
#include "soundstorage.h"
#include "musicstorage.h"

TextureStorage& txtrStrg()
{
    static TextureStorage t{};
    return t;
}

FontStorage& fontStrg()
{
    static FontStorage f{};
    return f;
}

SoundStorage& sndStrg()
{
    static SoundStorage s{};
    return s;
}

MusicStorage& mscStrg()
{
    static MusicStorage m{};
    return m;
}