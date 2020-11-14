#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "p2Defs.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	ray_on = false;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	App->renderer->camera.x = App->renderer->camera.y = 0;

	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	bg = App->textures->Load("pinball/pinball_bg.png");
	assets = App->textures->Load("pinball/background.png");
	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+35, SCREEN_WIDTH, 25);

	//Kicker
	kikerInvisble = {281,202,31,47};
	kikerRect = { 281,87,31,96 };
	kiker.anchor = App->physics->CreateStaticRectangle(455, 820, 5, 5);
	kiker.body = App->physics->CreateRectangle(455, 750, 20, 10);
	kiker.joint = App->physics->CreatePrismaticJoint(kiker.anchor, kiker.body, 1, -80, -20, 50);
	sensorBlock = App->physics->CreateStaticRectangle(435, 97, 12, 69);
	sensorBlock->body->SetActive(false);
	circles.add(App->physics->CreateCircle(450, 730, 12, true));
	circles.getLast()->data->listener = this;

	//Sensors
	sensorsList.add(sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT + 35, SCREEN_WIDTH, 25));
	sensorsList.add(kickerPathSensor = App->physics->CreateRectangleSensor(420, 90, 5, 85));
	sensorsList.add(rampSensor = App->physics->CreateRectangleSensor(192, 370, 5, 5));
	sensorsList.add(rampSensor2 = App->physics->CreateRectangleSensor(270, 74, 5, 5));
	sensorsList.add(rampSensorBack = App->physics->CreateRectangleSensor(192, 405, 5, 5));
	sensorsList.add(rampSensorBack2 = App->physics->CreateRectangleSensor(305, 74, 5, 5));

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");
	Mix_HaltMusic();
	App->textures->Unload(bg);
	App->textures->Unload(assets);
	App->textures->Unload(circle);
	return true;
	LOG("Unloading map");

	// Remove circles
	p2List_item<PhysBody*>* item;
	item = circles.getFirst();

	int counter = 1;
	while (item != NULL)
	{
		RELEASE(item->data);
		item = item->next;

	}
	circles.clear();
	LOG("Unloading circles");

	p2List_item<PhysBody*>* item3;
	item3 = sensorsList.getFirst();
	while (item3 != NULL)
	{
		RELEASE(item3->data);
		item3 = item3->next;

	}
	sensorsList.clear();
	LOG("Unloading sensors");

}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	App->renderer->Blit(bg, 0, 0);
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {

		kiker.joint->SetMotorSpeed(3);
		kiker.joint->SetMaxMotorForce(3);

	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) {
		kiker.joint->SetMotorSpeed(-20);
		kiker.joint->SetMaxMotorForce(900);

		//App->audio->PlayFx();
	}
		
	if(App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
	{
		ray_on = !ray_on;
		ray.x = App->input->GetMouseX();
		ray.y = App->input->GetMouseY();
	}

	if(App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		circles.add(App->physics->CreateCircle(App->input->GetMouseX(), App->input->GetMouseY(), 12, true));
		circles.getLast()->data->listener = this;
	}
	
	// Flipper Torque
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
	{
		p2List_item<PhysBody*>* data = App->physics->flippersL.getFirst();
		while (data != NULL)
		{
			data->data->body->ApplyTorque(-400, true);
			data = data->next;
		}
	}

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
	{
		p2List_item<PhysBody*>* data = App->physics->flippersR.getFirst();
		while (data != NULL)
		{
			data->data->body->ApplyTorque(400, true);
			data = data->next;
		}
	}

	// Prepare for raycast ------------------------------------------------------
	
	iPoint mouse;
	mouse.x = App->input->GetMouseX();
	mouse.y = App->input->GetMouseY();
	int ray_hit = ray.DistanceTo(mouse);

	fVector normal(0.0f, 0.0f);

	// All draw functions ------------------------------------------------------
	iPoint launch_pos;
	kiker.body->GetPosition(launch_pos.x, launch_pos.y);
	launch_pos.x -= kiker.body->width;
	launch_pos.y -= kiker.body->height;
	App->renderer->Blit(assets, launch_pos.x+3, launch_pos.y+7, &kikerRect);
	App->renderer->Blit(assets, 437, 832, &kikerInvisble);
	
	/// ------ Pala------------
	p2List_item<PhysBody*>* c = App->physics->flippersL.getFirst();
	SDL_Rect rect = { 1899,12,73,39 };
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(assets, x - 39, y - 25, &rect, 1.0f, c->data->GetRotation() - (RADTODEG * 0.50));
		c = c->next;
	}
	c = App->physics->flippersR.getFirst();
	rect = { 2067,12,73,39 };
	while (c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(assets, x - 39, y - 22, &rect, 1.0f, c->data->GetRotation() + (RADTODEG * 0.50));
		c = c->next;
	}
	// Rampa -------------------------------------------//////
	rect = {12,87,84,312};
	App->renderer->Blit(assets, 141, 50, &rect);

	// Circle  -----------------------------------------//////
	c = circles.getFirst();
	rect = {1723,18,27,27};
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(assets, x, y, &rect, 1.0f, c->data->GetRotation());
		c = c->next;
	}
	//Bouncer bols--------------------------------------/////
	if (bouncerBallDraw)
	{
		rect = {1764,12,56,53};
		App->renderer->Blit(assets, 110, 162, &rect);
		App->renderer->Blit(assets, 90, 227, &rect);
		App->renderer->Blit(assets, 157, 210, &rect);

	}

	// ray -----------------
	if(ray_on == true)
	{
		fVector destination(mouse.x-ray.x, mouse.y-ray.y);
		destination.Normalize();
		destination *= ray_hit;

		App->renderer->DrawLine(ray.x, ray.y, ray.x + destination.x, ray.y + destination.y, 255, 255, 255);

		if(normal.x != 0.0f)
			App->renderer->DrawLine(ray.x + destination.x, ray.y + destination.y, ray.x + destination.x + normal.x * 25.0f, ray.y + destination.y + normal.y * 25.0f, 100, 255, 100);
	}
	//Sensors-------------

	/// Delete Ball / Spawn next Ball
	if (sensed)
	{
		circles.getLast()->data->body->GetWorld()->DestroyBody(circles.getLast()->data->body);
		circles.del(circles.getLast());
		circles.add(App->physics->CreateCircle(450, 720, 12, true));
		circles.getLast()->data->listener = this;
		sensorBlock->body->SetActive(false); ///Block input to kicker
		sensed = false;
		FlipperKickerup = false;
	}
	if(FlipperKickerup)//Active collison of block input in the kicker
		sensorBlock->body->SetActive(true);
	//Active Desactive Collisions
	if (boolRampSensor)
	{
		App->physics->bouncerBall3->body->SetActive(false);
		App->physics->pieceRed->body->SetActive(false);
		App->physics->corner->body->SetActive(false);
		App->physics->segmentBig->body->SetActive(false);
		if (App->physics->flippersL.getLast()->data->body->IsActive())App->physics->flippersL.getLast()->data->body->SetActive(false);
		if (!App->physics->ramp->body->IsActive())App->physics->ramp->body->SetActive(true);
		boolRampSensor = false;
		bouncerBallDraw = false;
	}
	if (BoolRampSensorBack)
	{
		App->physics->bouncerBall3->body->SetActive(true);
		App->physics->pieceRed->body->SetActive(true);
		App->physics->corner->body->SetActive(true);
		App->physics->segmentBig->body->SetActive(true);
		if (!App->physics->flippersL.getLast()->data->body->IsActive())App->physics->flippersL.getLast()->data->body->SetActive(true);
		if (App->physics->ramp->body->IsActive())App->physics->ramp->body->SetActive(false);
		BoolRampSensorBack = false;
		rampDraw = true;
		bouncerBallDraw = true;
	}
	//Hit of Bouncer Balls
	rect = { 1832,12,56,53 };
	if (bouncerBallHit1)
	{
		App->renderer->Blit(assets, 110, 162, &rect);
		bouncerBallHit1 = false;
	}
	if (bouncerBallHit2)
	{
		App->renderer->Blit(assets, 90, 227, &rect);
		bouncerBallHit2 = false;
	}
	if (bouncerBallHit3)
	{
		App->renderer->Blit(assets, 157, 210, &rect);
		bouncerBallHit3 = false;
	}
	//Draw Ramp
	if (rampDraw)
	{
		rect = { 12,87,84,312 };
		App->renderer->Blit(assets, 141, 50, &rect);
	}
	if (!bouncerBallDraw)rampDraw = false;//depende de bouncerBallDraw porque queremos primero dejar de pintar las bolas que la rampa
	
	
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;
	
	if (bodyA == sensor  ||	bodyB == sensor ) {

		App->audio->PlayFx(bonus_fx);
		sensed = true;
	}

	if (bodyA == kickerPathSensor && bodyB == circles.getLast()->data ||
		bodyB == kickerPathSensor && bodyA == circles.getLast()->data) {

		if (FlipperKickerup != true) {
			FlipperKickerup = true;
		}
	}
	if (bodyA == rampSensor && bodyB == circles.getLast()->data ||
		bodyB == rampSensor && bodyA == circles.getLast()->data ||
		bodyA == rampSensor2 && bodyB == circles.getLast()->data ||
		bodyB == rampSensor2 && bodyA == circles.getLast()->data) {

		if (boolRampSensor != true) {
			boolRampSensor = true;
		}
	}
	if (bodyA == rampSensorBack && bodyB == circles.getLast()->data ||
		bodyB == rampSensorBack && bodyA == circles.getLast()->data ||
		bodyA == rampSensorBack2 && bodyB == circles.getLast()->data ||
		bodyB == rampSensorBack2 && bodyA == circles.getLast()->data) {

		if (BoolRampSensorBack != true) {
			BoolRampSensorBack = true;
		}
	}
	if (bodyA == App->physics->bouncerBall1 && bodyB == circles.getLast()->data ||
		bodyB == App->physics->bouncerBall1 && bodyA == circles.getLast()->data) {
		if (bouncerBallHit1 != true) {
			bouncerBallHit1 = true;
		}
		//+Score
	}
	if (bodyA == App->physics->bouncerBall2 && bodyB == circles.getLast()->data ||
		bodyB == App->physics->bouncerBall2 && bodyA == circles.getLast()->data) {
		if (bouncerBallHit2 != true) {
			bouncerBallHit2 = true;
		}
		//+Score
	}
	if (bodyA == App->physics->bouncerBall3 && bodyB == circles.getLast()->data ||
		bodyB == App->physics->bouncerBall3 && bodyA == circles.getLast()->data) {
		if (bouncerBallHit3 != true) {
			bouncerBallHit3 = true;
		}
		//+Score
	}
	
	/*
	if(bodyA)
	{
		bodyA->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}

	if(bodyB)
	{
		bodyB->GetPosition(x, y);
		App->renderer->DrawCircle(x, y, 50, 100, 100, 100);
	}*/
}
