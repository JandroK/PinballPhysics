#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;
struct b2PrismaticJoint;


struct Kicker {
	PhysBody* anchor;
	PhysBody* body;
	b2PrismaticJoint* joint;
};

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

public:
	p2List<PhysBody*> circles;
	PhysBody* sensor;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* bg;
	SDL_Texture* assets;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;

	SDL_Rect kikerRect;
	Kicker kiker;

};
