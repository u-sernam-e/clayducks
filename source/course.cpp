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

void drawTextureTiles(Texture2D txtr, Vector2 origin, Camera2D cam) // this should be inside of camera mode
{
    Rectangle cameraRec{getCameraRec(cam)};

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

std::vector<std::vector<int>> getTileMapforTextureTilesPro(Camera2D cam, Vector2 origin, Vector2 textureSize, int amountTxtrsHorz, int amountTxtrsVert, int numTxtrs) // AOHFEWODSIIUYJYHJHNWEFSYUHBWESFKIUYHGB
{
    static std::vector<std::vector<int>> map{};
    static Vector2 mapPos{floorf((cam.target.x/textureSize.x)/(cam.zoom)), floorf((cam.target.y/textureSize.y)/(cam.zoom))};
    
    if (map.size() != amountTxtrsVert || (!map.empty() && map[0].size() != amountTxtrsHorz)) // when zoom changes (should also be true on first call)
    {
        map.clear(); // clear map

        map.resize(amountTxtrsVert); // put it to the right size
        for (auto& v : map)
        {
            v.resize(amountTxtrsHorz);
        }

        for (int y{}; y < amountTxtrsVert; ++y)
        {
            for (int x{}; x < amountTxtrsHorz; ++x)
            {
                map[y][x] = GetRandomValue(0, numTxtrs - 1); // make cool random textures
            }
        }
    }

    if (Vector2{floorf((cam.target.x/textureSize.x)/(cam.zoom)), floorf((cam.target.y/textureSize.y)/(cam.zoom))} != mapPos) // position not the same
    {
        Vector2 offset{mapPos - Vector2{floorf((cam.target.x/textureSize.x)/(cam.zoom)), floorf((cam.target.y/textureSize.y)/(cam.zoom))}}; // the change in position from last pos
        std::vector<std::vector<int>> mapCopy{map}; // make a copy of the map for copying

        // need to move existing txtrs by the offset
        for (int y{}; y < amountTxtrsVert; ++y)
        {
            for (int x{}; x < amountTxtrsHorz; ++x)
            {
                if (x - offset.x < 0 || y - offset.y < 0 || x - offset.x >= map[0].size() || y - offset.y >= map.size()) // if it's a new texture
                    map[y][x] = GetRandomValue(0, numTxtrs - 1);
                else
                    map[y][x] = mapCopy[y - static_cast<int>(offset.y)][x - static_cast<int>(offset.x)];
            }
        }

        mapPos = Vector2{floorf((cam.target.x/textureSize.x)/(cam.zoom)), floorf((cam.target.y/textureSize.y)/(cam.zoom))}; // set the new position
    }

    return map;
}

void drawTextureTilesPro(std::vector<Texture2D> txtrs, Vector2 origin, Camera2D cam, float parallaxMult) // this should be outside of camera mode, and zooming will look weird `\/^o^\/`, also origin might be broken i haven't tested it
{
    //Rectangle camRec{getCameraRec(cam)};

    cam.target /= parallaxMult / cam.zoom; // parallax changes the cam target

    int amountTxtrsHorz = (GetScreenWidth() / cam.zoom) / txtrs[0].width + 3; // calculate how many textures need to be drawn (maybe +2 can be removed)
    int amountTxtrsVert = (GetScreenHeight() / cam.zoom) / txtrs[0].height + 3;

    Vector2 fuckOffset{GetScreenWidth()/2, GetScreenHeight()/2}; // fuckoff dont ask (the txtr part fixes a fucky thing with getTileMapforTextureTilesPro()
    /*if (cam.target.x < 0)
        fuckOffset.x = -fuckOffset.x;
    if (cam.target.y < 0)
        fuckOffset.y = -fuckOffset.y;
    std::cout << cam.target.x << ", " << cam.target.y << '\n';*/

    Vector2 tLeft{ -cam.target - Vector2{(GetScreenWidth()/2) / cam.zoom, (GetScreenHeight()/2) / cam.zoom} + fuckOffset/cam.zoom}; // top left texture

    tLeft = Vector2{fmod(tLeft.x, txtrs[0].width*cam.zoom), fmod(tLeft.y, txtrs[0].height*cam.zoom)};

    origin = Vector2{fmod(origin.x, txtrs[0].width), fmod(origin.y, txtrs[0].height)}; // make sure the origin isn't too far away (origin changes the offset of the top left texture)

    Vector2 targetOffset{fmod(cam.target.x, txtrs[0].width), fmod(cam.target.y, txtrs[0].height)}; // the offset based on camera position


    std::vector<std::vector<int>> map{getTileMapforTextureTilesPro(cam, origin, Vector2{txtrs[0].width, txtrs[0].height}, amountTxtrsHorz, amountTxtrsVert, txtrs.size())}; // get map for textures to keep *continuity*

    for (int y{}; y < amountTxtrsVert; ++y) // finally, draw :)!
    {
        for (int x{}; x < amountTxtrsHorz; ++x)
        {
            Vector2 eatShitOffset{0, 0}; // again, dont ask (this is because textures get fucked up on the axis)
            if (cam.target.x < 0)
                eatShitOffset.x = -txtrs[0].width;
            if (cam.target.y < 0)
                eatShitOffset.y = -txtrs[0].height;
            
            DrawTextureEx(txtrs[map[y][x]], tLeft + Vector2{((x-1)*txtrs[0].width + eatShitOffset.x)*cam.zoom, ((y-1)*txtrs[0].height + eatShitOffset.y)*cam.zoom}, 0, cam.zoom, WHITE);
        }
    }
}

void drawWater(Camera2D cam, float waterLevel) // inside camera
{
    int txtrWidth{txtrStrg().get("res/watertop.png").width/2};

    static float time{};
    time -= lowerLimitFrameTime()/50; // repeats every 50 sec
    time = fmod(time, 1);

    Rectangle cRec{getCameraRec(cam)};

    float topLeftPos{cRec.x + fmod(time*2, 1)*txtrWidth - txtrWidth*2 - fmod(cam.target.x, txtrWidth)}; // for looping animation (surface moves twice as fast)
    float botLeftPos{cRec.x + time*txtrWidth - txtrWidth*2 - fmod(cam.target.x, txtrWidth)};

    int amountTxtrs{(cRec.width / txtrWidth) + 4}; // amount txtrs horizontolly (assuming surface and depth txtrs have the same width)

    for (int i{1}; i <= amountTxtrs; ++i)
    {
        DrawTexturePro(txtrStrg().get("res/watertop.png"), {0, 0, txtrStrg().get("res/watertop.png").width, txtrStrg().get("res/watertop.png").height}, {topLeftPos + txtrWidth*i, -(txtrStrg().get("res/watertop.png").height/2) + waterLevel, txtrStrg().get("res/watertop.png").width/2, txtrStrg().get("res/watertop.png").height/2}, {0, 0}, 0, WHITE);
        DrawTexturePro(txtrStrg().get("res/waterbot.png"), {0, 0, txtrStrg().get("res/waterbot.png").width, txtrStrg().get("res/waterbot.png").height}, {botLeftPos + txtrWidth*i, waterLevel, txtrStrg().get("res/waterbot.png").width/2, txtrStrg().get("res/waterbot.png").height/2}, {0, 0}, 0, WHITE);
        DrawRectangle(cRec.x, txtrStrg().get("res/waterbot.png").height/2, cRec.width, cRec.height, {27, 50, 84, 255}); // the last bit is the color in the backround of the waterbot texture
    }
}

Texture2D getBlockTexture(BlockType b) // change this if you add different themes
{
    switch (b)
    {
        case BlockType::REC: return txtrStrg().get("res/blockrec.png");
        case BlockType::CIRCLE: return txtrStrg().get("res/blockcircle.png");
        case BlockType::DUCK: return txtrStrg().get("res/duck.png");
        case BlockType::SPIKES: return txtrStrg().get("res/spike.png");
        case BlockType::BOUNCER: return txtrStrg().get("res/bouncer.png");
        case BlockType::BOOSTER: return txtrStrg().get("res/booster.png");
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

void drawSpikes(Block b) // draws a bunch of little spikies
{
    Texture2D txtr{getBlockTexture(b.type)};

    if (b.size.x <= 300)
    {
        DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {b.pos.x, b.pos.y, b.size.x, b.size.y}, b.size/2, b.rot, WHITE);
    }
    else
    {
        int txtrAmount{static_cast<int>(b.size.x) / 300};           // get the amount of textures
        float txtrSize{b.size.x / static_cast<float>(txtrAmount)}; // get the size of each texture

        for (int i{}; i < txtrAmount; ++i)
        {
            DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {(b.pos.x - b.size.x / 2) + txtrSize * i + b.size.x / 2, b.pos.y, txtrSize, b.size.y}, b.size / 2, b.rot, WHITE);
        }
    }
    
}

void drawCourseBlocks(const Course& crs, Rectangle camRec) // this should be inside of camera mode
{
    for (auto& b : crs.blocks)
    {
        if (CheckCollisionRecs(getRotatedRecBounds({b.pos.x-b.size.x/2, b.pos.y-b.size.y/2, b.size.x, b.size.y}, b.rot, b.pos), camRec))
        { // only draw the block if it's bounding box touches the camera
            if (b.type == BlockType::SPIKES) // special special spikes
                drawSpikes(b);
            else
            {
                Texture2D txtr{getBlockTexture(b.type)};
                DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {b.pos.x, b.pos.y, b.size.x, b.size.y}, b.size/2, b.rot, WHITE);
            }
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
    m_blockButt = {{10, 10}, {120, 30}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "block", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 1)};
    m_startButt = {{10, 42}, {120, 30}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "start", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 1)};
    m_buttSelected = -1;
    m_quitButt = {{60, 10}, {50, 30}, RED, {240, 100, 100, 255}, MAROON, "quit", txtrStrg().get("res/button.png"), 1, 1, 0, fontStrg().get("res/font/Days.ttf", 1)};
    m_saveButt = {{60, 42}, {50, 30}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "save", txtrStrg().get("res/button.png"), 1, 1, 0, fontStrg().get("res/font/Days.ttf", 1)};
    m_testButt = {{60, 74}, {50, 30}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "test", txtrStrg().get("res/button.png"), 1, 1, 0, fontStrg().get("res/font/Days.ttf", 1)};
    m_blockButts = {
        {{20, 70}, {80, 50}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "rec", txtrStrg().get("res/button.png"), 1, 0, 1, fontStrg().get("res/font/Days.ttf", 1)},
        {{120, 70}, {80, 50}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "circle", txtrStrg().get("res/button.png"), 1, 0, 1, fontStrg().get("res/font/Days.ttf", 1)},
        {{220, 70}, {80, 50}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "duck", txtrStrg().get("res/button.png"), 1, 0, 1, fontStrg().get("res/font/Days.ttf", 1)},
        {{320, 70}, {80, 50}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "spikes", txtrStrg().get("res/button.png"), 1, 0, 1, fontStrg().get("res/font/Days.ttf", 1)},
        {{420, 70}, {80, 50}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "bouncer", txtrStrg().get("res/button.png"), 1, 0, 1, fontStrg().get("res/font/Days.ttf", 1)},
        {{520, 70}, {80, 50}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "booster", txtrStrg().get("res/button.png"), 1, 0, 1, fontStrg().get("res/font/Days.ttf", 1)}
    };
    m_blockButtSelected = -1;
    m_deleteButt = {{160, 10}, {60, 30}, RED, {240, 100, 100, 255}, MAROON, "delete", txtrStrg().get("res/button.png"), 1, 1, 0, fontStrg().get("res/font/Days.ttf", 0)};
    m_blockSelected = false;
    m_editMode = -1;
    m_quitDialogueActive = false;
    m_quitConfirmButt = {getScreenCenter(), {80, 40}, WHITE, {240, 100, 100, 255}, MAROON, "confirm", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)};
    m_quitCancelButt = {getScreenCenter(), {80, 40}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "cancel", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 0)};
    m_saveDialogueActive = false;
    m_saveTextInput = {getScreenCenter(), {200, 25}, 0, 3};
    m_saveCancelButt = {getScreenCenter(), {60, 25}, WHITE, LIGHTGRAY, {230, 230, 230, 255}, "cancel", txtrStrg().get("res/button.png"), 1, 0, 0, fontStrg().get("res/font/Days.ttf", 1)};
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
        DrawRectangleV(crsE.m_blockButts[crsE.m_blockButtSelected].getPos(), crsE.m_blockButts[crsE.m_blockButtSelected].getSize(), {50, 225, 10, 125});
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

        if (getBlockButtonNumberToBlockType(crsE.m_blockButtSelected) == BlockType::SPIKES)
        {
            Vector2 blockPos{(GetScreenToWorld2D(GetMousePosition(), c) + crsE.m_placingBlockStartingPos)/2};
            if (size.x == crsE.m_minBlockDimension)
                blockPos.x = crsE.m_placingBlockStartingPos.x + crsE.m_minBlockDimension/2;
            if (size.y == crsE.m_minBlockDimension)
                blockPos.y = crsE.m_placingBlockStartingPos.y + crsE.m_minBlockDimension/2;

            drawSpikes(Block{blockPos, size, 0, BlockType::SPIKES, 1});
        }
        else
        {
            Texture2D txtr{getBlockTexture(getBlockButtonNumberToBlockType(crsE.m_blockButtSelected))};
            DrawTexturePro(txtr, {0, 0, txtr.width, txtr.height}, {crsE.m_placingBlockStartingPos.x, crsE.m_placingBlockStartingPos.y, size.x, size.y}, {0, 0}, 0, {255, 255, 255, 180});
        }
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

void useCameraControlsEditor(Camera2D& cam)
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
    DrawTextEx(fontStrg().get("res/font/Days.ttf", 1), "   Are you sure you would\n  like to quit? You will LOSE\nunsaved progress FOREVER.", {getScreenCenter().x - 110, getScreenCenter().y - 70}, 16, 2, WHITE);
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

        useCameraControlsEditor(m_cam);
    }
}

void CourseEditor::draw(const Course& crs)
{
    BeginMode2D(m_cam);

    drawTextureTiles(txtrStrg().get("res/sky1.png"), {0, 0}, m_cam);

    drawCourseBlocks(crs, getCameraRec(m_cam));

    drawWater(m_cam, 1000);
    
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
        DrawRectangle(10, m_buttSelected * 32 + 10, m_blockButt.getSize().x, m_blockButt.getSize().y, {50, 225, 10, 125});

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