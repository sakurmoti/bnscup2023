#pragma once
#include "common.hpp"
#include "Physics.hpp"

class Hand : public Physics
{
public:
	Hand(double mass, Vec3 vel, Vec3 force, OrientedBox obj);

	void update(double dt, DebugCamera3D camera);

	void draw() const override;

	void draw(const ColorF& color) const override;
};

