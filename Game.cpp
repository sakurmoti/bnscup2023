#include "Game.hpp"
#include "Physics.hpp"


Game::Game(const InitData& init)
	: IScene(init)
{

}

void Game::update()
{
	ClearPrint();
	Print << U"camera pos : " << camera.getEyePosition();
	double dt = Scene::DeltaTime();
	camera.update(2.0);
	Graphics3D::SetCameraTransform(camera);

	hand.update(dt, camera);
	launcher.update(dt);

	for (auto& ball : launcher.balls)
	{
		if (!ball) continue;
		if (hand.obj.intersects(ball.value().obj))
		{
			score++;
			ball.value().reflect(hand, 0.2);
		}
	}
}

void Game::draw() const
{
	// 3D 描画
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		{
			const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
			Plane{ {0,-5,0}, 64 }.draw(uvChecker);
			hand.draw(ColorF(1.0, 0.3, 0.3, 0.5));
			launcher.draw();
			//Box playArea = Box::FromPoints({ -20,-20,0 }, { 20,20,1 }).draw();
		}
	}

	// 3D シーンを 2D シーンに描画
	{
		Graphics3D::Flush();
		renderTexture.resolve();
		Shader::LinearToScreen(renderTexture);
	}

	// 2D 描画はFlushした後に
	{

		font(score).drawAt(Scene::Center().movedBy(0, 200));
	}

	//launcher.DebugShoot();
}
