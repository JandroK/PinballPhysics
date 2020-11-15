#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"
#include "Timer.h"
#include "Animation.h"

class PhysBody;
struct b2PrismaticJoint;
struct AnimationIntro
{
	iPoint position;
	Animation* currentAnimation;
	SDL_Texture* texture;
};

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
	bool sensed;

	AnimationIntro neon;
	Animation idleAnim;

	SDL_Texture* circle;
	SDL_Texture* bg;
	SDL_Texture* assets;
	uint bonus_fx;
	p2Point<int> ray;
	bool ray_on;

	SDL_Rect kikerInvisble;
	SDL_Rect kikerRect;
	Kicker kiker;

	//Timer
	Timer* timerBouncerBallHit1= new Timer(600);
	Timer* timerBouncerBallHit2= new Timer(600);
	Timer* timerBouncerBallHit3= new Timer(600);

	//Variables to sensors
	PhysBody* sensorBlock;
	bool FlipperKickerup = false;
	bool rampDraw = true;
	bool rampLeftDraw = true;
	bool rampRightDraw = true;
	bool boolRampSensor = false;
	bool boolRampSensorBack = false;
	bool boolRampLeftSensor = false;
	bool boolRampLeftSensorBack = false;
	bool boolRampRightSensor = false;
	bool boolRampRightSensorBack = false;
	bool bouncerBallDraw = true;
	bool bouncerBallHit1 = false;
	bool bouncerBallHit2 = false;
	bool bouncerBallHit3 = false;

	//Sensors
	p2List<PhysBody*> sensorsList;
	PhysBody* kickerPathSensor;
	PhysBody* sensor;
	PhysBody* rampSensor;
	PhysBody* rampSensor2;
	PhysBody* rampSensorBack;
	PhysBody* rampSensorBack2;
	PhysBody* rampLeftSensor;
	PhysBody* rampLeftSensor2;
	PhysBody* rampLeftSensorBack;
	PhysBody* rampLeftSensorBack2;
	PhysBody* rampRightSensor;
	PhysBody* rampRightSensor2;
	PhysBody* rampRightSensorBack;
	PhysBody* rampRightSensorBack2;

};
