#include "clayui.h"

// I HATE THIS FILE IT HAS SOME OF THE WORST CODE IVE EVER WRITTEN LITERALLY 90% OF THIS SHIT COULD BE AUTOMATED OR WRITTEN IN MUCH MUCH LESS CODE

void ClayUi::initialize(Scene s)
{
    m_scn = s;
    switch (s)
    {
        case Scene::MAIN:
        {
            m_editorMode = false;
            m_btns = {{{60, 60}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "play course", 0},
                    {{60, 200}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "course editor", 0}
#if defined(PLATFORM_DESKTOP)
                    ,{{60, 340}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "quit", 0}
#endif
                    };
            break;
        }
        case Scene::CAMPAIGNORCUSTOM:
        {
            m_btns = {{{60, 60}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "campaign", 0},
                    {{60, 200}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "custom", 0},
                    {{60, 340}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "back", 0}
                    };
            break;
        }
        case Scene::COURSESELECT:
        {
            m_btns = {{{60, 60}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "course 1", 0},
                    {{60, 340}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "back", 0}
                    };
            break;
        }
        case Scene::DRAGINCRS:
        {
            m_btns = {{{60, 340}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "back", 0}
                    };
            break;
        }
        case Scene::INGAME:
        {
            m_btns = {{{GetScreenWidth() - 160, 60}, {100, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "", 0}
                    };
            break;
        }
        case Scene::PAUSE:
        {
            m_btns = {{{60, 60}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "resume", 0},
                    {{60, 200}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "restart", 0},
                    {{60, 340}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "main menu", 0},
                    {{60, 340}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "back to editor", 0}
                    };
            break;
        }
        case Scene::WIN:
        {
            m_btns = {{{60, 200}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "restart", 0},
                    {{60, 340}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "main menu", 0},
                    {{60, 340}, {300, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "back to editor", 0}
                    };
            break;
        }
        case Scene::LOADORNEW:
        {
            m_editorMode = true;
            m_btns = {{{60, 60}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "load", 0},
                    {{60, 200}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "new", 0},
                    {{60, 340}, {600, 100}, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "back", 0}
                    };
            break;
        }
        case Scene::EDITOR: // this is a whole can o' worms in course.h
        {
            m_btns.clear();
            break;
        }
        default:
        {
            break;
        }
    }
}

bool updateUiMain(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released()) 
    {
        u.initialize(Scene::CAMPAIGNORCUSTOM);
        return true;
    }
    if (u.m_btns.at(1).released())
    {
        u.initialize(Scene::LOADORNEW);
        return true;
    }
#if defined(PLATFORM_DESKTOP)
    if (u.m_btns.at(2).released())
    {
        return true; // quit the game
    }
#endif
    return false;
}
void drawUiMain(ClayUi& u)
{
    u.m_btns.at(0).draw();
    u.m_btns.at(1).draw();
#if defined(PLATFORM_DESKTOP)
    u.m_btns.at(2).draw();
#endif
}

bool updateUiCampaignOrCustom(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released()) 
    {
        u.initialize(Scene::COURSESELECT);
        return true;
    }
    if (u.m_btns.at(1).released()) 
    {
        u.initialize(Scene::DRAGINCRS);
        return true;
    }
    if (u.m_btns.at(2).released() || IsKeyReleased(KEY_ESCAPE)) 
    {
        u.initialize(Scene::MAIN);
        return true;
    }
    return false;
}
void drawUiCampaignOrCustom(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.draw();
}

bool updateUiCourseSelect(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released()) 
    {
        u.initialize(Scene::INGAME); // need to set the course
        return true;
    }
    if (u.m_btns.at(1).released() || IsKeyReleased(KEY_ESCAPE)) 
    {
        u.initialize(Scene::CAMPAIGNORCUSTOM);
        return true;
    }
    return false;
}
void drawUiCourseSelect(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.draw();
}

bool updateUiDragInCrs(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released() || IsKeyReleased(KEY_ESCAPE)) // need to specify between whether this is in the editor or not
    {
        if (u.m_editorMode)
        {
            u.initialize(Scene::LOADORNEW);
            return true;
        }
        u.initialize(Scene::CAMPAIGNORCUSTOM);
        return true;
    }
    if (IsFileDropped())
    {
        if (u.m_editorMode)
        {
            u.m_tmpC.setC(loadCourse());
            u.initialize(Scene::EDITOR);
            return true;
        }
        u.m_tmpC.setC(loadCourse());
        u.initialize(Scene::INGAME);
        return true;
    }
    return false;
}
void drawUiDragInCrs(ClayUi& u)
{
    Texture2D txtr{txtrStrg().get("res/dragincoursefile.png")};
    DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {50, 50, GetScreenWidth() - 100, GetScreenHeight() - 100}, {0, 0}, 0, {255, 255, 255, 169});
    for (auto& b : u.m_btns)
        b.draw();
}

bool updateUiInGame(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released() || IsKeyReleased(KEY_ESCAPE)) 
    {
        u.initialize(Scene::PAUSE);
        return true;
    }
    return false;
}
void drawUiInGame(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.draw();
}

bool updateUiPause(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released() || IsKeyReleased(KEY_ESCAPE))
    {
        u.initialize(Scene::INGAME);
        return true;
    }
    if (u.m_btns.at(1).released()) // need to send signal or whatever that the course should restart
    {
        u.initialize(Scene::INGAME);
        return true;
    }
    if (u.m_btns.at(2).released() && !u.m_editorMode)
    {
        u.initialize(Scene::MAIN);
        return true;
    }
    if (u.m_btns.at(3).released() && u.m_editorMode)
    {
        u.initialize(Scene::EDITOR);
        return true;
    }
    return false;
}
void drawUiPause(ClayUi& u)
{
    int i{};
    for (auto& b : u.m_btns)
    {
        if (i == 2 && u.m_editorMode)
            continue;
        if (i == 3 && !u.m_editorMode)
            continue;
        b.draw();
        ++i;
    }
}

bool updateUiWin(ClayUi& u)
{
    u.m_btns.at(0).update();
    if (u.m_btns.at(0).released()) 
    {
        u.initialize(Scene::INGAME);
        return true;
    }
    if (u.m_btns.at(1).released() && !u.m_editorMode) 
    {
        u.initialize(Scene::MAIN);
        return true;
    }
    if (u.m_btns.at(2).released() && u.m_editorMode)
    {
        u.initialize(Scene::EDITOR);
        return true;
    }
    return false;
}
void drawUiWin(ClayUi& u)
{
    int i{};
    for (auto& b : u.m_btns)
    {
        if (i == 1 && u.m_editorMode)
            continue;
        if (i == 2 && !u.m_editorMode)
            continue;
        b.draw();
        ++i;
    }
}

bool updateUiLoadOrNew(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released()) 
    {
        u.initialize(Scene::DRAGINCRS);
        return true;
    }
    if (u.m_btns.at(1).released())
    {
        u.initialize(Scene::EDITOR);
        return true;
    }
    if (u.m_btns.at(2).released() || IsKeyReleased(KEY_ESCAPE))
    {
        u.initialize(Scene::MAIN);
        return true;
    }
    return false;
}
void drawUiLoadOrNew(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.draw();
}

bool updateUiEditor(ClayUi& u) // these to bitches do nothing but i'll leave them here just in case i ever change my mind
{
    for (auto& b : u.m_btns)
        b.update();
    return false;
}
void drawUiEditor(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.draw();
}

bool ClayUi::update()
{
    if (IsKeyPressed(KEY_R))
        initialize(Scene::MAIN);
    switch (m_scn)
    {
        case Scene::MAIN:
            return updateUiMain(*this);
        case Scene::CAMPAIGNORCUSTOM:
            return updateUiCampaignOrCustom(*this);
        case Scene::COURSESELECT:
            return updateUiCourseSelect(*this);
        case Scene::DRAGINCRS:
            return updateUiDragInCrs(*this);
        case Scene::INGAME:
            return updateUiInGame(*this);
        case Scene::PAUSE:
            return updateUiPause(*this);
        case Scene::WIN:
            return updateUiWin(*this);
        case Scene::LOADORNEW:
            return updateUiLoadOrNew(*this);
        case Scene::EDITOR:
            return updateUiEditor(*this);
        default:
            return 0;
    }
}

void ClayUi::draw()
{
    switch (m_scn)
    {
        case Scene::MAIN:
            drawUiMain(*this);
            break;
        case Scene::CAMPAIGNORCUSTOM:
            drawUiCampaignOrCustom(*this);
            break;
        case Scene::COURSESELECT:
            drawUiCourseSelect(*this);
            break;
        case Scene::DRAGINCRS:
            drawUiDragInCrs(*this);
            break;
        case Scene::INGAME:
            drawUiInGame(*this);
            break;
        case Scene::PAUSE:
            drawUiPause(*this);
            break;
        case Scene::WIN:
            drawUiWin(*this);
            break;
        case Scene::LOADORNEW:
            drawUiLoadOrNew(*this);
            break;
        case Scene::EDITOR:
            drawUiEditor(*this);
            break;
        default:
            break;
    }
}