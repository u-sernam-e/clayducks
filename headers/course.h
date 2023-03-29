#ifndef COURSE
#define COURSE
#include "button.h"
#include "texturestorage.h"
#include "rayextended.h"
#include <vector>
#include <string>
#include <cmath>
#include <iostream>

enum class BlockType
{
    REC,
    DUCK
    /*to do:
    TRIANGLE
    CIRCLE
    SPIKES
    BOUNCER
    BOOSTER*/ 
};

struct Block
{
    Vector2 pos;
    Vector2 size;
    float rot;
    BlockType type;
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
public:
    void initialize();
    void update(Course& crs, Camera2D& c);
    void draw(const Course& crs, Camera2D& c);
};


Course loadCourse(); // drag and drop file
void saveCourse(const Course& crs, std::string name);

#endif