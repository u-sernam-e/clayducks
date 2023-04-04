#ifndef COORDINATOR
#define COORDINATOR
#include "course.h"
#include "ingame.h"
#include "clayui.h"
#include "texturestorage.h"

// so this file is  the general game coordination,
// controlling what scene the program is in and updating and drawing
// things accordingly

struct GameData
{
    InGame inga;
	Course crs;
    CourseEditor crsE;
    ClayUi u;
};

void initialize(GameData& g);
void update(GameData& g);
void draw(GameData& g);

#endif