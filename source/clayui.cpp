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
            m_btns = {{{60, 200}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "play course", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "course editor", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::CAMPAIGNORCUSTOM:
        {
            m_btns = {{{60, 60}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "campaign", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 200}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "custom", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "back", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::COURSESELECT:
        {
            m_btns = {{{60, 60}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "course 1", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "back", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::DRAGINCRS:
        {
            m_btns = {{{60, 340}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "back", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::INGAME:
        {
            m_strokes = -1;
            m_btns = {{{160, 60}, {100, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "||", txtrStrg().get("res/button.png"), 1, 1, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::PAUSE:
        {
            m_btns = {{{60, 60}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "resume", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 200}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "restart", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "main menu", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "back to editor", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::FINISH: // need to add image at top like won, died, new best strokes, etc
        {
            m_btns = {{{60, 200}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "restart", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "main menu", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {300, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "back to editor", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
                    };
            break;
        }
        case Scene::LOADORNEW:
        {
            m_editorMode = true;
            m_btns = {{{60, 60}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "load", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 200}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "new", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)},
                    {{60, 340}, {600, 100}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "back", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)}
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
        Course c{}; // reset course to empty course
        u.m_tmpC.setC(c);
        u.initialize(Scene::CAMPAIGNORCUSTOM);
        return true;
    }
    if (u.m_btns.at(1).released())
    {
        Course c{};
        u.m_tmpC.setC(c);
        u.initialize(Scene::LOADORNEW);
        return true;
    }
    return false;
}
void drawUiMain(ClayUi& u)
{
    DrawTexture(txtrStrg().get("res/background.png"), 0, 0, WHITE);
    u.m_btns.at(0).draw();
    u.m_btns.at(1).draw();
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
    DrawTexture(txtrStrg().get("res/background.png"), 0, 0, WHITE);
    for (auto& b : u.m_btns)
        b.draw();
}

bool updateUiCourseSelect(ClayUi& u)
{
    for (auto& b : u.m_btns)
        b.update();
    if (u.m_btns.at(0).released()) 
    {
#if defined(PLATFORM_WEB)
        u.m_tmpC.setC(loadCourseFromFile("res/campaign/1.claycrs"));
#else
        u.m_tmpC.setC(loadCourseFromFile("res/campaign/1desk.claycrs"));
#endif
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
    DrawTexture(txtrStrg().get("res/background.png"), 0, 0, WHITE);
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
            u.m_tmpC.setC(loadDroppedCourse());
            u.initialize(Scene::EDITOR);
            return true;
        }
        u.m_tmpC.setC(loadDroppedCourse());
        u.initialize(Scene::INGAME);
        return true;
    }
    return false;
}
void drawUiDragInCrs(ClayUi& u)
{
    DrawTexture(txtrStrg().get("res/background.png"), 0, 0, WHITE);
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
        u.m_shouldResumeGame = true;
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
    int i{-1};
    for (auto& b : u.m_btns)
    {
        ++i;
        if (i == 2 && u.m_editorMode)
            continue;
        if (i == 3 && !u.m_editorMode)
            continue;
        b.draw();
    }
}

bool updateUiFinish(ClayUi& u)
{
    u.m_btns.at(0).update();
    if (!u.m_editorMode)
        u.m_btns.at(1).update();
    else
        u.m_btns.at(2).update();

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
void drawUiFinish(ClayUi& u)
{
    if (u.m_strokes == -1)
    {
        DrawText("lose :(", 10, 10, 20, DARKBLUE);
    }
    else
    {
        if (u.m_editorMode)
            DrawText("New par:", 10, 10, 20, WHITE); // its too late i have to go to bed
        else
            DrawText("Win!", 10, 10, 20, WHITE);
        DrawText(std::to_string(u.m_strokes).c_str(), 10, 40, 20, WHITE);
    }


    int i{-1};
    for (auto& b : u.m_btns)
    {
        ++i;
        if (i == 1 && u.m_editorMode)
            continue;
        if (i == 2 && !u.m_editorMode)
            continue;
        b.draw();
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
    DrawTexture(txtrStrg().get("res/background.png"), 0, 0, WHITE);
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
        case Scene::FINISH:
            return updateUiFinish(*this);
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
        case Scene::FINISH:
            drawUiFinish(*this);
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