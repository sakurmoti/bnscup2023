#include "Title.hpp"

Title::Title(const InitData& init)
	: IScene(init)
{
	Scene::SetBackground(ColorF(0.8, 0.9, 1.0));
}

void Title::update()
{
	if (startButton.mouseOver() || exitButton.mouseOver())
	{
		Cursor::RequestStyle(CursorStyle::Hand);
	}

	if (startButton.leftClicked())
	{
		changeScene(State::Game);
	}

	if (exitButton.leftClicked())
	{
		System::Exit();
	}
}

void Title::draw() const
{
	FontAsset(U"Menu")(U"ここにタイトル").drawAt(Scene::Center(), Palette::White);

	{
		startButton.draw(ColorF(1.0, 0.5, 0.5, 0.5));
		FontAsset(U"Menu")(U"スタート").drawAt(startButton.center(), Palette::Black);
	}

	{
		exitButton.draw(ColorF(1.0, 0.5, 0.5, 0.5));
		FontAsset(U"Menu")(U"終了").drawAt(exitButton.center(), Palette::Black);
	}
}
