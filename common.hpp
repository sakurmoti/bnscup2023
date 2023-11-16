#pragma once

#include <Siv3D.hpp>

enum class State

{
	Title,
	Game,
	Result,
};

struct GameData
{
	Array<int32> highScores = { 0,0,0,0,0 };
};

using App = SceneManager<State, GameData>;


