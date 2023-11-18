#pragma once

#include <SFML/Graphics.hpp>
#include "Vec2.h"

class CTransform {
public:
	float	angle		= 0;
	Vec2	pos			= { 0.0, 0.0 };
	Vec2	velocity	= { 0.0, 0.0 };

	CTransform() { }

	CTransform(float angle, const Vec2& pos, const Vec2& velocity)
		: angle(angle), pos(pos), velocity(velocity) {
	}
};

class CShape {
public:
	sf::CircleShape shape;

	CShape(float radius, int points, const sf::Color& fill, const sf::Color& outline, float thickness) : shape(radius, points) {
		shape.setFillColor(fill);
		shape.setOutlineColor(outline);
		shape.setOutlineThickness(thickness);
		shape.setOrigin(radius, radius);
	}
};

class CCollision {
public:
	float radius = 0;

	CCollision() { }

	CCollision(float radius) : radius(radius) { }
};

class CScore {
public:
	int score = 0;

	CScore() { }

	CScore(int score) : score(score) { }
};

class CLifespan {
public:
	int remaining	= 0;
	int total		= 0;

	CLifespan() { }

	CLifespan(int total, int remaining) : total(total), remaining(remaining) { }
};

class CInput {
public:
	bool down	= false;
	bool left	= false;
	bool right	= false;
	bool shoot	= false;
	bool up		= false;
};
