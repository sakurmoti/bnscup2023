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
		balls.at(i).update(dt);
	}
}

void Launcher::draw() const
{
	for (int i = std::max(number, 0); i < balls.size(); i++)
	{
		Print << U"i: " << i;
		Print << U"ball pos: " << balls.at(i).pos;
		balls.at(i).draw(Palette::Black);
	}
}

void Launcher::Shoot()
{
	if (number < 0) {
		return;
	}
	else {
		Print << U"number : " << number << U"assined";
		Physics *ball = &balls.at(number);
		ball->setMass(1);
		ball->setObj(OrientedBox{ Vec3{-10,10,-10}, 3 });
		ball->setPos(Vec3{ -10, 10, -10 });
		ball->setVel(Vec3{ 0, 0, 0 });
		ball->setForce(Vec3{ 0, 0, 0 });
	}
}

