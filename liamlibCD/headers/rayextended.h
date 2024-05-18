#ifndef RAYEXTENDED
#define RAYEXTENDED
#include "raylib.h"

Vector2 operator-(const Vector2& a);

Vector2 operator+(const Vector2& a, const Vector2& b);
Vector2 operator-(const Vector2& a, const Vector2& b);
Vector2 operator*(const Vector2& a, const Vector2& b);
Vector2 operator/(const Vector2& a, const Vector2& b);

Vector2& operator+=(Vector2& a, const Vector2& b);
Vector2& operator-=(Vector2& a, const Vector2& b);
Vector2& operator*=(Vector2& a, const Vector2& b);
Vector2& operator/=(Vector2& a, const Vector2& b);

Vector2 operator+(const float& a, const Vector2& b);
Vector2 operator-(const float& a, const Vector2& b);
Vector2 operator*(const float& a, const Vector2& b);
Vector2 operator/(const float& a, const Vector2& b);

Vector2 operator+(const Vector2& a, const float& b);
Vector2 operator-(const Vector2& a, const float& b);
Vector2 operator*(const Vector2& a, const float& b);
Vector2 operator/(const Vector2& a, const float& b);

Vector2& operator+=(Vector2& a, const float& b);
Vector2& operator-=(Vector2& a, const float& b);
Vector2& operator*=(Vector2& a, const float& b);
Vector2& operator/=(Vector2& a, const float& b);

bool operator==(const Vector2& a, const Vector2& b);
bool operator!=(const Vector2& a, const Vector2& b);

Vector2 vecAbs(const Vector2& a);
float vec2distance(const Vector2& a, const Vector2& b);
float lowerLimitFrameTime(); // GetFrameTime(), but only goes down to 15 fps

float vec2ToFloat(Vector2 x); // all of these use degrees
float vec2ToAngle(Vector2 x);
Vector2 floatAngleToVec2(float x, float angle);

Rectangle getRotatedRecBounds(Rectangle rec, float rot, Vector2 anchor); // basically a big unrotated unrectangle that has the rotated rectangle perfectly inside it
bool checkCollisionPointRotatedRec(Vector2 point, Rectangle rec, float rot, Vector2 anchor);
bool checkCollisionCircleRotatedRec(Vector2 pos, float rad, Rectangle rec, float rot, Vector2 anchor);

Vector2 getScreenCenter();

#endif