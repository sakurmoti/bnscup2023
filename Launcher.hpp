#pragma once
#include "common.hpp"
#include "Physics.hpp"

class Launcher
{
public:
	Launcher(int number);
	void update(double dt);

	void draw() const;

	void Shoot();

private:
	int number = 20;
	const double spawnTime = 1.2;
	double accumulateTime = 0.0;
	Array<Physics> balls;
};

