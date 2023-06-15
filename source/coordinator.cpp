#include "coordinator.h"

/*
priority list:
    the resume button works
    actual golf game mechanics/ui or whatever, or maybe not a golf game, right now its just like an engine
    ^ mainly level finish
    add more blocks
    art (0~o~0)
    make the course editor useable xdd
    make levels for campaign
*/

void initialize(GameData& g)
{
	txtrStrg().init({"res/error.png", "res/flow chart.png", "res/blockselect.png", "res/blockrec1.png", "res/duck.png", "res/sky1.png", "res/start.png", "res/dragincoursefile.png"});
    fontStrg().init({});

    g.u.initialize(Scene::MAIN);
    g.crsE.initialize();
    g.inga.initialize(g.crs);
}

void update(GameData& g)
{
    bool sceneChange{g.u.update()};

    if (g.u.m_tmpC.isActive())
    {
        g.crs = g.u.m_tmpC.getC();
        g.u.m_tmpC.deleteC();
    }
    if (g.u.m_scn == Scene::EDITOR)
    {
        g.crsE.update(g.crs);
        if (sceneChange)
            g.crsE.initialize();

        if (g.crsE.m_sceneChange == 1)
            g.u.initialize(Scene::MAIN);
        if (g.crsE.m_sceneChange == 2)
            g.u.initialize(Scene::INGAME);
    }
    if (g.u.m_scn == Scene::INGAME)
    {
        g.inga.update(g.crs);
        if (sceneChange)
        {
            if (g.u.m_shouldResumeGame)
                g.u.m_shouldResumeGame = false;
            else
                g.inga.initialize(g.crs);
        }
    }
}

void draw(GameData& g)
{
    if (g.u.m_scn == Scene::EDITOR)
        g.crsE.draw(g.crs);
    if (g.u.m_scn == Scene::INGAME)
        g.inga.draw(g.crs);
    g.u.draw();
}