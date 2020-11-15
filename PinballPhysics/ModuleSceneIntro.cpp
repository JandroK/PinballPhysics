#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModuleFonts.h"
#include "p2Defs.h"

#include <string.h>
#include <sstream>

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

	triangleBounceFx = App->audio->LoadFx("pinball/bounceAxecopia.wav");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");
	bg = App->textures->Load("pinball/pinball_bg.png");
	assets = App->textures->Load("pinball/background.png");
	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+35, SCREEN_WIDTH, 25);

	//Kicker
	kikerInvisble = {281,202,31,47};
	kikerRect = { 281,87,31,96 };
	kiker.anchor = App->physics->CreateStaticRectangle(455, 820, 5, 5);
	kiker.body = App->physics->CreateRectangle(455, 750, 20, 10);
	kiker.body->body->IsBullet();
	kiker.joint = App->physics->CreatePrismaticJoint(kiker.anchor, kiker.body, 1, -80, -20, 50);

	circles.add(App->physics->CreateCircle(450, 730, 12, true));
	circles.getLast()->data->listener = this;

	//Sensors
	sensorsList.add(sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT + 35, SCREEN_WIDTH, 25));
	sensorsList.add(kickerPathSensor = App->physics->CreateRectangleSensor(420, 90, 5, 85));
	sensorsList.add(rampSensor = App->physics->CreateRectangleSensor(192, 370, 5, 5));
	sensorsList.add(rampSensor2 = App->physics->CreateRectangleSensor(270, 74, 5, 5));
	sensorsList.add(rampSensorBack = App->physics->CreateRectangleSensor(192, 405, 5, 5));
	sensorsList.add(rampSensorBack2 = App->physics->CreateRectangleSensor(305, 74, 5, 5));
	App->fonts->Load("pinball/FontY.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,им?!*$%&()+-/:;<=>@__     ", 5, 720, 224);

	scoreRect = {338,87,SCREEN_WIDTH,60};

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
	rect = { 12,87,84,312 };
	App->renderer->Blit(assets, 141, 50, &rect);

	// Circle  -----------------------------------------//////
	c = circles.getFirst();
	rect = { 1723,18,27,27 };
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(assets, x, y, &rect, 1.0f, c->data->GetRotation());
		c = c->next;
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
		if(sensorBlock !=nullptr && sensorBlock->body->IsActive()==true)sensorBlock->body->SetActive(false);
		sensed = false;
	}
	///Block input to kicker
	if (FlipperKickerup)
	{
		sensorBlock = App->physics->CreateStaticRectangle(432, 95, 10, 73);
		FlipperKickerup = false;
	}
	if (boolRampSensor)
	{
		if (App->physics->bouncerBall->body->IsActive())App->physics->bouncerBall->body->SetActive(false);
		if (App->physics->pieceRed->body->IsActive())App->physics->pieceRed->body->SetActive(false);
		if (App->physics->corner->body->IsActive())App->physics->corner->body->SetActive(false);
		if (App->physics->segmentBig->body->IsActive())App->physics->segmentBig->body->SetActive(false);
		if (App->physics->flippersL.getLast()->data->body->IsActive())App->physics->flippersL.getLast()->data->body->SetActive(false);
		if (!App->physics->ramp->body->IsActive())App->physics->ramp->body->SetActive(true);
		boolRampSensor = false;
		rampDraw = true;
	}
	rect = {12,87,84,312};
	if(!rampDraw)
		App->renderer->Blit(assets, 141, 50, &rect);
	if(BoolRampSensorBack)
	{
		if (!App->physics->bouncerBall->body->IsActive())App->physics->bouncerBall->body->SetActive(true);
		if (!App->physics->pieceRed->body->IsActive())App->physics->pieceRed->body->SetActive(true);
		if (!App->physics->corner->body->IsActive())App->physics->corner->body->SetActive(true);
		if (!App->physics->segmentBig->body->IsActive())App->physics->segmentBig->body->SetActive(true);
		if (!App->physics->flippersL.getLast()->data->body->IsActive())App->physics->flippersL.getLast()->data->body->SetActive(true);
		if (App->physics->ramp->body->IsActive())App->physics->ramp->body->SetActive(false);
		BoolRampSensorBack = false;
		rampDraw = false;
	}
	DrawScore();

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::DrawScore()
{
	std::stringstream strs;
	strs << score;
	std::string temp_str = strs.str();
	char* char_type = (char*)temp_str.c_str();
	App->renderer->Blit(assets, 0, SCREEN_HEIGHT - 60, &scoreRect);
	App->fonts->BlitText( 35,885, 0, char_type);
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;
	App->audio->PlayMusic("pinball/bounceAxe.wav");

	if (bodyA == sensor  ||	bodyB == sensor ) {

		App->audio->PlayFx(bonus_fx);
		App->audio->PlayMusic("pinball/bounceAxe.wav");

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

	if(bodyA != nullptr&& bodyB != nullptr)
	if (bodyA->type== TypePhysbody::BOUNCER || bodyB->type== TypePhysbody::BOUNCER)
	{
		App->audio->PlayFx(bonus_fx);
		score += 10;
	}
	if(bodyA != nullptr&& bodyB != nullptr)
	if (bodyA->type== TypePhysbody::BOUNCER_BALL || bodyB->type== TypePhysbody::BOUNCER_BALL)
	{
		App->audio->PlayFx(bonus_fx);
		score += 10;
	}

}

