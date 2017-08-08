#include "Vector2.h"

Vector2::Vector2() : x(0.0f), y(0.0f) {}
Vector2::Vector2(const Vector2& vector) : x(vector.x), y(vector.y) {}
Vector2::Vector2(float pmx, float pmy) : x(pmx), y(pmy) {}
Vector2::~Vector2() {}