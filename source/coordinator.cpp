#include "coordinator.h"

void initialize(GameData& g)
{
	txtrStrg().init({"res/error.png", "res/flow chart.png", "res/blockselect.png", "res/blockrec1.png", "res/duck.png", "res/sky1.png", "res/start.png", "res/dragincoursefile.png"});

    g.u.initialize(Scene::MAIN);
    g.crsE.initialize();
}

void update(GameData& g)
{
    g.u.update();
    if (g.u.m_tmpC.isActive())
    {
        g.crs = g.u.m_tmpC.getC();
        g.u.m_tmpC.deleteC();
    }
    if (g.u.m_scn == Scene::EDITOR)
        g.crsE.update(g.crs);
    if (g.u.m_scn == Scene::INGAME)
        g.inga.update(g.crs);
}

void draw(GameData& g)
{
    g.u.draw();
    if (g.u.m_scn == Scene::EDITOR)
        g.crsE.draw(g.crs);
    if (g.u.m_scn == Scene::INGAME)
        g.inga.draw(g.crs);
}