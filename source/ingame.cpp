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

    bl.stopVels.fill(69696969);
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

    m_strokes = 0;

    m_shad = LoadShader(0, "res/shaders/330/test.fs");
    m_shadTex = LoadRenderTexture(GetScreenWidth(), GetScreenHeight());
    m_shadBallPosLoc = GetShaderLocation(m_shad, "ballPos");
    m_shadBallRadLoc = GetShaderLocation(m_shad, "ballRad");
    m_shadZoomLoc = GetShaderLocation(m_shad, "zoom");
    m_shadScreenSizeLoc = GetShaderLocation(m_shad, "screenSize");
}

bool checkCollisionSpikes(Ball& ball, Block& blo)
{
    float hBoxBigness{6}; // bigger number means bigger hitbox
    float botHBB{hBoxBigness * sqrt(2)}; // hitboxbigness for the bottom of the spikes

    if (blo.size.x <= 300)
    {
        return CheckCollisionPointTriangle(ball.pos, {(blo.pos.x - blo.size.x/2) - botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x + blo.size.x/2) + botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x), blo.pos.y - blo.size.y/2 - hBoxBigness}); // lower left, top, lower right
    }
    else
    {
        int spikeAmount{static_cast<int>(blo.size.x) / 300};           // get the amount of spike
        float spikeSize{blo.size.x / static_cast<float>(spikeAmount)}; // get the size of each spike

        for (int i{}; i < spikeAmount; ++i)
        {
            if (CheckCollisionPointTriangle(ball.pos, {(blo.pos.x - blo.size.x / 2) + spikeSize * i - botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x - blo.size.x / 2) + spikeSize * (i + 1) + botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x - blo.size.x / 2) + spikeSize * (i + 0.5f), blo.pos.y - blo.size.y/2 - hBoxBigness})) // lower left, top, lower right
                return true;
        }
    }
    return false;
}

void drawCollisionSpikes(Block& blo)
{
    float hBoxBigness{6}; // bigger number means bigger hitbox
    float botHBB{hBoxBigness * sqrt(2)}; // hitboxbigness for the bottom of the spikes

    if (blo.size.x <= 300)
    {
        DrawTriangleLines({(blo.pos.x - blo.size.x/2) - botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x + blo.size.x/2) + botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x), blo.pos.y - blo.size.y/2 - hBoxBigness}, RED); // lower left, top, lower right
    }
    else
    {
        int spikeAmount{static_cast<int>(blo.size.x) / 300};           // get the amount of spike
        float spikeSize{blo.size.x / static_cast<float>(spikeAmount)}; // get the size of each spike

        for (int i{}; i < spikeAmount; ++i)
        {
            DrawTriangleLines({(blo.pos.x - blo.size.x / 2) + spikeSize * i - botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x - blo.size.x / 2) + spikeSize * (i + 1) + botHBB, blo.pos.y + blo.size.y/2 + botHBB}, {(blo.pos.x - blo.size.x / 2) + spikeSize * (i + 0.5f), blo.pos.y - blo.size.y/2 - hBoxBigness}, RED); // lower left, top, lower right
        }
    }
}

bool checkCollisionBallBlock(Ball& ball, Block& blo)
{
    switch (blo.type)
    {
        case BlockType::REC:
        case BlockType::BOOSTER:
            return checkCollisionCircleRotatedRec(ball.pos, ball.rad, getBlockRec(blo), blo.rot, blo.pos);
        case BlockType::CIRCLE:
        case BlockType::DUCK:
        case BlockType::BOUNCER:
            return CheckCollisionCircles(ball.pos, ball.rad, blo.pos, blo.size.x/2);
        case BlockType::SPIKES:
            return checkCollisionSpikes(ball, blo);
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

Vector2 getNewVelBooster(Ball& ball, Block& blo, float physDelta)
{
    return floatAngleToVec2(3000 * physDelta, blo.rot - 90);
}

Vector2 getBounceValuesBouncer(Ball& ball, Block& blo, Vector2& newPos)
{
    float maxBounceAmount{(vec2ToFloat(ball.vel) + 1000) / vec2ToFloat(ball.vel)}; // basically adding 1000 velocity
    return reducedAngVel(ball.vel, getBounceValuesCircle(ball, blo, newPos), maxBounceAmount, 1);
}

Vector2 getBounceValues(Ball& ball, Block& blo, Vector2& newPos, float physDelta) // returns new velocity and new, pushed out position
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
            return -ball.vel/2;
        }
        case BlockType::SPIKES:
        {
            ball.state = Ball::State::LOSS;
            return -ball.vel;
        }
        case BlockType::BOUNCER:
            return getBounceValuesBouncer(ball, blo, newPos);
        case BlockType::BOOSTER:
            return getNewVelBooster(ball, blo, physDelta);
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
    std::vector<Vector2> bPoss{}; // bounce positions

    ball.justCollided = false; // set it to false, then set it to true only if it has collided

    if (ball.pos.y > 1000) // checking if the ball went into the water
    {
        ball.state = Ball::State::LOSS;
    }

    for (auto& blo : crs.blocks)
    {
        if (checkCollisionBallBlock(ball, blo))
        {
            Vector2 newPos;
            Vector2 newVel{getBounceValues(ball, blo, newPos, physDelta)};

            if (blo.type != BlockType::BOOSTER && blo.type != BlockType::SPIKES && blo.type != BlockType::DUCK)
            {
                bVels.push_back(newVel); // blocks that bounce are put into bvels, which averages the velocity between them, same with bPos but with position
                bPoss.push_back(newPos);
                ball.justCollided = true; // also only say it collided if it bounced (just collided controls the logic for stopping the ball to be hit)
            }
            else // this is for blocks that add velocity and dont bounce, also booster, spikes, and duck return a velocity to add to ball.vel rather than a new velocity
                ball.vel += newVel;
        }
    }

    Vector2 vTotal{};
    Vector2 pTotal{};

    for (auto& vel : bVels)
    {
        vTotal += vel;
    }

    for (auto& pos : bPoss)
    {
        pTotal += pos;
    }

    if (!bVels.empty())
        ball.vel = vTotal/bVels.size();
    if (!bPoss.empty())
        ball.pos = pTotal/bPoss.size();
}

Vector2 getDisplayBallPos(Ball& bl, PhysFPSController& PFC, Course& crs) // this is buggy
{
    Vector2 displayBallPos{bl.pos}; // estimates the position between physframes
    displayBallPos.x += bl.vel.x * (PFC.getTimeSinceLastFrame()/PFC.getPhysFrameTime()) * PFC.getPhysFrameTime();
    if (!(bl.justCollided && bl.vel.y < 100 && bl.vel.y > -100)) // jitter fix
        displayBallPos.y += bl.vel.y * (PFC.getTimeSinceLastFrame()/PFC.getPhysFrameTime()) * PFC.getPhysFrameTime();

    Ball tempBall{bl};
    tempBall.pos = displayBallPos;

    ballBlockPhysics(tempBall, crs, PFC.getPhysFrameTime()); // to push the tempball out of blocks

    return tempBall.pos;
}

void updateStopVels(Ball& bl, float physDelta)
{
    bl.timeSinceLastVel += physDelta;

    if (bl.timeSinceLastVel >= .1) // we check 10 times a second
    {
        bl.stopVels.at(bl.stopVelIterator) = vec2ToFloat(bl.vel);
        bl.stopVelIterator = (bl.stopVelIterator + 1) % bl.stopVels.size();
        bl.timeSinceLastVel = 0;
    }
}

void resetStopVels(Ball& bl) // puts the stop vels at large numbers
{
    for (auto& vel : bl.stopVels)
    {
        vel = 69420;
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

bool ballStopped(Ball& bl, float blStopSpeed)
{
    return stopVelsLowerThan(bl, blStopSpeed) && bl.justCollided;
}

void InGame::doBallPhysics(Course& crs) // physics stuff
{
    int amountOfPhysFrames{m_PFC.getAmountPhysFrames()}; // we do it like this so that if the fps is lower than the fixed physfps, then the game doesn't slow down
    std::cout << amountOfPhysFrames << '\n';

    for (int i{}; i < amountOfPhysFrames; ++i)
    {
        m_bl.vel.y += M_GRAVITY * m_PFC.getPhysFrameTime(); // gravity

        m_bl.vel = floatAngleToVec2(vec2ToFloat(m_bl.vel), vec2ToAngle(m_bl.vel)); // air resistance
        
        m_bl.pos += m_bl.vel * m_PFC.getPhysFrameTime();

        ballBlockPhysics(m_bl, crs, m_PFC.getPhysFrameTime());

        updateStopVels(m_bl, m_PFC.getPhysFrameTime());
    }
}

void useCameraControlsIngame(Camera2D& cam, Vector2 blPos)
{
    cam.offset = {GetScreenWidth()/2, GetScreenHeight()/2};

    if (cam.zoom * std::pow(1.2, GetMouseWheelMove()) > .01) // limit on how far it can zoom out
        cam.zoom *= std::pow(1.2, GetMouseWheelMove());

    int wasdCamSpeed{100.0f/cam.zoom}; // for pressing wasd

    if (IsKeyDown(KEY_A))
        cam.target.x -= wasdCamSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_D))
        cam.target.x += wasdCamSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_W))
        cam.target.y -= wasdCamSpeed * lowerLimitFrameTime();
    if (IsKeyDown(KEY_S))
        cam.target.y += wasdCamSpeed * lowerLimitFrameTime();


    float ballCamSpeedx{2}; // update cam target to slowly fly towards ball
    float ballCamSpeedy{2};
    
    int margin{100}; // for if the ball is outside camera range
    if (GetWorldToScreen2D(blPos, cam).x > GetScreenWidth() - margin ||
        GetWorldToScreen2D(blPos, cam).x < margin)
            ballCamSpeedx = 5;
    if  (GetWorldToScreen2D(blPos, cam).y > GetScreenHeight() - margin ||
        GetWorldToScreen2D(blPos, cam).y < margin)
            ballCamSpeedy = 5;
    
    cam.target.x += (blPos.x - cam.target.x) * lowerLimitFrameTime() * ballCamSpeedx;
    cam.target.y += (blPos.y - cam.target.y) * lowerLimitFrameTime() * ballCamSpeedy;
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
    
    std::vector<Texture2D> txtrVec{txtrStrg().get("res/sky0.png"), txtrStrg().get("res/sky1.png"), txtrStrg().get("res/sky2.png"), txtrStrg().get("res/sky3.png"), txtrStrg().get("res/sky4.png")};
    //std::vector<Texture2D> txtrVec{txtrStrg().get("res/edgetest.png"), txtrStrg().get("res/sky1.png")}; // this is here for debugging
    drawTextureTilesPro(txtrVec, {0, 0}, m_cam, 3);

    BeginMode2D(m_cam);
    
    /// drawing ball
    DrawTexturePro(txtrStrg().get("res/ball.png"), {0, 0, txtrStrg().get("res/ball.png").width, txtrStrg().get("res/ball.png").height}, {m_displayBallPos.x, m_displayBallPos.y, m_bl.rad*2, m_bl.rad*2}, {m_bl.rad, m_bl.rad}, 0, WHITE);
    DrawTexturePro(txtrStrg().get("res/balloverlay.png"), {0, 0, txtrStrg().get("res/balloverlay.png").width, txtrStrg().get("res/balloverlay.png").height}, {m_displayBallPos.x, m_displayBallPos.y, m_bl.rad*2, m_bl.rad*2}, {m_bl.rad, m_bl.rad}, m_displayBallPos.x*2, {255, 255, 255, 125});

    drawCourseBlocks(crs, getCameraRec(m_cam));
    drawWater(m_cam, 1000);

    EndMode2D();

    EndTextureMode();
}

void InGame::update(Course& crs)
{
    if (ballStopped(m_bl, M_BALLSTOPSPEED)) // ball is stopped
    { 
        m_blPower = (vec2distance(GetWorldToScreen2D(m_bl.pos, m_cam), GetMousePosition()) - 50) / (txtrStrg().get("res/glasspower.png").width/5); // set the blpower
        if (m_blPower > 1)
            m_blPower = 1;
        if (m_blPower < 0)
            m_blPower = 0;

        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && m_blPower != 0)
        {
            m_bl.vel += floatAngleToVec2(m_blPower * 2000, vec2ToAngle(GetScreenToWorld2D(GetMousePosition(), m_cam) - m_bl.pos)); // have animation here for golf club swinging (PROB NOT LOOOOL)
            m_strokes += 1;
            m_bl.justCollided = false;
            resetStopVels(m_bl);
        }
    }
    else
        doBallPhysics(crs);
        
    m_PFC.update();

    m_displayBallPos = getDisplayBallPos(m_bl, m_PFC, crs);
        
    useCameraControlsIngame(m_cam, m_displayBallPos);
    //useCameraControlsEditor(m_cam); // this is here for debugging

    updateShaderThings(crs);
}

void InGame::draw(Course& crs)
{
    BeginShaderMode(m_shad);
    // y-flip texture for glsl
    DrawTextureRec(m_shadTex.texture, {0, 0, m_shadTex.texture.width, -m_shadTex.texture.height}, {0, 0}, WHITE);

    EndShaderMode();

    if (stopVelsLowerThan(m_bl, M_BALLSTOPSPEED))
    {
        Texture2D gltxtr{txtrStrg().get("res/glass.png")};
        Vector2 blScreenPos{GetWorldToScreen2D(m_bl.pos, m_cam)};

        DrawTexturePro(gltxtr, {0, 0, gltxtr.width, gltxtr.height}, {blScreenPos.x, blScreenPos.y, gltxtr.width/5, gltxtr.height/5}, {-50, gltxtr.height/10}, vec2ToAngle(GetScreenToWorld2D(GetMousePosition(), m_cam) - m_bl.pos), WHITE);
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            Texture2D glptxtr{txtrStrg().get("res/glasspower.png")};
            DrawTexturePro(glptxtr, {0, 0, glptxtr.width * m_blPower, glptxtr.height}, {blScreenPos.x, blScreenPos.y, (glptxtr.width/5) * m_blPower, glptxtr.height/5}, {-50, glptxtr.height/10}, vec2ToAngle(GetScreenToWorld2D(GetMousePosition(), m_cam) - m_bl.pos), WHITE);
        }
        
        DrawCircle(50, 50, 50, GREEN);
    }

    DrawText((std::to_string(m_bl.vel.x) + "\n" + std::to_string(m_bl.vel.y) + "\n\n" + 
            std::to_string(vec2ToFloat(m_bl.vel))).c_str(), 10, 10, 20, MAGENTA);
    
    DrawText(("Strokes: " + std::to_string(m_strokes)).c_str(), 10, 150, 30, RED);
}

Ball::State InGame::getState(Course& crs)
{
    return m_bl.state;
}