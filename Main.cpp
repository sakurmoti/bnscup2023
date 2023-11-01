# include <Siv3D.hpp>

double t = 0.0;

struct Physics
{
	double mass;
	Vec3 pos;
	Vec3 force;

	Physics(double _mass, Vec3 _pos, Vec3 _force)
		: mass(_mass), pos(_pos), force(_force)
	{
	}

	void update()
	{
		// 抵抗が強すぎるので、力を入れないと動かない世界を想定
		Vec3 nex = pos + ((force / mass) * pow(Scene::DeltaTime(), 2));
		pos = nex;

		// 力を減衰させて徐々に減速。速度は保存されないものとする
		force *= 0.90;
	}

	void setForce(Vec3 _force)
	{
		force = _force;
	}

	void revX()
	{
		force.x *= -1;
	}

	void revY()
	{
		force.y *= -1;
	}

	void revZ()
	{
		force.z *= -1;
	}

	void rev()
	{
		force *= -1;
	}

	Vec3 detect(Physics &obj)
	{
		double e = 0.5; //反発係数
		double m1 = this->mass;
		double m2 = obj.mass;
		double M = (m1 * m2) / (m1 + m2);

		Vec3 f1 = this->force;
		Vec3 f2 = obj.force;

		Vec3 v1 = f1 / m1;
		Vec3 v2 = f2 / m2;

		// 衝突面の法線ベクトル objからthisへの向き
		Vec3 n = (this->pos - obj.pos).normalize();

		// 撃力を計算する
		Vec3 F = ( (1 + e) * M * (v1 - v2) * n ) + (M * (f1/m1 - f2/m2) * n ) * (n) * (1/Scene::DeltaTime());

		// 撃力を物体に加える
		this->setForce(this->force + F);
		obj.setForce(obj.force - F);
		
		return F;
	}


};

struct Phy_Test : public Physics
{
	Mesh mesh;

	Phy_Test(Mesh _mesh, double _mass, Vec3 _pos, Vec3 _force)
		: Physics(_mass, _pos, _force), mesh(_mesh)
	{
	}
	

	void draw() const
	{
		mesh.draw(pos, Palette::Red);
	}

	
};

struct Prize : public Physics
{
	Mesh type;
	Vec3 pos;
	double th0; // 初期位相

	Prize(const Mesh& _type, const Vec3& _pos, const Vec3& _force, const double& _th0)
		: Physics(1.0, _pos, _force), type(_type), pos(_pos), th0(_th0)
	{
	}

	void draw() const
	{
		type.draw(pos, Palette::Aquamarine);
	}

	void update(double w_rad)
	{
		// yを固定軸としてxz平面で等速円運動
		double radius = Math::Sqrt(pos.x * pos.x + pos.z * pos.z);
		Vec3 nex = { radius * Math::Cos(w_rad * t + th0), pos.y, radius * Math::Sin(w_rad * t + th0)};
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
			g.update(w_rad);
		}
	}
};

struct Detection
{
	Vec3 pos;
	double radius;
	Sphere sphere;

	void draw() const
	{
		sphere.draw(Palette::Black);
	}

	void update()
	{
		sphere = Sphere(pos, radius);
	}

	void detect(const Machine &machine)
	{
		for (auto& g : machine.goods)
		{
			
		}
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
		Vec3 nex = { radius * Math::Cos(w_rad * t) + center.x, radius * Math::Sin(w_rad * t) + center.y, pos.z };
		pos = nex;
		detection.pos = pos;
		detection.update();
	}
};

void Main()
{
	//Window::Resize(1280, 720);
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const Texture uvChecker{ U"example/texture/uv.png", TextureDesc::MippedSRGB };
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 120_deg, Vec3{ 0, 3, 10 } };

	Array<Prize> goods;
	const Mesh torus{ MeshData::Torus(6.0, 2.0) };
	for (int i = 0; i < 500; i++)
	{
		Mesh obj;
		const Array<String> type = { U"Sphere", U"Box", U"Pyramid" };
		String tp = type.choice();

		// トーラスの一般式 中心までの距離D, 環の半径R, 環の中の角度u, 中心までの角度vとする
		double D = 5.0;
		double R = 1.0;
		double u = Random(Math::Pi);
		double v = Random(Math::TwoPi);
		double x = (D + R * Math::Cos(u)) * Math::Cos(v);
		double z = (D + R * Math::Cos(u)) * Math::Sin(v);
		double y = R * Math::Sin(u);
		Vec3 pos = { x, y, z };

		if (tp == U"Sphere")
		{
			// 中心pos, 半径rの球を生成
			double r = 0.2;
			obj = (Mesh)MeshData::Sphere(r);
		}
		else if (tp == U"Box")
		{
			// 辺の長さlの立方体を生成
			double l = 0.3;
			obj = (Mesh)MeshData::Box(l);
		}
		else if (tp == U"Pyramid")
		{
			// 底辺l, 高さhの四角錐を生成
			double l = 0.3;
			double h = 0.3;
			obj = (Mesh)MeshData::Pyramid(l, h);
		}
		else
		{
			Print << U"Not Found Error in goods select";
		}

		goods.push_back(Prize(obj, pos, Vec3{ 0,0,0 }, v));
	}
	// machine{ center, w_rad, radius, goods }
	Machine machine{ Vec3{ 0, 0, 0 }, (1080_deg * Math::Pi / 180.0), 20.0, goods };

	Mesh hemisphere{ MeshData::Hemisphere(1.0) };
	// capture{ pos, center, w_rad, radius, hemisphere }
	Capture capture{ Vec3{ 0, 0, 0 }, Vec3{ 0, 1.5, 5 }, 10*(360_deg * Math::Pi / 180.0), 1.0 , hemisphere };

	Phy_Test test1{ (Mesh)MeshData::Box(1.0), 1.0, Vec3{ -20, -10, 0 }, Vec3{ 50, 0, 0 } };
	Phy_Test test2{ (Mesh)MeshData::Box(1.0), 1.0, Vec3{ 20, -10, 0 }, Vec3{ -50, 0, 0 } };



	while (System::Update())
	{
		t = Scene::Time();
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		// 3D 描画
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
			Plane{ Vec3{0,-30, 0}, 64 }.draw(uvChecker);
			//torus.draw({ 0, 0, 0 }, Quaternion::RotateY(t * 30_deg), uvChecker);
			{
				const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };
				machine.update();
				machine.draw();

				capture.update();
				capture.draw(uvChecker);
				capture.detection.detect(machine);
			}

			{
				test1.update();
				test1.draw();
				test2.update();
				test2.draw();

				if (Key1.down()) {
					Print << test1.detect(test2);
					
				}

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
