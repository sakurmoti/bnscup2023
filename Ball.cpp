#include "stdafx.h"
#include "Ball.hpp"

void Ball::draw() const
{
	Sphere{obj.center, 3}.draw();
}

void Ball::draw(const ColorF& color) const
{
	Sphere{obj.center, 3}.draw(color);
}

void Ball::draw(const Texture& texture) const
{
	Sphere{obj.center, 3}.draw(texture);
}
