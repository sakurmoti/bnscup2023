#pragma once
#include "common.hpp"
#include "Physics.hpp"

class Launcher
{
public:
	Array<Optional<Physics>> balls;

	Launcher(int number);
	void update(double dt);

	void draw() const;

	void Shoot();

	void DebugShoot() const;

private:
	int number = 20;
	const double spawnTime = 1.2;
	double accumulateTime = 0.0;
};

