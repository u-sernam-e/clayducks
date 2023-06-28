#include "course.h"
#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif
#include <algorithm>
#include <fstream>

/* TO DO:
make the ui for creating level nicer - have the mode where you can edit blocks be the default then make block mode into block placing mode
make it so you can place blocks to the up and to the left
WELL HAVE FUN, IM OFF TO MAKE THE MOTHERFUCKING GAME!

also eventually add more blocks
*/
Course loadCourse(std::ifstream& inf)
{
    Course output{};

    inf.seekg(0, std::ios::end);
    if (inf.tellg() > sizeof(output.start) + sizeof(output.par))
    {
        inf.seekg(0, std::ios::beg);

        inf.read(reinterpret_cast<char*>(&output.start), sizeof(output.start));
        inf.read(reinterpret_cast<char*>(&output.par), sizeof(output.par));
        int curBlock{};
        while (inf)
        {
            output.blocks.push_back({});
            inf.read(reinterpret_cast<char*>(&output.blocks[curBlock]), sizeof(output.blocks[curBlock]));
            ++curBlock;
        }
        output.blocks.resize(curBlock - 1);

        inf.close();
    }


    return output;
}

Course loadCourseFromFile(std::string fileName)
{
    std::ifstream inf{fileName.c_str()};
    return loadCourse(inf);
}

Course loadDroppedCourse() // returns empty course if no files are dropped
{
    std::ifstream inf{};

    if (IsFileDropped())
    {
            FilePathList droppedFiles = LoadDroppedFiles();

            inf.open(droppedFiles.paths[0]);

            UnloadDroppedFiles(droppedFiles);    // Unload filepaths from memory
    }

    return loadCourse(inf);
}

void saveCourse(Course& crs, std::string name) // adds .claycrs to name AND OVERWRITES FILE SO BE CAREFUL
{
    std::ofstream outf{name + ".claycrs"};

    outf.write(reinterpret_cast<char*>(&crs.start), sizeof(crs.start));
    outf.write(reinterpret_cast<char*>(&crs.par), sizeof(crs.par));
    for (auto& b : crs.blocks)
    {
        outf.write(reinterpret_cast<char*>(&b), sizeof(b));
    }
    outf.close();

#if defined(PLATFORM_WEB)
    emscripten_run_script(("saveFileFromMEMFSToDisk('" + name + ".claycrs','" + name + ".claycrs')").c_str());
#endif
}

Rectangle getCameraRec(const Camera2D& cam)
{
    return {cam.target.x - cam.offset.x / cam.zoom, cam.target.y - cam.offset.y / cam.zoom, cam.offset.x * 2 / cam.zoom, cam.offset.y * 2 / cam.zoom};
}

void drawTextureTiles(Texture2D txtr, Vector2 origin, Rectangle cameraRec) // this should be inside of camera mode
{
    std::vector<Vector2> txtrsToDraw{};
    // calculating the upper left txtr
    Vector2 cameraOffsetByOrigin{Vector2{cameraRec.x, cameraRec.y} - origin};
    Vector2 bitToOffsetThingyBy{Vector2{std::fmod(cameraOffsetByOrigin.x, txtr.width), std::fmod(cameraOffsetByOrigin.y, txtr.height)}};
    if (bitToOffsetThingyBy.x < 0 || bitToOffsetThingyBy.y < 0)
        bitToOffsetThingyBy += Vector2{txtr.width, txtr.height};
    txtrsToDraw.push_back(cameraOffsetByOrigin - bitToOffsetThingyBy + origin);
    // putting as many textures as can fit in the cameraRec
    while(txtrsToDraw[txtrsToDraw.size() - 1].x < cameraRec.x + cameraRec.width)
    {
        txtrsToDraw.push_back(txtrsToDraw[txtrsToDraw.size() - 1] + Vector2{txtr.width});
    }
    int txtrsPerRow{txtrsToDraw.size()};
    while(txtrsToDraw[txtrsToDraw.size() - 1].y < cameraRec.y + cameraRec.height)
    {
        float rowY{txtrsToDraw[txtrsToDraw.size() - 1].y + txtr.height};
        for (int i{}; i < txtrsPerRow; ++i)
            txtrsToDraw.push_back(Vector2{txtrsToDraw[0].x + txtr.width * i, rowY});
    }
    // draw the textures
    for (auto& t : txtrsToDraw)
    {
        DrawTextureV(txtr, t, WHITE);
    }
}

Texture2D getBlockTexture(BlockType b) // change this if you add different themes
{
    switch (b)
    {
        case BlockType::REC: return txtrStrg().get("res/blockrec1.png");
        case BlockType::CIRCLE: return txtrStrg().get("res/blockcircle1.png");
        case BlockType::DUCK: return txtrStrg().get("res/duck.png");
        case BlockType::SPIKES: return txtrStrg().get("res/spike1.png");
        case BlockType::BOUNCER: return txtrStrg().get("res/bouncer1.png");
        case BlockType::BOOSTER: return txtrStrg().get("res/booster1.png");
        default: return txtrStrg().get("res/error.png");
    }
}

BlockType getBlockButtonNumberToBlockType(int n)
{
    switch(n)
    {
        case 0: return BlockType::REC;
        case 1: return BlockType::CIRCLE;
        case 2: return BlockType::DUCK;
        case 3: return BlockType::SPIKES;
        case 4: return BlockType::BOUNCER;
        case 5: return BlockType::BOOSTER;
        default: return BlockType::REC;
    }
}

Rectangle getBlockRec(Block b)
{
    return {b.pos.x - b.size.x/2, b.pos.y - b.size.y/2, b.size.x, b.size.y};
}

Rectangle getRotateCollBlockRec(Block b) // for the little rotation circle
{
    return {b.pos.x - b.size.x * (6/182.0f), b.pos.y - b.size.y * (100/182.0f), b.size.x * (12/182.0f), b.size.y * (9/182.0f)};
}

bool checkCollisionForBlockScaling(Vector2 p, Block b)
{
    std::vector<Rectangle> recs{Rectangle{b.pos.x - b.size.x * (100/182.0f), b.pos.y - b.size.y * (100/182.0f), b.size.x * (20/182.0f), b.size.y * (20/182.0f)},
        Rectangle{b.pos.x + b.size.x * (80/182.0f), b.pos.y - b.size.y * (100/182.0f), b.size.x * (20/182.0f), b.size.y * (20/182.0f)},
        Rectangle{b.pos.x + b.size.x * (80/182.0f), b.pos.y + b.size.y * (80/182.0f), b.size.x * (20/182.0f), b.size.y * (20/182.0f)},
        Rectangle{b.pos.x - b.size.x * (100/182.0f), b.pos.y + b.size.y * (80/182.0f), b.size.x * (20/182.0f), b.size.y * (20/182.0f)}
    };
    for (auto& r : recs)
    {
        if (checkCollisionPointRotatedRec(p, r, b.rot, b.pos))
            return true;
    }
 
    return false;
}

void drawCourseBlocks(const Course& crs, Rectangle camRec) // this should be inside of camera mode
{
    for (auto& b : crs.blocks)
    {
        if (CheckCollisionRecs(getRotatedRecBounds({b.pos.x-b.size.x/2, b.pos.y-b.size.y/2, b.size.x, b.size.y}, b.rot, b.pos), camRec))
        { // only draw the block if it's bounding box touches the camera
            Texture2D txtr{getBlockTexture(b.type)};
            DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {b.pos.x, b.pos.y, b.size.x, b.size.y}, b.size/2, b.rot, WHITE);
        }
    }
}

bool anyBlockButtonsHovered(CourseEditor& crsE)
{
    for (auto& b : crsE.m_blockButts)
    {
        if (b.hover() || b.down())
            return true;
    }
    return false;
}

bool anyButtonsHovered(CourseEditor& crsE)
{
    return crsE.m_blockButt.hover() || crsE.m_blockButt.down() ||
        crsE.m_startButt.hover() || crsE.m_startButt.down() ||
        crsE.m_quitButt.hover() || crsE.m_quitButt.down() ||
        crsE.m_saveButt.hover() || crsE.m_saveButt.down() ||
        crsE.m_testButt.hover() || crsE.m_testButt.down() ||
        (crsE.m_blockSelected && (crsE.m_deleteButt.hover() || crsE.m_deleteButt.down())) ||
        (crsE.m_buttSelected == 0 && anyBlockButtonsHovered(crsE));
}

void putBlockAtEnd(std::vector<Block>& bs, int i)
{
    Block b{bs.at(i)};
    bs.erase(bs.begin() + i);
    bs.push_back(b);
}

void CourseEditor::initialize()
{
    m_cam = {{GetScreenWidth()/2, GetScreenHeight()/2}, {0, 0}, 0, 1};
    m_blockButt = {{10, 10}, {120, 20}, 0, 0, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "block", 0};
    m_startButt = {{10, 32}, {120, 20}, 0, 0, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "start", 0};
    m_buttSelected = -1;
    m_quitButt = {{60, 10}, {50, 20}, 1, 0, txtrStrg().get("res/error.png"), RED, {240, 100, 100, 255}, MAROON, "quit", 0};
    m_saveButt = {{60, 32}, {50, 20}, 1, 0, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "save", 0};
    m_testButt = {{60, 54}, {50, 20}, 1, 0, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "test", 0};
    m_blockButts = {
        {{70, 70}, {50, 50}, 0, 1, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "rec", 0},
        {{140, 70}, {50, 50}, 0, 1, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "circle", 0},
        {{210, 70}, {50, 50}, 0, 1, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "duck", 0},
        {{280, 70}, {50, 50}, 0, 1, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "spikes", 0},
        {{350, 70}, {50, 50}, 0, 1, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "bouncer", 0},
        {{420, 70}, {50, 50}, 0, 1, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "booster", 0}
    };
    m_blockButtSelected = -1;
    m_deleteButt = {{160, 10}, {60, 30}, 1, 0, txtrStrg().get("res/error.png"), RED, {240, 100, 100, 255}, MAROON, "delete", 0};
    m_blockSelected = false;
    m_editMode = -1;
    m_quitDialogueActive = false;
    m_quitConfirmButt = {getScreenCenter(), {80, 40}, 0, 0, txtrStrg().get("res/error.png"), BLACK, {240, 100, 100, 255}, MAROON, "confirm", 0};
    m_quitCancelButt = {getScreenCenter(), {80, 40}, 0, 0, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "cancel", 0};
    m_saveDialogueActive = false;
    m_saveTextInput = {getScreenCenter(), {200, 25}, 0, 3};
    m_saveCancelButt = {getScreenCenter(), {60, 25}, 0, 0, txtrStrg().get("res/error.png"), BLACK, LIGHTGRAY, GRAY, "cancel", 0};
    m_sceneChange = 0;
}

void updateCourseEditorBlockEditingMode(CourseEditor& crsE, Course& crs, Camera2D& c)
{
    Vector2 camMouse{GetScreenToWorld2D(GetMousePosition(), c)}; // pos of mouse in the camera

    if (!crsE.m_blockSelected) // no block currently selected
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !anyButtonsHovered(crsE))
        {
            int blkSeld{-1};
            for (int i{}; i < crs.blocks.size(); ++i)
            {
                if (checkCollisionPointRotatedRec(camMouse, getBlockRec(crs.blocks.at(i)), crs.blocks.at(i).rot, crs.blocks.at(i).pos))
                    blkSeld = i;
            }
            if (blkSeld != -1)
            {
                putBlockAtEnd(crs.blocks, blkSeld);
                crsE.m_blockSelected = true;
            }
        }
    }
    else // there is a block selected
    {
        Block& cBlk{crs.blocks.at(crs.blocks.size() - 1)};

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !anyButtonsHovered(crsE))
        {
            if (checkCollisionForBlockScaling(camMouse, cBlk) && !anyButtonsHovered(crsE)) // scaling
            {
                crsE.m_editMouseStartPos = camMouse;
                crsE.m_editBlockStartPos = cBlk.size;
                crsE.m_editMode = 2;
            }
            else if (checkCollisionPointRotatedRec(camMouse, getRotateCollBlockRec(cBlk), cBlk.rot, cBlk.pos) && !anyButtonsHovered(crsE)) // rotating
            {
                crsE.m_editMouseStartPos = camMouse;
                crsE.m_editBlockStartPos.x = cBlk.rot;
                crsE.m_editMode = 1;
            }
            else if (checkCollisionPointRotatedRec(camMouse, getBlockRec(cBlk), cBlk.rot, cBlk.pos) && !anyButtonsHovered(crsE)) // moving
            {
                crsE.m_editMouseStartPos = camMouse;
                crsE.m_editBlockStartPos = cBlk.pos;
                crsE.m_editMode = 0;
            }
            else // checking if the block should be unselected
            {
                int newBlkSelctd{-1};
                for (int i{}; i < crs.blocks.size(); ++i)
                {
                    if (checkCollisionPointRotatedRec(camMouse, getBlockRec(crs.blocks.at(i)), crs.blocks.at(i).rot, crs.blocks.at(i).pos) && !anyButtonsHovered(crsE))
                        newBlkSelctd = i;
                }
                if (newBlkSelctd == -1)
                    crsE.m_blockSelected = false;
                else
                {
                    putBlockAtEnd(crs.blocks, newBlkSelctd);
                }
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            switch (crsE.m_editMode)
            {
                case 0: // moving
                {
                    cBlk.pos = crsE.m_editBlockStartPos + camMouse - crsE.m_editMouseStartPos;
                    break;
                }
                case 1: // rotating
                {
                    cBlk.rot = crsE.m_editBlockStartPos.x + vec2ToAngle(camMouse - cBlk.pos) - vec2ToAngle(crsE.m_editMouseStartPos - cBlk.pos);
                    break;
                }
                case 2: // scaling
                {
                    Vector2 unRotMouse{cBlk.pos + floatAngleToVec2(vec2distance(cBlk.pos, camMouse), vec2ToAngle(camMouse - cBlk.pos) - cBlk.rot)};
                    if (cBlk.type == BlockType::CIRCLE || cBlk.type == BlockType::DUCK || cBlk.type == BlockType::BOUNCER) // if its circular (duck, circle, and bumper) then the x and y have to stay the same
                    {
                        float size{std::max(std::abs(cBlk.pos.x - unRotMouse.x), std::abs(cBlk.pos.y - unRotMouse.y)) * 2};
                        if (size < crsE.m_minBlockDimension) size = crsE.m_minBlockDimension;
                        cBlk.size = Vector2{size, size};
                    }
                    else
                    {
                        cBlk.size = vecAbs(cBlk.pos - unRotMouse)*2;
                        if (cBlk.size.x < crsE.m_minBlockDimension) cBlk.size.x = crsE.m_minBlockDimension;
                        if (cBlk.size.y < crsE.m_minBlockDimension) cBlk.size.y = crsE.m_minBlockDimension;
                    }
                }
                default:
                    break;
            }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            crsE.m_editMode = -1;
        }

        crsE.m_deleteButt.update();
        if (crsE.m_deleteButt.released())
        {
            crs.blocks.pop_back();
            crsE.m_blockSelected = false;
        }
    }
}

void updateCourseEditorBlockPlacingMode(CourseEditor& crsE, Course& crs, Camera2D& c)
{
    if (crsE.m_currentlyPlacingBlock)
    {
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT))
        {
            Vector2 size{GetScreenToWorld2D(GetMousePosition(), c) - crsE.m_placingBlockStartingPos};
            if (size.x < crsE.m_minBlockDimension) size.x = crsE.m_minBlockDimension;
            if (size.y < crsE.m_minBlockDimension) size.y = crsE.m_minBlockDimension;

            BlockType btype = getBlockButtonNumberToBlockType(crsE.m_blockButtSelected);
            if (btype == BlockType::CIRCLE || btype == BlockType::DUCK || btype == BlockType::BOUNCER) // if its circular (duck, circe, and bouncer) then the x and y have to stay the same
            {
                float biggerD{std::max(size.x, size.y)};
                size = Vector2{biggerD, biggerD};
            }

            crs.blocks.push_back(Block{crsE.m_placingBlockStartingPos + size/2, size, 0, getBlockButtonNumberToBlockType(crsE.m_blockButtSelected), 1});
            crsE.m_currentlyPlacingBlock = false;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_RIGHT))
        {
            crsE.m_currentlyPlacingBlock = false;
        }
    }
    else
    {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !anyButtonsHovered(crsE))
        {
            crsE.m_placingBlockStartingPos = GetScreenToWorld2D(GetMousePosition(), c);
            crsE.m_currentlyPlacingBlock = true;
        }
    }
}

void updateCourseEditorBlockMode(CourseEditor& crsE, Course& crs, Camera2D& c)
{
    if (IsKeyPressed(KEY_ESCAPE))
    {
        if (crsE.m_blockButtSelected == -1)
        {
            crsE.m_buttSelected = -1;
            return;
        }
        else
        {
            crsE.m_blockButtSelected = -1;
            crsE.m_currentlyPlacingBlock = false;
        }
    }
    for (int i{}; i < crsE.m_blockButts.size(); ++i)
    {
        crsE.m_blockButts.at(i).update();
        if (crsE.m_blockButts.at(i).released())
        {
            if (crsE.m_blockButtSelected == i) // block button unselected
            {
                crsE.m_blockButtSelected = -1;
                crsE.m_currentlyPlacingBlock = false;
            }
            else // block button selected
            {
                crsE.m_blockButtSelected = i;
                crsE.m_currentlyPlacingBlock = false;
                crsE.m_blockSelected = false;
            }
        }
    }
    if (crsE.m_blockButtSelected == -1)
        updateCourseEditorBlockEditingMode(crsE, crs, c);
    else
        updateCourseEditorBlockPlacingMode(crsE, crs, c);
}
void drawCourseEditorBlockMode(CourseEditor& crsE, const Course& crs, Camera2D& c)
{
    for (auto& b : crsE.m_blockButts)
    {
        b.draw();
    }
    if (crsE.m_blockButtSelected != -1)
    {
        DrawCircle(95 + 70 * crsE.m_blockButtSelected, GetScreenHeight() - 95, 10, PURPLE);
    }
    if (crsE.m_blockSelected)
    {
        crsE.m_deleteButt.draw();
    }

    BeginMode2D(c);
    if (crsE.m_currentlyPlacingBlock)
    {
        Vector2 size{GetScreenToWorld2D(GetMousePosition(), c) - crsE.m_placingBlockStartingPos};

        if (size.x < crsE.m_minBlockDimension) size.x = crsE.m_minBlockDimension;
        if (size.y < crsE.m_minBlockDimension) size.y = crsE.m_minBlockDimension;

        BlockType btype = getBlockButtonNumberToBlockType(crsE.m_blockButtSelected);
        if (btype == BlockType::CIRCLE || btype == BlockType::DUCK || btype == BlockType::BOUNCER) // if its circular (duck, circle, and bumper) then the x and y have to stay the same
        {
            float biggerD{std::max(size.x, size.y)};
            size = Vector2{biggerD, biggerD};
        }

        Texture2D txtr{getBlockTexture(getBlockButtonNumberToBlockType(crsE.m_blockButtSelected))};
        DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {crsE.m_placingBlockStartingPos.x, crsE.m_placingBlockStartingPos.y, size.x, size.y}, {0, 0}, 0, {255, 255, 255, 180});
    }
    EndMode2D();
}

void updateCourseEditorStartMode(CourseEditor& crsE, Course& crs, Camera2D& c)
{
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !anyButtonsHovered(crsE))
    {
        crs.start = GetScreenToWorld2D(GetMousePosition(), c);
    }
}
void drawCourseEditorStartMode(CourseEditor& crsE, const Course& crs, Camera2D& c)
{
    DrawTextureV(txtrStrg().get("res/start.png"), GetMousePosition() - Vector2{txtrStrg().get("res/start.png").width/2, txtrStrg().get("res/start.png").height/2}, {0, 200, 0, 190});
}

void useCameraControls(Camera2D& cam)
{
    cam.offset = {GetScreenWidth()/2, GetScreenHeight()/2};

    if (cam.zoom * std::pow(1.2, GetMouseWheelMove()) > .01) // limit on how far it can zoom out
        cam.zoom *= std::pow(1.2, GetMouseWheelMove());

    int cameraSpeed{500.0f/cam.zoom};
    if (IsKeyDown(KEY_A))
        cam.target.x -= cameraSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_D))
        cam.target.x += cameraSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_W))
        cam.target.y -= cameraSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_S))
        cam.target.y += cameraSpeed * lowerLimitFrameTime();
}

std::string getCameraInfo(const Camera2D& cam)
{
    return {"offset:     " + std::to_string(cam.offset.x) + ", " + std::to_string(cam.offset.y) + 
            "\ntarget:   " + std::to_string(cam.target.x) + ", " + std::to_string(cam.target.y) +
            "\nrotation: " + std::to_string(cam.rotation) + 
            "\nzoom:     " + std::to_string(cam.zoom)};
}

void updateQuitQuestion(CourseEditor& cse)
{
    cse.m_quitConfirmButt.setPos(getScreenCenter() + Vector2{-95, 15});
    cse.m_quitCancelButt.setPos(getScreenCenter() + Vector2{15, 15});

    cse.m_quitConfirmButt.update();
    cse.m_quitCancelButt.update();

    if (cse.m_quitConfirmButt.released())
        cse.m_sceneChange = 1;

    if (cse.m_quitCancelButt.released() || IsKeyPressed(KEY_ESCAPE))
        cse.m_quitDialogueActive = false;
}

void drawQuitQuestion(CourseEditor& cse)
{
    DrawRectangleV(getScreenCenter() + Vector2{-120, -75}, {240, 150}, {0, 0, 0, 200});
    DrawText("   Are you sure you would\n  like to quit? You will LOSE\nunsaved progress FOREVER.", getScreenCenter().x - 110, getScreenCenter().y - 70, 16, WHITE);
    cse.m_quitConfirmButt.draw();
    cse.m_quitCancelButt.draw();
}

void updateSaveQuestion(CourseEditor& cse, Course& crs)
{
    cse.m_saveTextInput.setPos(getScreenCenter() + Vector2{-100, -10});
    cse.m_saveCancelButt.setPos(getScreenCenter() + Vector2{40, 22});

    cse.m_saveTextInput.update();
    cse.m_saveCancelButt.update();

    if (cse.m_saveTextInput.justSubmitted())
    {
        saveCourse(crs, cse.m_saveTextInput.getInputTxt());
        cse.m_saveTextInput.setSelect(false);
        cse.m_saveDialogueActive = false;
    }

    if (cse.m_saveCancelButt.released() || IsKeyPressed(KEY_ESCAPE))
    {
        cse.m_saveDialogueActive = false;
    }
}

void drawSaveQuestion(CourseEditor& cse)
{
    DrawRectangleV(getScreenCenter() + Vector2{-120, -55}, {240, 110}, {0, 0, 0, 200});
    DrawText("Enter name of map.", getScreenCenter().x - 70, getScreenCenter().y - 40, 16, WHITE);
    cse.m_saveTextInput.draw();
    cse.m_saveCancelButt.draw();
}

void CourseEditor::update(Course& crs)
{
    if (m_quitDialogueActive) // add other diologues etc
    {
        updateQuitQuestion(*this);
    }
    else if (m_saveDialogueActive)
    {
        updateSaveQuestion(*this, crs);
    }
    else
    {
        m_blockButt.update();
        m_startButt.update();

        m_quitButt.update();
        m_saveButt.update();
        m_testButt.update();


        if (m_quitButt.released())
            m_quitDialogueActive = true;
        if (m_saveButt.released())
        {
            m_saveDialogueActive = true;
            m_saveTextInput.setSelect(true);
        }
        if (m_testButt.released())
            m_sceneChange = 2;


        if (m_blockButt.released())
        {
            if (m_buttSelected == 0)
                m_buttSelected = -1;
            else
                m_buttSelected = 0;
        }
        if (m_startButt.released())
        {
            if (m_buttSelected == 1)
                m_buttSelected = -1;
            else
                m_buttSelected = 1;
        }

        switch (m_buttSelected)
        {
            case 0:
                updateCourseEditorBlockMode(*this, crs, m_cam);
                break;
            case 1:
                updateCourseEditorStartMode(*this, crs, m_cam);
                break;
            default:
                break;
        }

        useCameraControls(m_cam);
    }
}

void CourseEditor::draw(const Course& crs)
{
    BeginMode2D(m_cam);

    drawTextureTiles(txtrStrg().get("res/sky1.png"), {0, 0}, getCameraRec(m_cam));

    drawCourseBlocks(crs, getCameraRec(m_cam));
    
    for (int i{}; i < crs.blocks.size(); ++i)
    {
        Block b{crs.blocks.at(i)};

        if (i == crs.blocks.size() - 1 && m_blockSelected && m_buttSelected == 0)
        {
            Texture2D selTxtr{txtrStrg().get("res/blockselect.png")};
            DrawTexturePro(selTxtr, {0, 0, selTxtr.width, selTxtr.height}, {b.pos.x, b.pos.y, b.size.x*1.1, b.size.y*1.1}, (b.size/2)*1.1, b.rot, WHITE);
        }
    }

    EndMode2D();

    DrawTextureV(txtrStrg().get("res/start.png"), GetWorldToScreen2D(crs.start, m_cam) - Vector2{txtrStrg().get("res/start.png").width/2, txtrStrg().get("res/start.png").height/2}, WHITE);

    m_blockButt.draw();
    m_startButt.draw();

    m_quitButt.draw();
    m_saveButt.draw();
    m_testButt.draw();

    if (m_buttSelected != -1)
        DrawCircle(145, m_buttSelected * 22 + 20, 10, GREEN);

    switch (m_buttSelected)
    {
        case 0:
            drawCourseEditorBlockMode(*this, crs, m_cam);
            break;
        case 1:
            drawCourseEditorStartMode(*this, crs, m_cam);
            break;
        default:
            break;
    }
    
    if (m_quitDialogueActive)
        drawQuitQuestion(*this);
    if (m_saveDialogueActive)
        drawSaveQuestion(*this);
}