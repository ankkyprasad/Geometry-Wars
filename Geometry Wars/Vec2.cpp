#include "Vec2.h"
#include <cmath>

Vec2::Vec2() { }

Vec2::Vec2(float x, float y) : x(x), y(y) { }

bool Vec2::operator == (const Vec2& rhs) const {
	return this->x == rhs.x && this->y == rhs.y;
}

bool Vec2::operator != (const Vec2& rhs) const {
	return this->x != rhs.x || this->y != rhs.y;
}

Vec2 Vec2::operator + (const Vec2& rhs) const {
	return Vec2(this->x + rhs.x, this->y + rhs.y);
}

Vec2 Vec2::operator - (const Vec2& rhs) const {
	return Vec2(this->x - rhs.x, this->y + rhs.y);
}

Vec2 Vec2::operator * (const float val) const {
	return Vec2(this->x * val, this->y * val);
}

Vec2 Vec2::operator / (const float val) const {
	return Vec2(this->x / val, this->y / val);
}

void Vec2::operator += (const Vec2& rhs) {
	this->x += rhs.x;
	this->y += rhs.y;
}

void Vec2::operator -= (const Vec2& rhs) {
	this->x -= rhs.x;
	this->y -= rhs.y;
}

void Vec2::operator *= (const float val) {
	this->x *= val;
	this->y *= val;
}

void Vec2::operator /= (const float val) {
	this->x /= val;
	this->y /= val;
}

float Vec2::dist(const Vec2& rhs) const {
	float d1 = (rhs.x - this->x) * (rhs.x - this->x);
	float d2 = (rhs.y - this->y) * (rhs.y - this->y);
	return sqrtf(d1 + d2);
}
