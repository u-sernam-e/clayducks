#ifndef COURSE
#define COURSE
#include "button.h"
#include "textinput.h"
#include "texturestorage.h"
#include "rayextended.h"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

enum class BlockType
{
    REC,
    CIRCLE,
    DUCK,
    SPIKES,
    BOUNCER,
    BOOSTER
};

struct Block // BE CAREFUL ABOUT CHANGING THIS, IT WILL CORRUPT ALREADY MADE COURSES
{
    Vector2 pos;
    Vector2 size;
    float rot;

    BlockType type;

    float intensity; // controls booster boost amount && bouncer bounce amount, also currently unused
};

struct Course
{
    std::vector<Block> blocks;
    Vector2 start;
    int par;
};

struct CourseEditor
{
public:
    const int m_minBlockDimension{20};

    Camera2D m_cam;

    Button m_blockButt;
    Button m_startButt;
    int m_buttSelected; // in the order of ^^: -1 for nothing selected, 0 for blockButt, 1 for startButt, etc.

    Button m_quitButt;
    Button m_saveButt;
    Button m_testButt;

    std::vector<Button> m_blockButts;
    int m_blockButtSelected; // same as with buttselected

    bool m_currentlyPlacingBlock;
    Vector2 m_placingBlockStartingPos;

    Button m_deleteButt;
    bool m_blockSelected;
    int m_editMode; // -1 for nothing, 0 for move, 1 for rotate, 2 for scale
    Vector2 m_editMouseStartPos; // might be unneeded
    Vector2 m_editBlockStartPos; // for rotations, just use x

    bool m_quitDialogueActive;
    Button m_quitConfirmButt;
    Button m_quitCancelButt;

    bool m_saveDialogueActive;
    TextInput m_saveTextInput;
    Button m_saveCancelButt;

    int m_sceneChange; // 0 for no change, 1 for main menu, 2 for ingame
public:
    void initialize();
    void update(Course& crs);
    void draw(const Course& crs);
};


Course loadCourseFromFile(std::string fileName);
Course loadDroppedCourse(); // drag and drop file
void saveCourse(const Course& crs, std::string name);

Rectangle getCameraRec(const Camera2D& cam);
void drawTextureTiles(Texture2D txtr, Vector2 origin, Camera2D cam); // inside camera mode
void drawTextureTilesPro(std::vector<Texture2D> txtrs, Vector2 origin, Camera2D cam, float parallaxMult); // outside camera mode, txtrs should all be the same width/height
void drawWater(Camera2D cam, float waterLevel); // inside camera
Texture2D getBlockTexture(BlockType b);
void drawCourseBlocks(const Course& crs, Rectangle camRec);
Rectangle getBlockRec(Block b);
std::string getCameraInfo(const Camera2D& cam);

void useCameraControlsEditor(Camera2D& cam); // in here mostly for debug

#endif