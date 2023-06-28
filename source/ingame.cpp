#include "ingame.h"
#include "raymath.h"
#include <utility>
#include <cmath>

/*
todo:
    fix the push out of block for when the ball gets pushed into another block, somehow
    make a rolling mode for when the ball stops bouncing very high (maybe, but it seems to work ok with low physics fps)
*/

void ballInit(Ball& bl, Vector2 start)
{
    bl.state = Ball::ACTIVE;

    bl.pos = start;
    bl.vel = {0, 0};
    bl.rad = 10;

    bl.stopVels.fill(1000);
    bl.stopVelIterator = 0;
    bl.timeSinceLastVel = 0;

    bl.collAng = 0;
    bl.rolling = false;
}

void InGame::initialize(Course& crs)
{
    ballInit(m_bl, crs.start);
    m_cam = {{GetScreenWidth()/2, GetScreenHeight()/2}, crs.start, 0, 1};
    m_PFC.initialize(60);

    m_shad = LoadShader(0, "res/shaders/330/test.fs");
    m_shadTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    m_shadBallPosLoc = GetShaderLocation(m_shad, "ballPos");
    m_shadBallRadLoc = GetShaderLocation(m_shad, "ballRad");
    m_shadZoomLoc = GetShaderLocation(m_shad, "zoom");
    m_shadScreenSizeLoc = GetShaderLocation(m_shad, "screenSize");
}

bool checkCollisionBallBlock(Ball& ball, Block& blo)
{
    switch (blo.type)
    {
        case BlockType::REC:
        case BlockType::BOOSTER:
        case BlockType::SPIKES:
            return checkCollisionCircleRotatedRec(ball.pos, ball.rad, getBlockRec(blo), blo.rot, blo.pos);
        case BlockType::CIRCLE:
        case BlockType::DUCK:
        case BlockType::BOUNCER:
            return CheckCollisionCircles(ball.pos, ball.rad, blo.pos, blo.size.x/2);
        default:
            return false;
    }
}

Vector2 reducedAngVel(Vector2 preV, Vector2 bounceV, float perpMult, float parallelMult) // changes velocity based on the angle it bounces
{
    float angleChange{vec2ToAngle(bounceV) - vec2ToAngle(preV)};
    float velMult{(cosf(angleChange * DEG2RAD) * (parallelMult - perpMult) + parallelMult + perpMult) / 2}; // desmos....
    return floatAngleToVec2(vec2ToFloat(bounceV) * velMult, vec2ToAngle(bounceV));
}

Vector2 getBounceValuesRotRec(Ball& ball, Block& blo, Vector2& newPos) // newPos directly writes over reference put in
{
    Vector2 rotP{blo.pos + floatAngleToVec2(vec2distance(ball.pos, blo.pos), vec2ToAngle(ball.pos - blo.pos) - blo.rot)}; // rotated position
    Vector2 rotV{floatAngleToVec2(vec2ToFloat(ball.vel), vec2ToAngle(ball.vel) - blo.rot)}; // rotated velocity

    Rectangle box{getBlockRec(blo)};

    Vector2 collV{  (rotP.x < box.x ? -1 : 
                    (rotP.x > box.x + box.width ? 1 : 0)),
                    (rotP.y < box.y ? -1 : 
                    (rotP.y > box.y + box.height ? 1 : 0))}; // collision vector, if its on the left, x is -1, right 1, inside is 0, same for y

    Vector2 collisionPos{};

    if (collV.x * collV.y == 0) // its not intersecting a corner
    {
        float radPushOutMultiplier{1};

        if (collV.x + collV.y == 0) // center inside box
        {
            // which lines to check intersection width, it should only be the two opposite of velocity
            float yOffset{rotV.x > 0 ? 0 : box.width};
            std::pair<Vector2, Vector2> lineY{Vector2{box.x + yOffset, box.y}, Vector2{box.x + yOffset, box.y + box.height}};
            float xOffset{rotV.y > 0 ? 0 : box.height};
            std::pair<Vector2, Vector2> lineX{Vector2{box.x, box.y + xOffset}, Vector2{box.x + box.width, box.y + xOffset}};
            
            std::pair<Vector2, Vector2> collLine{rotP - floatAngleToVec2(box.width + box.height, vec2ToAngle(rotV)), rotP};

            Vector2 FUCKINGUSELESSOTHERCOLLPOS;

            if (CheckCollisionLines(collLine.first, collLine.second, lineY.first, lineY.second, &FUCKINGUSELESSOTHERCOLLPOS)) // horizontol
            {
                rotV = {-rotV.x, rotV.y};
                if (yOffset == 0)
                {
                    rotP.x = box.x - ball.rad * radPushOutMultiplier;
                    collisionPos = {rotP.x + ball.rad, rotP.y};
                }
                else
                {
                    rotP.x = box.x + box.width + ball.rad * radPushOutMultiplier;
                    collisionPos = {rotP.x - ball.rad, rotP.y};
                }
            }
            else if (CheckCollisionLines(collLine.first, collLine.second, lineX.first, lineX.second, &FUCKINGUSELESSOTHERCOLLPOS)) // vertical
            {
                rotV = {rotV.x, -rotV.y};
                if (xOffset == 0)
                {
                    rotP.y = box.y - ball.rad * radPushOutMultiplier;
                    collisionPos = {rotP.x, rotP.y + ball.rad};
                }
                else
                {
                    rotP.y = box.y + box.height + ball.rad * radPushOutMultiplier;
                    collisionPos = {rotP.x, rotP.y - ball.rad};
                }
            }
        }
        else if (collV.y == 0) // intersecting horizontally
        {
            rotV = {-rotV.x, rotV.y};
            float x = (collV.x + 1)/2; // 1 or 0
            rotP.x = box.x + (box.width * x) + (ball.rad * radPushOutMultiplier * collV.x);

            collisionPos = {rotP.x + ball.rad - 2*(ball.rad * x), rotP.y};
        } 
        else if (collV.x == 0) // intersecting vertically
        {
            rotV = {rotV.x, -rotV.y};
            float y = (collV.y + 1)/2; // 1 or 0
            rotP.y = box.y + (box.height * y) + (ball.rad * radPushOutMultiplier * collV.y);

            collisionPos = {rotP.x, rotP.y + ball.rad - 2*(ball.rad * y)};
        }
    } // dont like the repeating code ^^^^, but fuck it
    else // colliding corner
    {
        collisionPos = {box.x + (collV.x == -1 ? 0 : box.width), box.y + (collV.y == -1 ? 0 : box.height)}; // corner

        rotP = collisionPos + floatAngleToVec2(ball.rad, vec2ToAngle(rotP - collisionPos));

        Vector2 newVel{rotV.x, -rotV.y};
        rotV = floatAngleToVec2(vec2ToFloat(newVel), vec2ToAngle(newVel) + (2 * vec2ToAngle(rotP - collisionPos)) + 180); 
    }

    // now set new pos to counter-rotated new position, and same with returned new velocity

    ball.collAng = vec2ToAngle(rotP - collisionPos) + blo.rot;

    newPos = blo.pos + floatAngleToVec2(vec2distance(rotP, blo.pos), vec2ToAngle(rotP - blo.pos) + blo.rot);

    return floatAngleToVec2(vec2ToFloat(rotV), vec2ToAngle(rotV) + blo.rot);
}

Vector2 getBounceValuesCircle(Ball& ball, Block& blo, Vector2& newPos)
{
    ball.collAng = vec2ToAngle(ball.pos - blo.pos);

    newPos = blo.pos + floatAngleToVec2(ball.rad + blo.size.x/2, ball.collAng);

    Vector2 newVel{ball.vel.x, -ball.vel.y};
    return floatAngleToVec2(vec2ToFloat(newVel), vec2ToAngle(newVel) + (2 * (ball.collAng)) + 180);
}

Vector2 getNewVelBooster(Ball& ball, Block& blo)
{
    Vector2 rotV{floatAngleToVec2(vec2ToFloat(ball.vel), vec2ToAngle(ball.vel) - blo.rot)}; // rotated velocity
    rotV.y -= 300'000 * lowerLimitFrameTime();
    return floatAngleToVec2(vec2ToFloat(rotV), vec2ToAngle(rotV) + blo.rot);
}

Vector2 getBounceValuesBouncer(Ball& ball, Block& blo, Vector2& newPos)
{
    float maxBounceAmount{(vec2ToFloat(ball.vel) + 1000) / vec2ToFloat(ball.vel)}; // basically adding 1000 velocity
    return reducedAngVel(ball.vel, getBounceValuesCircle(ball, blo, newPos), maxBounceAmount, 1);
}

Vector2 getBounceValues(Ball& ball, Block& blo, Vector2& newPos) // returns new velocity and new, pushed out position
{
    switch (blo.type)
    {
        case BlockType::REC:
            return reducedAngVel(ball.vel, getBounceValuesRotRec(ball, blo, newPos), .5, .99);
        case BlockType::CIRCLE:
            return reducedAngVel(ball.vel, getBounceValuesCircle(ball, blo, newPos), .5, .99);
        case BlockType::DUCK:
        {
            ball.state = Ball::State::WIN;
            return ball.vel/2;
        }
        case BlockType::SPIKES:
        {
            ball.state = Ball::State::LOSS;
            return {0, 0};
        }
        case BlockType::BOUNCER:
            return getBounceValuesBouncer(ball, blo, newPos);
        case BlockType::BOOSTER:
            return getNewVelBooster(ball, blo);
        default:
            return ball.vel;
    }
}

void ballBlockPhysics(Ball& ball, Course& crs, float physDelta)
{
    // first check collision with all the blocks
    // then go though all the blocks that are colliding with the ball and find the bounce velocity
    // then get the average bounce velocity apply it
    // for bumpers, it would multiply the bounce velocity
    // for boosters, just add velocity in direction

    std::vector<Vector2> bVels{}; // bounce velocities

    for (auto& blo : crs.blocks)
    {
        if (checkCollisionBallBlock(ball, blo))
        {
            Vector2 bbb{getBounceValues(ball, blo, ball.pos)};
            bVels.push_back(bbb);
        }
    }

    Vector2 total{};

    for (auto& vel : bVels)
    {
        total += vel;
    }

    if (!bVels.empty())
        ball.vel = total/bVels.size();
}

Vector2 getDisplayBallPos(Ball& bl, PhysFPSController& PFC, Course& crs) // this is buggy
{
    Vector2 displayBallPos {bl.pos + (bl.vel * (PFC.getTimeSinceLastFrame()/PFC.getPhysFrameTime()) * PFC.getPhysFrameTime())};
    Ball tempBall{bl};
    tempBall.pos = displayBallPos;
    for (auto& blo : crs.blocks)
    {
        if (checkCollisionBallBlock(tempBall, blo))
        {
            getBounceValues(tempBall, blo, displayBallPos);
        }
    }
    return displayBallPos;
}

void updateStopVels(Ball& bl, float physDelta)
{
    bl.timeSinceLastVel += physDelta;

    if (bl.timeSinceLastVel >= 1/bl.stopVels.size())
    {
        bl.timeSinceLastVel -= 1/bl.stopVels.size();
        bl.stopVels.at(bl.stopVelIterator) = vec2ToFloat(bl.vel);
        bl.stopVelIterator = (bl.stopVelIterator + 1) % bl.stopVels.size();
    }
}

bool stopVelsLowerThan(Ball& bl, float upperLimit)
{
    float total{};

    for (auto& f : bl.stopVels)
    {
        total += f;
    }

    return total/bl.stopVels.size() < upperLimit;
}

void InGame::updateShaderThings(Course& crs)
{
    // set shader values
    Vector2 displayBallScreenPos{GetWorldToScreen2D(m_displayBallPos, m_cam) * Vector2{1, -1} + Vector2{0, GetScreenHeight()}};
    Vector2 screenSize{GetScreenWidth(), GetScreenHeight()};

    SetShaderValue(m_shad, m_shadBallPosLoc, &displayBallScreenPos, SHADER_UNIFORM_VEC2);
    SetShaderValue(m_shad, m_shadBallRadLoc, &m_bl.rad, SHADER_UNIFORM_FLOAT);
    SetShaderValue(m_shad, m_shadZoomLoc, &m_cam.zoom, SHADER_UNIFORM_FLOAT);
    SetShaderValue(m_shad, m_shadScreenSizeLoc, &screenSize, SHADER_UNIFORM_VEC2);
    if (IsKeyPressed(KEY_G)) // reload shader
    {
        m_shad = LoadShader(0, "res/shaders/330/test.fs");
        m_shadBallPosLoc = GetShaderLocation(m_shad, "ballPos");
        m_shadBallRadLoc = GetShaderLocation(m_shad, "ballRad");
        m_shadZoomLoc = GetShaderLocation(m_shad, "zoom");
        m_shadScreenSizeLoc = GetShaderLocation(m_shad, "screenSize");
    }

    // do this here because its drawing to a texture
    if (IsWindowResized())
        m_shadTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());

    BeginTextureMode(m_shadTex);
    BeginMode2D(m_cam);

    drawTextureTiles(txtrStrg().get("res/sky1.png"), {0, 0}, getCameraRec(m_cam));

    drawCourseBlocks(crs, getCameraRec(m_cam));

    DrawCircleV(m_displayBallPos, m_bl.rad, BLUE);

    EndMode2D();
    EndTextureMode();
}

void InGame::update(Course& crs)
{
    m_PFC.update();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && stopVelsLowerThan(m_bl, 1000)) // click make ball move
        m_bl.vel += (GetScreenToWorld2D(GetMousePosition(), m_cam) - m_bl.pos) * 5;

    if (m_PFC.isPhysFrame()) // physics stuff
    {
        m_bl.vel.y += 3000 * m_PFC.getPhysFrameTime();

        ballBlockPhysics(m_bl, crs, m_PFC.getPhysFrameTime());

        m_bl.pos += m_bl.vel * m_PFC.getPhysFrameTime();

        updateStopVels(m_bl, m_PFC.getPhysFrameTime());
    }

    useCameraControls(m_cam);
    m_displayBallPos = getDisplayBallPos(m_bl, m_PFC, crs);

    float camSpeed{2}; // update cam target to slowly fly towards ball
    m_cam.target += floatAngleToVec2(vec2distance(m_displayBallPos, m_cam.target) * lowerLimitFrameTime() * camSpeed, vec2ToAngle(m_displayBallPos - m_cam.target));

    updateShaderThings(crs);
}

void InGame::draw(Course& crs)
{
    BeginShaderMode(m_shad);
    // y-flip texture for glsl
    DrawTextureRec(m_shadTex.texture, {0, 0, m_shadTex.texture.width, -m_shadTex.texture.height}, {0, 0}, WHITE);

    EndShaderMode();
    
    //DrawCircleV(GetWorldToScreen2D(m_bl.pos + floatAngleToVec2(m_bl.rad, m_bl.collAng + 180), m_cam), 3, BLACK);

    DrawText((std::to_string(m_bl.vel.x) + "\n" + std::to_string(m_bl.vel.y) + "\n\n" + 
            std::to_string(vec2ToFloat(m_bl.vel))).c_str(), 10, 10, 20, MAGENTA);
    
    timerifj += lowerLimitFrameTime();
    if (std::abs(vec2ToFloat(m_bl.vel)) > max)
        max = std::abs(vec2ToFloat(m_bl.vel));
    if (timerifj > 1)
    {
        std::cout << max << '\n';
        timerifj = 0;
        max = 0;
    }
}

Ball::State InGame::getState(Course& crs)
{
    return m_bl.state;
}