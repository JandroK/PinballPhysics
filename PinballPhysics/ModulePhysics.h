#pragma once
#include "Module.h"
#include "Globals.h"
#include "Box2D/Box2D/Box2D.h"

#define GRAVITY_X 0.0f
#define GRAVITY_Y -7.0f

#define PIXELS_PER_METER 50.0f // if touched change METER_PER_PIXEL too
#define METER_PER_PIXEL 0.02f // this is 1 / PIXELS_PER_METER !

#define METERS_TO_PIXELS(m) ((int) floor(PIXELS_PER_METER * m))
#define PIXEL_TO_METERS(p)  ((float) METER_PER_PIXEL * p)

// Small class to return to other modules to track position and rotation of physics bodies
class PhysBody
{
public:
	PhysBody() : listener(NULL), body(NULL)
	{}

	void GetPosition(int& x, int &y) const;
	float GetRotation() const;
	bool Contains(int x, int y) const;
	int RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const;

public:
	int width, height;
	b2Body* body;
	Module* listener;
};

// Module --------------------------------------
class ModulePhysics : public Module, public b2ContactListener // TODO
{
public:
	ModulePhysics(Application* app, bool start_enabled = true);
	~ModulePhysics();

	bool Start();
	update_status PreUpdate();
	update_status PostUpdate();
	bool CleanUp();

	PhysBody* CreateCircle(int x, int y, int radius, bool dynamic, float _restitution=0);
	PhysBody* CreateRectangle(int x, int y, int width, int height);
	PhysBody* CreateRectangleSensor(int x, int y, int width, int height);
	PhysBody* CreateChain(int x, int y, int* points, int size, float _restitution=0);
	PhysBody* CreateStaticRectangle(int x, int y, int width, int height, int _restitution=0);
	b2PrismaticJoint* CreatePrismaticJoint(PhysBody* anchor, PhysBody* body, int max_move, int min_move, int motor_speed, int max_force);



	// b2ContactListener ---
	void BeginContact(b2Contact* contact);

private:

	// Create a Left flipper with pivot in position x y
	void CreateFlipperL(int x, int y);

	// Create a Rigth flipper with pivot in position x y
	void CreateFlipperR(int x, int y);
	
	// Create a pair of flippers 
	//x,y: the position of the left pivot
	//separation: distance between left and right pivot (minim 200) 
	void CreatePairFlippers(int x, int y, int separation);

	bool debug;
	b2World* world;
	b2MouseJoint* mouse_joint;
	b2Body* ground;

	PhysBody* bodyClicked;
	bool isClicked = false;
	b2MouseJointDef def;

	b2RevoluteJointDef JoinFlipper;
	p2List<b2Body*> pivotJoins;

public:
	PhysBody* bouncerBall;
	PhysBody* pieceRed;
	PhysBody* corner;
	PhysBody* segmentBig;
	PhysBody* ramp;
	p2List<PhysBody*> flippersL;
	p2List<PhysBody*> flippersR;
	p2List<PhysBody*> circlesToDelete;

};