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

	//hand.update(dt, camera);
	launcher.update(dt);
	

}

void Game::draw() const
{
	// 3D 描画
	{
		const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
		{
			const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
			Plane{ {0,-5,0}, 64 }.draw(uvChecker);
			//hand.draw(Palette::Red);
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
}
