#include "coordinator.h"

/*
priority list:
    soft speed limit for booster/bouncer
    collision detection using a line between ballpos in physics frames for high speeds/thin blocks
    rolling

    permanent floor
    better background
    spikes texture

    actual golf game mechanics/ui or whatever, or maybe not a golf game, right now its just like an engine
    ^ mainly level finish
    add more blocks
    art (0~o~0)
    make the course editor useable xdd
    make levels for campaign
*/

void initialize(GameData& g)
{
	txtrStrg().init({"res/error.png", "res/background.png", "res/button.png", "res/watertop.png", "res/waterbot.png", "res/ball.png", "res/balloverlay.png", "res/edgetest.png", "res/flow chart.png", "res/blockselect.png", "res/blockrec.png", "res/blockcircle.png", "res/duck.png", "res/booster.png", "res/bouncer.png", "res/spike.png", "res/sky0.png", "res/sky1.png", "res/sky2.png", "res/sky3.png", "res/sky4.png", "res/sky1.png", "res/sky2.png", "res/sky3.png", "res/sky0.png", "res/start.png", "res/dragincoursefile.png", "res/glass.png", "res/glasspower.png"});
    fontStrg().init({"res/font/Roboto.ttf"});

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
        {
            g.u.initialize(Scene::MAIN);
            sceneChange = true;
        }
        if (g.crsE.m_sceneChange == 2)
        {
            g.u.initialize(Scene::INGAME);
            sceneChange = true;
        }
    }
    if (g.u.m_scn == Scene::INGAME)
    {
        g.inga.update(g.crs);
        if (sceneChange && !g.u.m_shouldResumeGame)
            g.inga.initialize(g.crs);
        if (g.u.m_shouldResumeGame)
            g.u.m_shouldResumeGame = false;

        switch (g.inga.getState(g.crs))
        {
            case Ball::State::WIN:
            {
                if (g.u.m_editorMode) // setting par lol
                    g.crs.par = g.inga.getStrokes();
                g.u.m_strokes = g.inga.getStrokes();
                g.u.initialize(Scene::FINISH);
                break;
            }
            case Ball::State::LOSS:
            {
                g.u.m_strokes = -1;
                g.u.initialize(Scene::FINISH);
                break;
            }
            default:
                break;
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