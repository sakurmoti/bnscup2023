#include "Launcher.hpp"
#include "Physics.hpp"

Launcher::Launcher(int number)
{
	this->number = number;
	balls.resize(number);
}

double Vx,Vy,Vz, theta, F;
Optional<Physics> debugBall;
void Launcher::update(double dt)
{
	accumulateTime += dt;
	if (accumulateTime > spawnTime)
	{
		accumulateTime -= spawnTime;
		number--;
		Shoot();
	}

	for (int i = std::max(number, 0); i < balls.size(); i++)
	{
		if (balls.at(i)) balls.at(i).value().update(dt);
	}

	if(debugBall) debugBall.value().update(dt);
}

void Launcher::draw() const
{
	for (int i = std::max(number, 0); i < balls.size(); i++)
	{
		if(balls.at(i)) balls.at(i).value().draw(Palette::Red);
	}

	if (debugBall) {
		Print << U"vel: " << debugBall.value().vel << U"force: " << debugBall.value().force;
		debugBall.value().draw(Palette::Blue);

	}
}

void Launcher::Shoot()
{
	if (number < 0) {
		return;
	}
	else {
		if (!balls.at(number)) return;
		Physics *ball = &balls.at(number).value();
		ball->setMass(1);

		double size = 3.0;
		// Boxが埋まらないように中心の半分ずらす
		Array<std::tuple<Vec3, Vec3, Vec3>> rand = {
			std::make_tuple(Vec3{ -20, -5+(size/2), -20}, Vec3{5, 0, 10}, Vec3{0, 0, 0}),
			std::make_tuple(Vec3{ 20, -5+(size/2), -20}, Vec3{-5, 0, 10}, Vec3{0, 0, 0}),
		};

		std::tie(ball->pos, ball->vel, ball->force) = rand.choice();
		ball->setObj(OrientedBox{ ball->pos, size });
	}
}

void Launcher::DebugShoot() const
{
	Physics *ball = new Physics();
	ball->setMass(1);

	double size = 3.0;
	Print << U"V: " << Vx;
	Print << U"F: " << F;
	SimpleGUI::Slider(U"Vx", Vx, 0.0, 10.0, Vec2(300, 10), 200);
	SimpleGUI::Slider(U"Vy", Vy, 0.0, 10.0, Vec2(300, 50), 200);
	SimpleGUI::Slider(U"Vz", Vz, 0.0, 10.0, Vec2(300, 90), 200);
	SimpleGUI::Slider(U"theta", theta, 0.0, 360_deg, Vec2(300, 130), 200);

	// プレイエリア内に収まるようにする
	// Z = Z' + Vz't + 1/2 Az t^2 のとき、Z = 0 となるtを求め、
	// そのようなtのとき
	// X = X' + Vx't + 1/2 Ax t^2 から、-20 < x < 20 となるAxおよびVx'を求めたい
	// 簡単のため、媒介変数kを用いて、Vx' Cos(k) = Vz' Sin(k) = Vとする
	// ここはあとで作る。計算よろしくお願いします。


	Array<std::tuple<Vec3, Vec3, Vec3>> rand = {
			std::make_tuple(Vec3{ -20, -5 + (size / 2), -20}, Vec3{Vx, Vy, Vz}, Vec3{10, -9.8, 10}),
			//std::make_tuple(Vec3{ 20, -5 + (size / 2), -20}, Vec3{-V*Cos(theta), 0, -V*Sin(theta)}, Vec3{10, -9.8, F * Sin(theta)}),
	};

	std::tie(ball->pos, ball->vel, ball->force) = rand.choice();
	ball->setObj(OrientedBox{ ball->pos, size });

	if(SimpleGUI::Button(U"Add", Vec2{30, 30})) debugBall = *ball;
	delete ball;
}
