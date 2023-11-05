# include <Siv3D.hpp>

struct Physics
{
	double mass;
	Vec3 pos;
	Vec3 vel;
	Vec3 force;
	OrientedBox obj;

	Physics() : mass(0), vel({ 0,0,0 }), force({ 0,0,0 }), obj(OrientedBox{ {0,0,0},{1,1,1} })
	{
		pos = obj.center;
	}

	/// @param mass 質量
	/// @param vel 速度
	/// @param force 力
	/// @param obj OrientedBox: {center, size}
	Physics(double mass, Vec3 vel, Vec3 force, OrientedBox obj)
		: mass(mass), vel(vel), force(force), obj(obj)
	{
		pos = obj.center;
	}

	virtual void draw() const
	{
		//Print << U"Physics: virtual draw";
		obj.draw();
	}

	virtual void draw(const Texture& tex) const
	{
		obj.draw(tex);
	}

	virtual void update()
	{
		double dt = Scene::DeltaTime();
		vel += (force / mass) * dt;
		pos += vel * dt;
		obj.setPos(pos);
	}

	void stop()
	{
		vel = Vec3{ 0, 0, 0 };
		force = Vec3{ 0, 0, 0 };
	}

	virtual void reflectX(Physics& wall, double e)
	{
		Vec3 X = { 1,0,0 };

		Quaternion q = wall.obj.orientation;
		Vec3 inDir = vel;

		// X,Y,Zに四元数で回転したものが各面の法線の候補
		// 今回、制約として横面にのみ衝突することを考える
		Vec3 Normal = (q * X).normalized();

		Vec3 Res = inDir + (2 * Normal.dot(-inDir)) * Normal;

		vel = Res * e;
	}

	virtual void reflectY(Physics& wall, double e)
	{
		Vec3 Y = { 0,1,0 };
		
		Quaternion q = wall.obj.orientation;
		Vec3 inDir = vel;

		// 制約として上面にのみ衝突することを考える
		Vec3 Normal = (q * Y).normalized();

		Vec3 Res = inDir + (2 * Normal.dot(-inDir)) * Normal;

		vel = Res * e;
	}

	virtual void reflectZ(Physics& wall, double e)
	{
		Vec3 Z = { 0,0,1 };

		Quaternion q = wall.obj.orientation;
		Vec3 inDir = vel;

		// X,Y,Zに四元数で回転したものが各面の法線の候補
		// 今回、制約として上面にのみ衝突することを考える
		Vec3 Normal = (q * Z).normalized();

		Vec3 Res = inDir + (2 * Normal.dot(-inDir)) * Normal;

		vel = Res * e;
	}

};

struct Prize : public Physics
{
	Vec3 center;
	double w_rad;
	double radius;
	double th0;
	bool isHit;

	/// @param mass 
	/// @param vel 
	/// @param force 
	/// @param obj 
	/// @param center 
	/// @param w_rad 
	Prize(double mass, Vec3 vel, Vec3 force, OrientedBox obj, Vec3 center, double w_rad, double th0)
		: Physics(mass, vel, force, obj), center(center), w_rad(w_rad), isHit(false), th0(th0)
	{
		radius = (pos - center).length();
	}

	void draw() const override
	{
		obj.draw(Palette::Aquamarine);
	}

	void update() override
	{
		if (!isHit)
		{
			// Prizeはxz平面で円運動をする
			double theta = w_rad * Scene::Time() + th0;
			Vec3 dir = (pos - center).normalized(); // 中心への方向ベクトル

			vel = dir * radius * w_rad;
			force = dir * mass * radius * w_rad * w_rad;

			pos = center + Vec3{ radius* Cos(theta), 0, radius* Sin(theta) };

			obj.setPos(pos);
		}
		else
		{
			// Prizeは衝突した後、重力に従って落下する
			vel += force / mass * Scene::DeltaTime();
			pos += vel * Scene::DeltaTime();
			obj.setPos(pos);
		}

		if (pos.length() > 10)
		{
			isHit = false;
		}
	}

	void reflectY(Physics& wall, double e) override
	{
		Vec3 Y = { 0,-1,0 };

		Quaternion q = wall.obj.orientation;
		Vec3 inDir = vel;

		Vec3 Normal = (q * Y).normalized();
		Vec3 Res = inDir + (2 * Normal.dot(-inDir)) * Normal;

		vel = Math::Abs(Res) * e;
	}
};

struct Pusher : Physics
{
	double w_rad;

	Pusher() : Physics(), w_rad(0.0) {}

	/// @param mass 
	/// @param vel 
	/// @param force 
	/// @param obj 
	Pusher(double mass, Vec3 vel, Vec3 force, OrientedBox obj)
		: Physics(mass, vel, force, obj)
	{
		w_rad = Math::ToRadians(90);
		// T = 2pi / w (s)
	}

	void update() override
	{
		// xz平面で前後に動く
		double dt = Scene::DeltaTime();
		double theta = w_rad * Scene::Time();

		pos += Vec3{ 0, 0, 1 } * Sin(theta) * dt;
		obj.setPos(pos);
	}

	void draw() const override
	{
		obj.draw(Palette::Red);
	}

	void push(Physics &p)
	{
		// 衝突したPrizeを前方に押す
		p.vel += Vec3{ 0, 0, 1 } * 10;
		p.force += Vec3{ 0, 0, 1 } * 10;

	}
};

struct Arm : Physics
{
	Arm() : Physics() {}

	Arm(double mass, Vec3 vel, Vec3 force, OrientedBox obj)
		: Physics(mass, vel, force, obj)
	{
	}

	void update() override
	{
		if (KeySpace.pressed() && obj.orientation.getZ() < Quaternion::RotateZ(90_deg).getZ())
		{
			obj.orientation *= Quaternion::RotateZ(0.3);
		}
		else if (obj.orientation.getZ() > Quaternion::RotateZ(-70_deg).getZ())
		{
				obj.orientation *= Quaternion::RotateZ(-0.1);
		}
	}

	void draw()
	{
		obj.draw(Palette::Green);
	}
};

struct Machine
{
	Physics base;
	Pusher pusher;
	Arm arm;

	Machine()
	{
		base = Physics{ 1.0, {0,0,0}, {0,0,0}, OrientedBox{{0,0,2}, {4, 0.5, 4}} };
		pusher = Pusher{ 1.0, {0,0,0}, {0,0,0}, OrientedBox{{0,0.5,0}, {4, 0.4, 2}} };
		arm = Arm{ 1.0, {0,0,0}, {0,0,0}, OrientedBox{{-8,0,1}, {10, 0.1, 1.0}, Quaternion::RotateZ(-70_deg)} };
	}

	void draw()
	{
		base.draw();
		pusher.draw();
		arm.draw();
	}

	void update()
	{
		base.update();
		pusher.update();
		arm.update();
	}

};

void Main()
{
	//Window::Resize(1280, 720);
	const ColorF backgroundColor = ColorF{ 0.4, 0.6, 0.8 }.removeSRGBCurve();
	const Texture uvChecker{ U"example/texture/uv.png", TextureDesc::MippedSRGB };
	const MSRenderTexture renderTexture{ Scene::Size(), TextureFormat::R8G8B8A8_Unorm_SRGB, HasDepth::Yes };
	DebugCamera3D camera{ renderTexture.size(), 120_deg, Vec3{ 0, 3, 5 } };

	Array<Prize> prizes;
	for (int i : step(100))
	{
		double D = 5.0;
		double R = 0.3;
		double u = Random(Math::TwoPi);
		double v = Random(Math::TwoPi);
		double x = (D + R * Math::Cos(u)) * Math::Cos(v);
		double z = (D + R * Math::Cos(u)) * Math::Sin(v);
		double y = R * Math::Sin(u);
		Prize p{ 1.0, Vec3{ 0, 0, 0 }, Vec3{ 0, 0, 0 }, OrientedBox{ Vec3{x,y,z}, Vec3{0.3, 0.3, 0.3}},  Vec3{0,1,0}, Math::ToRadians(90), v};
		prizes.push_back(p);
	}

	Machine machine;
	Box capture = Box::FromPoints({ -2, -2, 4 }, { 2, 0, 6});

	int point = 0;
	while (System::Update())
	{
		camera.update(2.0);
		Graphics3D::SetCameraTransform(camera);

		// 3D 描画
		{
			const ScopedRenderTarget3D target{ renderTexture.clear(backgroundColor) };
			{
				const ScopedRenderStates3D blend{ BlendState::OpaqueAlphaToCoverage };

				machine.update();
				machine.draw();

				capture.draw(Palette::Black);
				for (auto& p : prizes)
				{
					p.update();
					p.draw();

					if (p.obj.intersects(machine.arm.obj))
					{
						p.isHit = true;
						p.force = Vec3{ 0, -9.8, 0 };
						p.reflectY(machine.arm, 0.9);
					}

					if (p.obj.intersects(machine.pusher.obj))
					{
						machine.pusher.push(p);
					}

					if (p.obj.intersects(machine.base.obj))
					{
						p.force = Vec3{ 0, 0, 0 };
						p.reflectY(machine.base, 0.1);
					}

					if (p.obj.intersects(capture))
					{
						point++;
					}

					for (auto& q : prizes)
					{
						if (p.obj.intersects(q.obj) && Abs(p.vel.y) < 0.1 && Abs(q.vel.y) < 0.1)
						{
							p.reflectZ(q, 0.9);
						}
					}

				}
			}
		}

		// 3D シーンを 2D シーンに描画
		{
			Graphics3D::Flush();
			renderTexture.resolve();
			Shader::LinearToScreen(renderTexture);
		}

		// 2D 描画
		{
			FontAsset(U"Score")(U"Score: ", point).draw(10, 10, Palette::White);
		}

	}
}
