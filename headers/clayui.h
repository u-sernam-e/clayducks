#ifndef CLAYUI
#define CLAYUI
#include "course.h"
#include "button.h"
#include "textinput.h"
#include "texturestorage.h"
#include <string>
#include <vector>
#include <iostream>

enum class Scene
{
    MAIN,
    CAMPAIGNORCUSTOM,
    COURSESELECT,
    DRAGINCRS,
    INGAME,
    PAUSE,
    WIN,
    LOADORNEW,
    EDITOR
};

class TempCourse
{
private:
    Course crs{};
    bool active{false};
public:
    void setC(const Course& c) {active = true; crs = c;}
    Course getC() {return crs;}
    void deleteC() {active = false; crs = Course{};}
    bool isActive() {return active;}
};

class ClayUi
{
public: // xdd fuck the police fuck the feds fuck the cia fuck the fbi
    std::vector<Button> m_btns;
    Scene m_scn;
    TempCourse m_tmpC;
    bool m_editorMode; // consider using flags/bitset/whatever if theres too many bools eventually
public:
    ClayUi() {}

    void initialize(Scene s);

    bool update(); // true if scene change
    void draw();
};

#endif