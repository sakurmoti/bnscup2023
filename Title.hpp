#pragma once
#include "common.hpp"

class Title : public App::Scene
{
public:
	Title(const InitData& init);

	void update() override;

	void draw() const override;

private:
	Rect startButton = Rect(Arg::center = Scene::Center().movedBy(0, 100), 200, 60);
	Rect exitButton = Rect(Arg::center = Scene::Center().movedBy(0, 200), 200, 60);
};

