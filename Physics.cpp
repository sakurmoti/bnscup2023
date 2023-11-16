#include "Physics.hpp"

Physics::Physics()
	: mass(0), vel(Vec3{ 0, 0, 0 }), force(Vec3{ 0, 0, 0 }), obj(OrientedBox{ Vec3{0,0,0}, Vec3{1,1,1} })
{
}

Physics::Physics(double mass, Vec3 vel, Vec3 force, OrientedBox obj)
	: mass(mass), vel(vel), force(force), obj(obj)
{
	pos = obj.center;
}


void Physics::setPos(const Vec3& _pos)
{
	this->pos = _pos;
	obj.setPos(_pos);
}

void Physics::setVel(const Vec3& _vel)
{
	this->vel = _vel;
}

void Physics::setForce(const Vec3& _force)
{
	this->force = _force;
}

void Physics::setObj(const OrientedBox& _obj)
{
	this->obj = _obj;
}

void Physics::draw() const
{
	obj.draw();
}

void Physics::draw(const Texture& tex) const
{
	obj.draw(tex);
}

void Physics::draw(const ColorF& color) const
{
	obj.draw(color);
}

void Physics::update(double dt)
{
	vel += (force / mass) * dt;
	pos += vel * dt;
	obj.setPos(pos);
}

void Physics::stop()
{
	vel = Vec3{ 0, 0, 0 };
	force = Vec3{ 0, 0, 0 };
}

void Physics::reflect(Physics Wall, double e)
{
	auto getNormal = [](const Vec3& point, const Physics physics) {
		// pointがphysicsのどの面にあるかを判定
		// {0,0,0}を中心としgetCorners()で得られる頂点のうち、インデックスが
		// {0,1,2,3}が -Z	{4,5,6,7}が +Z
		// {0,4,2,6}が -X	{1,5,3,7}が +X
		// {2,3,6,7}が -Y	{0,1,4,5}が +Yの方向
		// 角に当たったときの処理は不定

		Optional<Vec3> ret;
		int indexs[6][4] = { {0,1,2,3}, {4,5,6,7}, {0,4,2,6}, {1,5,3,7}, {2,3,6,7}, {0,1,4,5} };
		Array<Vec3> normals = { {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0} };
		Quaternion rot = physics.obj.orientation;
		for (int i = 0; i < normals.size(); i++) normals[i] = normals[i] * rot;

		std::array<Vec3, 8> corners = physics.obj.getCorners();
		double eps = 1e-5;
		for (int i = 0; i < 6; i++)
		{
			Vec3 A = corners[indexs[i][0]];
			Vec3 B = corners[indexs[i][1]];
			Vec3 C = corners[indexs[i][2]];

			Vec3 AB = B - A;
			Vec3 AC = C - A;
			Vec3 AP = point - A;

			// 行列式が0 == 四面体の体積は0 == 点は平面上にある
			Mat3x3 mat;
			mat._11_12_13 = AB;
			mat._21_22_23 = AC;
			mat._31_32_33 = AP;

			//Print << U"mat: " << mat.determinant();
			if (Abs(mat.determinant()) < eps) {
				ret = normals[i];
				//Print << U"normal: " << normals[i];
			}
		}

		return ret;
	};

	// rayは速度方向として計算
	Vec3 v = this->vel;
	Ray ray{ this->pos, v.normalized() };
	Optional<Float3> intersect = ray.intersectsAt(Wall.obj);
	if (!intersect) return;

	Optional<Vec3> n = getNormal(*intersect, Wall);
	if (!n) return;

	Vec3 N = *n;
	this->vel = v - (1 + e) * Dot(v, N) * N;
	Print << U"reflect1";
}

void Physics::collision(Physics& p2, double e)
{
	Physics &p1 = *this;
	auto getNormal = [](const Vec3& point, const Physics physics) {
		Optional<Vec3> ret;
		int indexs[6][4] = { {0,1,2,3}, {4,5,6,7}, {0,4,2,6}, {1,5,3,7}, {2,3,6,7}, {0,1,4,5} };
		Array<Vec3> normals = { {0,0,-1}, {0,0,1}, {-1,0,0}, {1,0,0}, {0,-1,0}, {0,1,0} };
		Quaternion rot = physics.obj.orientation;
		for (int i = 0; i < normals.size(); i++) normals[i] = normals[i] * rot;

		std::array<Vec3, 8> corners = physics.obj.getCorners();
		double eps = 1e-5;
		for (int i = 0; i < 6; i++)
		{
			Vec3 A = corners[indexs[i][0]];
			Vec3 B = corners[indexs[i][1]];
			Vec3 C = corners[indexs[i][2]];

			Vec3 AB = B - A;
			Vec3 AC = C - A;
			Vec3 AP = point - A;

			// 行列式が0 == 四面体の体積は0 == 点は平面上にある
			Mat3x3 mat;
			mat._11_12_13 = AB;
			mat._21_22_23 = AC;
			mat._31_32_33 = AP;

			//Print << U"mat: " << mat.determinant();
			if (Abs(mat.determinant()) < eps) {
				ret = normals[i];
				//Print << U"normal: " << normals[i];
			}
		}

		return ret;
	};

	Vec3 v1 = p1.vel;
	Vec3 v2 = p2.vel;
	double m1 = p1.mass;
	double m2 = p2.mass;

	Ray ray{ p1.pos, v1.normalized() };
	Optional<Float3> intersect = ray.intersectsAt(p2.obj);
	if (!intersect) {
		//Print << U"no intersect";
		return;
	}

	Optional<Vec3> n = getNormal(*intersect, p2);
	if (!n) {
		//Print << U"no normal";
		return;
	}

	// 外力を一旦無視して運動量保存則から速度を求める
	Vec3 N = *n;
	Vec3 v1_after = (m1 * v1 + m2 * v2 - m2 * e * (v1 - v2)) / (m1 + m2);
	Vec3 v2_after = (m1 * v1 + m2 * v2 + m1 * e * (v1 - v2)) / (m1 + m2);
	p1.vel = v1_after * N;
	p2.vel = v2_after;

}
