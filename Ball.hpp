#pragma once
#include "common.hpp"
#include "Physics.hpp"


class Ball : public Physics
{
public:
	void draw() const override;
	void draw(const ColorF& color) const;
	void draw(const Texture& texture) const;
};

