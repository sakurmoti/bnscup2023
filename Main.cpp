#include "common.hpp"
#include "Title.hpp"
#include "Game.hpp"

void Main()
{
	//Window::Resize(1280, 720);

	FontAsset::Register(U"Menu", 30, Typeface::Bold);

	App manager;
	manager.add<Title>(State::Title);
	manager.add<Game>(State::Game);

	manager.init(State::Game);
	while (System::Update())
	{
		if(not manager.update()) break;
	}
}
