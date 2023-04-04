#include "ingame.h"


void InGame::update(Course& crs)
{
    m_bl.pos = GetMousePosition();

    useCameraControls(m_cam);
}

void InGame::draw(Course& crs)
{
    drawTextureTiles(txtrStrg().get("res/sky1.png"), {0, 0}, getCameraRec(m_cam));


    BeginMode2D(m_cam);

    drawCourseBlocks(crs, getCameraRec(m_cam));

    EndMode2D();


    DrawCircleV(m_bl.pos, 10, BLUE);
}