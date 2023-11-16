#pragma once
#include "common.hpp"

class Physics
{
public:
	double mass;
	Vec3 vel;
	Vec3 force;
	Vec3 pos;
	OrientedBox obj;

	Physics();

	Physics(double mass, Vec3 vel, Vec3 force, OrientedBox obj);

	void setPos(const Vec3& pos);

	void setVel(const Vec3& vel);

	void setForce(const Vec3& force);

	void setObj(const OrientedBox& obj);

	virtual void draw() const;
	virtual void draw(const Texture& tex) const;
	virtual void draw(const ColorF& color) const;

	virtual void update(double dt);

	void stop();

	void reflect(Physics Wall, double e);

	void collision(Physics& p2, double e);
};
