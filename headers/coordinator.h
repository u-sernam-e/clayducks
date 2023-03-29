#ifndef COORDINATOR
#define COORDINATOR
#include "course.h"
#include "ball.h"
#include "clayui.h"
#include "texturestorage.h"

// so this file is  the general game coordination,
// controlling what scene the program is in and updating and drawing
// things accordingly

struct GameData
{
    Ball b;
	Course crs;
    CourseEditor crsE;
    ClayUi u;
    Camera2D c;
};

void initialize(GameData& g);
void update(GameData& g);
void draw(GameData& g);

#endif