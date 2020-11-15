#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"


#include <string>

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
	void DrawScore();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);
	void WriteLetter(int x, int y, char letra);
	int WriteWords(int x, int y, std::string frase);

public:
	p2List<PhysBody*> circles;
	bool sensed;

	SDL_Texture* circle;
	SDL_Texture* bg;
	SDL_Texture* assets;
	uint bonus_fx;
	uint triangleBounceFx;
	p2Point<int> ray;
	bool ray_on;

	SDL_Rect scoreRect;
	SDL_Rect kikerInvisble;
	SDL_Rect kikerRect;
	Kicker kiker;
	//Variables to sensors
	PhysBody* sensorBlock;
	bool FlipperKickerup = false;
	bool rampDraw = false;
	bool boolRampSensor = false;
	bool BoolRampSensorBack = false;
	//Sensors
	p2List<PhysBody*> sensorsList;
	PhysBody* kickerPathSensor;
	PhysBody* sensor;
	PhysBody* rampSensor;
	PhysBody* rampSensor2;
	PhysBody* rampSensorBack;
	PhysBody* rampSensorBack2;


	int score;

};
