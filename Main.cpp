# include <Siv3D.hpp>

double t = 0.0;

struct Prize
{
	Mesh type;
	Vec3 pos;

	void draw() const
	{
		type.draw(pos, Palette::Aquamarine);
	}

	void update(double radius, double w_rad)
	{
		// yを固定軸としてxz平面で等速円運動
		Vec3 nex = { radius * Math::Cos(w_rad * t), pos.y, radius * Math::Sin(w_rad * t) };
		pos = nex;
	}
};

struct Machine
{
	Vec3 center;
	double w_rad;
	double radius;
	Array<Prize> goods;

	void draw() const
	{
		for (auto& g : goods)
		{
			g.draw();
		}
	}

	void update()
	{
		for (auto& g : goods)
		{
			g.update(radius, w_rad);
		}
	}
};

struct Detection
{
	Vec3 pos;
	double radius;

	void draw() const
	{
		Sphere{ pos, radius }.draw(Palette::Black);
	}

	void detect()
	{
		// Machineのgoodsと衝突判定

	}

};

struct Capture
{
	Vec3 pos; // キャッチャーの位置
	Vec3 center; // 回転の中心
	double w_rad; // 回転速度(rad/s)
	double radius; //回転の半径
	Mesh hemisphere;
	Detection detection;

	Capture(const Vec3& _pos, const Vec3& _center, const double& _w_rad, const double& _radius, const Mesh& _hemisphere)
		: pos(_pos), center(_center), w_rad(_w_rad), radius(_radius), hemisphere(_hemisphere)
	{
		// ここで検出器の位置を初期化?
		detection.pos = pos;
		detection.radius = hemisphere.boundingSphere().r - 0.1;
	}

	void draw(auto& uv) const
	{
		const ScopedRenderStates3D inActiveCull{ RasterizerState::SolidCullNone }; //カリングを両面無効に
		hemisphere.draw(pos, Quaternion::RotateZ(w_rad * t + 180_deg), uv);
		detection.draw();
	}

	void update()
	{
		// zを固定軸としてxy平面で回転運動のシミュレーション
		// とりあえず等速円運動
		Vec3 nex = { radius * Math::Cos(w_rad * t), radius * Math::Sin(w_rad * t), pos.z };
		pos = nex;
		detection.pos = pos;
	}
};

void Main()
{
	//Window::Resize(1280, 720);
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const Texture uvChecker{ U"example/texture/uv.png", TextureDesc::MippedSRGB };
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 120_deg, Vec3{ 16, 18, -32 } };

	Array<Prize> goods;
	for (int i = 0; i < 10; i++)
	{
		Mesh obj;
		const Array<String> type = { U"Sphere", U"Box", U"Pyramid" };
		String tp = type.choice();

		if (tp == U"Sphere")
		{
			// 中心[0,x0] [0,y0] [0,z0], 半径rの球を生成
			int x0 = 10;
			int y0 = 10;
			int z0 = 10;
			int r = 3;
			obj = (Mesh)MeshData::Sphere(RandomVec3(x0, y0, z0), r);
		}
		else if (tp == U"Box")
		{
			// 辺の長さlの立方体を生成
			int l = 3;
			obj = (Mesh)MeshData::Box(l);
		}
		else if (tp == U"Pyramid")
		{
			// 底辺l, 高さhの四角錐を生成
			int l = 3;
			int h = 3;
			obj = (Mesh)MeshData::Pyramid(l, h);
		}
		else
		{
			Print << U"Not Found Error in goods select";
		}

		Vec3 pos = RandomVec3(10);
		goods.push_back(Prize(obj, pos));
	}
	Machine machine{ Vec3{ 0, 0, 0 }, 0.1, 6.0, goods };

	Mesh hemisphere{ MeshData::Hemisphere(10.0) };
	Capture capture{ Vec3{ 0, 10, 0 }, Vec3{ 0, 10, 0 }, (1080_deg * Math::Pi / 180.0), 10.0 , hemisphere };

	while (System::Update())
	{
		t = Scene::Time();
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		// 3D 描画
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
			Plane{ Vec3{0,-30, 0}, 64 }.draw(uvChecker);
			{
				const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
				machine.draw();
				capture.update();
				capture.draw(uvChecker);
			}
		}

		// 3D シーンを 2D シーンに描画
		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);
		}

	}
}
