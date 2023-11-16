#include "Hand.hpp"
#include "Physics.hpp"

Hand::Hand(double mass, Vec3 vel, Vec3 force, OrientedBox obj)
	: Physics(mass, vel, force, obj)
{
}

void Hand::update(double dt, DebugCamera3D camera)
{
	Vec3 camCenter = camera.getFocusPosition();
	Box camBox = Box{ camCenter, 0.5, 0.5, 0.5 };

	Ray ray = camera.screenToRay(Cursor::Pos());
	Box playArea = Box::FromPoints({-20,-20,0}, {20,20,1});

	Optional<Float3> intersection = ray.intersectsAt(playArea);
	if (intersection)
	{
		obj.setPos(intersection.value());
	}
	else
	{
		obj.setPos(playArea.center);
	}
}

void Hand::draw() const
{
	obj.draw();
}

void Hand::draw(const ColorF& color) const
{
	obj.draw(color);
}
