#include "Launcher.hpp"
#include "Physics.hpp"

Launcher::Launcher(int number)
{
	this->number = number;
	balls.resize(number);
}

void Launcher::update(double dt)
{
	accumulateTime += dt;
	if (accumulateTime > spawnTime)
	{
		accumulateTime = 0.0;
		number--;
		Shoot();
	}

	for (int i = std::max(number, 0); i < balls.size(); i++)
	{
		balls[i].update(dt);
	}
}

void Launcher::draw() const
{
	for (int i = std::max(number, 0); i < balls.size(); i++)
	{
		balls[i].draw();
	}
}

void Launcher::Shoot()
{
	if (number < 0) {
		return;
	}
	else {
		Physics *ball = &balls.at(number);
		ball->setPos(Vec3{ 1,1,1 });
		ball->setVel(Vec3{ 0, 0, 0 });
		ball->setForce(Vec3{ 0, 0, 0 });
	}
}

