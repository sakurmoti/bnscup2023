#pragma once
#include "common.hpp"
#include "Hand.hpp"
#include "Launcher.hpp"


class Game : public App::Scene
{
public:
	Game(const InitData& init);

	void update() override;

	void draw() const override;

private:
	const Font font{ 30, Typeface::Bold };
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const Texture uvChecker{ U"example/texture/uv.png", TextureDesc::MippedSRGB };
	const Texture grass{U"example/texture/grass.jpg", TextureDesc::MippedSRGB};
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 120_deg, Vec3{ 0, 1, 5 } };

	Hand hand{ 1.0, {0,0,0}, {0,0,0}, OrientedBox{ {1,1,0.1}, {1,1,1} } };
	Launcher launcher{ 20 };

	int score = 0;
};

