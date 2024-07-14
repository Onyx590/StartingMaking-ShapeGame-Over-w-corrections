#include "Vec2.h"
#include <cmath>

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
	: x(xin), y(yin)
{}

bool Vec2::operator== (const Vec2& rhs) const
{
	return this->x == rhs.x && this->y == rhs.y;
}

bool Vec2::operator!= (const Vec2& rhs) const
{
	return this->x != rhs.x || this->y != rhs.y;
}

Vec2 Vec2::operator + (const Vec2& rhs) const
{
	Vec2 result;
	result.x = this->x + rhs.x;
	result.y = this->y + rhs.y;
	return result;
}

Vec2 Vec2::operator - (const Vec2& rhs) const
{
	Vec2 result;
	result.x = this->x - rhs.x;
	result.y = this->y - rhs.y;
	return result;
}

Vec2 Vec2::operator / (const float val) const
{
	if (val != 0.0f) {
		// Create a new Vec2 to store the result
		Vec2 result;
		// Divide each component of the vector by val
		result.x = this->x / val;
		result.y = this->y / val;
		return result;
	}
	else {
		// Handle division by zero gracefully (you may choose to return a default vector or throw an exception)
		// For simplicity, let's return a default constructed vector with components set to 0.0f
		return Vec2();
	}
}

Vec2 Vec2::operator * (const float val) const
{
	if (val != 0.0f) {
		Vec2 result;
		result.x = this->x * val;
		result.y = this->y * val;
		return result;
	}
	else {
		return Vec2();
	}
}

void Vec2::operator+= (const Vec2& rhs)
{
	this->x += rhs.x;
	this->y += rhs.y;
}

void Vec2::operator-= (const Vec2& rhs)
{
	this->x -= rhs.x;
	this->y -= rhs.y;
}

void Vec2::operator*= (const float val)
{
	this->x* val;
	this->y* val;
}

void Vec2::operator /= (const float val)
{
	// Check if val is not zero to avoid division by zero
	if (val != 0.0f) {
		// Divide each component of the vector by val and update the components
		this->x /= val;
		this->y /= val;
	}
	else {
		// Handle division by zero gracefully (you may choose to do nothing, return a default vector, or throw an exception)
		// For simplicity, let's set the components of the vector to zero
		this->x = 0.0f;
		this->y = 0.0f;
	}
}

float Vec2::dist(const Vec2& rhs) const
{
	float dx = rhs.x - this->x;
	float dy = rhs.y - this->y;

	float dxSquared = dx * dx;
	float dySquared = dy * dy;

	float sumSquared = dxSquared + dySquared;

	return sumSquared;
}

Vec2 Vec2::normalize() const
{
	float dx = this->x;
	float dy = this->y;

	float dxSquared = dx * dx;
	float dySquared = dy * dy;

	float mag = sqrtf(dxSquared + dySquared);

	if (mag != 0) {
		float inverseMag = 1.0f / mag;
		return Vec2(dx * inverseMag, dy * inverseMag);
	}
	else {
		return Vec2(0, 0);
	}

	
}