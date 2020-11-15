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
	gameOver = App->textures->Load("pinball/GameOver.png");
	neon.texture = App->textures->Load("pinball/Neon.png");
	neon.position = { 0 , 0 };
	idleAnim.PushBack({ 0,0,465,711 }) ;
	idleAnim.PushBack({ 465,0,465,711 });
	idleAnim.loop = true;
	idleAnim.speed = 0.1f;
	neon.currentAnimation = &idleAnim;


	//Kicker
	kikerInvisble = {281,202,31,47};
	kikerRect = { 281,87,31,96 };
	kiker.anchor = App->physics->CreateStaticRectangle(455, 820, 5, 5);
	kiker.body = App->physics->CreateRectangle(455, 750, 20, 10);
	kiker.body->body->IsBullet();
	kiker.joint = App->physics->CreatePrismaticJoint(kiker.anchor, kiker.body, 1, -80, -20, 50);
	sensorBlock = App->physics->CreateStaticRectangle(435, 97, 12, 69);
	sensorBlock->body->SetActive(false);
	circles.add(App->physics->CreateCircle(450, 730, 12, true));
	circles.getLast()->data->listener = this;

	//Sensors upstairs
	sensorsList.add(sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT + 35, SCREEN_WIDTH, 25));
	sensorsList.add(kickerPathSensor = App->physics->CreateRectangleSensor(420, 90, 5, 85));
	sensorsList.add(rampSensor = App->physics->CreateRectangleSensor(192, 370, 5, 5));
	sensorsList.add(rampSensor2 = App->physics->CreateRectangleSensor(270, 74, 5, 5));
	sensorsList.add(rampSensorBack = App->physics->CreateRectangleSensor(192, 405, 5, 5));
	sensorsList.add(rampSensorBack2 = App->physics->CreateRectangleSensor(305, 74, 5, 5));
	App->fonts->Load("pinball/FontY.png", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789.,им?!*$%&()+-/:;<=>@__     ", 5, 720, 224);

	scoreRect = {338,87,SCREEN_WIDTH,60};
	//Sensors downstairs Left
	sensorsList.add(rampLeftSensor = App->physics->CreateRectangleSensor(67, 457, 5, 5));
	sensorsList.add(rampLeftSensor2 = App->physics->CreateRectangleSensor(70, 630, 5, 5));
	sensorsList.add(rampLeftSensorBack = App->physics->CreateRectangleSensor(72, 496, 5, 5));
	sensorsList.add(rampLeftSensorBack2 = App->physics->CreateRectangleSensor(70, 665, 5, 5));
	//Sensors downstairs Right
	sensorsList.add(rampRightSensor = App->physics->CreateRectangleSensor(380, 427, 5, 5));
	sensorsList.add(rampRightSensor2 = App->physics->CreateRectangleSensor(370, 647, 5, 5));
	sensorsList.add(rampRightSensorBack = App->physics->CreateRectangleSensor(376, 465, 5, 5));
	sensorsList.add(rampRightSensorBack2 = App->physics->CreateRectangleSensor(368, 678, 5, 5));

	App->physics->CreateStaticRectangle(345, 141, 10, 20);
	App->physics->CreateStaticRectangle(387, 141, 10, 20);
	App->physics->CreateStaticRectangle(87, 736, 10, 30);

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
	App->textures->Unload(neon.texture);
	App->textures->Unload(gameOver);
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
	if (lives != 0)
	{
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) {

			kiker.joint->SetMotorSpeed(3);
			kiker.joint->SetMaxMotorForce(3);

		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_UP) {
			kiker.joint->SetMotorSpeed(-20);
			kiker.joint->SetMaxMotorForce(900);

			//App->audio->PlayFx();
		}

		if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN)
		{
			ray_on = !ray_on;
			ray.x = App->input->GetMouseX();
			ray.y = App->input->GetMouseY();
		}

		if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
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
				data->data->body->ApplyTorque(-500, true);
				data = data->next;
			}
		}

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			p2List_item<PhysBody*>* data = App->physics->flippersR.getFirst();
			while (data != NULL)
			{
				data->data->body->ApplyTorque(500, true);
				data = data->next;
			}
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

		if (sensorBlock != nullptr)sensorBlock->body->SetActive(false); ///Block input to kicker
		sensed = false;
		FlipperKickerup = false;
	}
	if(FlipperKickerup)//Active collison of block input in the kicker
		sensorBlock->body->SetActive(true);
	
	//Active Desactive Collisions----------------------------------

	//Sensors upstairs
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
	if (boolRampSensorBack)
	{
		App->physics->bouncerBall3->body->SetActive(true);
		App->physics->pieceRed->body->SetActive(true);
		App->physics->corner->body->SetActive(true);
		App->physics->segmentBig->body->SetActive(true);
		if (!App->physics->flippersL.getLast()->data->body->IsActive())App->physics->flippersL.getLast()->data->body->SetActive(true);
		if (App->physics->ramp->body->IsActive())App->physics->ramp->body->SetActive(false);
		boolRampSensorBack = false;
		rampDraw = true;
		bouncerBallDraw = true;
	}
	//Sensors downstairs
	//Sensors Left
	if (boolRampLeftSensor)
	{
		App->physics->wayLeftLine1->body->SetActive(false);
		App->physics->wayLeftLine2->body->SetActive(false);
		App->physics->wayLeftChain->body->SetActive(true);
		boolRampLeftSensor = false;
		rampLeftDraw = false;
	}
	if (boolRampLeftSensorBack)
	{
		App->physics->wayLeftLine1->body->SetActive(true);
		App->physics->wayLeftLine2->body->SetActive(true);
		App->physics->wayLeftChain->body->SetActive(false);
		boolRampLeftSensorBack = false;
		rampLeftDraw = true;
	}
	//Sensors Right
	if (boolRampRightSensor)
	{
		App->physics->wayRightLine->body->SetActive(false);
		App->physics->wayRightChain->body->SetActive(true);
		boolRampRightSensor = false;
		rampRightDraw = false;
	}
	if (boolRampRightSensorBack)
	{
		App->physics->wayRightLine->body->SetActive(true);
		App->physics->wayRightChain->body->SetActive(false);
		boolRampRightSensorBack = false;
		rampRightDraw = true;
	}

	//Hit of Bouncer Balls
	rect = { 1832,12,56,53 };
	if (bouncerBallHit1)
	{
		App->renderer->Blit(assets, 110, 162, &rect);
		if(timerBouncerBallHit1->check(200))bouncerBallHit1 = false;
	}
	if (bouncerBallHit2)
	{
		App->renderer->Blit(assets, 90, 227, &rect);
		if (timerBouncerBallHit2->check(200))bouncerBallHit2 = false;
	}
	if (bouncerBallHit3)
	{
		App->renderer->Blit(assets, 157, 210, &rect);
		if (timerBouncerBallHit3->check(200))bouncerBallHit3 = false;
	}
	//Draw Ramp
	if (rampDraw)
	{
		rect = { 12,87,84,312 };
		App->renderer->Blit(assets, 141, 50, &rect);
	}
	if (!bouncerBallDraw)rampDraw = false;//depende de bouncerBallDraw porque queremos primero dejar de pintar las bolas que la rampa
	if (rampLeftDraw)
	{
		rect = { 107,87,89,140 };
		App->renderer->Blit(assets, 54, 414, &rect);
	}
	if (rampRightDraw)
	{
		rect = { 208,87,61,115 };
		App->renderer->Blit(assets, 326, 451, &rect);
	}
	//Animation of Neon-------------------
	
	if ((score % 100)==0 && score!=0 && !scoreBonus && checkNeon != score)
	{
		checkNeon = score;
		scoreBonus = true;
		neonTimer->Start();
	}

	if (scoreBonus)
	{
		
		if (neonTimer->check(2000))
		{
			scoreBonus = false;
			checkNeon = score;
		}
		if (!neonTimer->check(2000))
		{
			neon.currentAnimation->Update();
			SDL_Rect rectPlayer;
			rectPlayer = neon.currentAnimation->GetCurrentFrame();
			App->renderer->Blit(neon.texture, 0, 0, &rectPlayer);
		}
	}
	if (lives == 0)
	{
		App->renderer->Blit(gameOver, 0, 0);
		if (App->input->GetKey(SDL_SCANCODE_KP_ENTER) == KEY_DOWN
			|| App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN
			|| App->input->GetKey(SDL_SCANCODE_RETURN2) == KEY_DOWN)lives = 5, score = 0;
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
	App->renderer->Blit(assets, 347, 897, new SDL_Rect({ 1723,18,27,27 }));

	DrawLives();
	App->fonts->BlitText( 35,885, 0, char_type);
}
void ModuleSceneIntro::DrawLives()
{
	std::stringstream strsLives;
	strsLives << lives;
	std::string temp_strLives = strsLives.str();
	char* char_typeLives = (char*)temp_strLives.c_str();
	App->fonts->BlitText(380, 885, 0, char_typeLives);
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	//Sensor Respawn Ball
	if (bodyA == sensor || bodyB == sensor) {

		App->audio->PlayFx(bonus_fx);
		App->audio->PlayMusic("pinball/bounceAxe.wav");
		lives--;
		sensed = true;
	}
	//Sensor input kicker block
	if (bodyA == kickerPathSensor && bodyB == circles.getLast()->data ||
		bodyB == kickerPathSensor && bodyA == circles.getLast()->data) {

		if (FlipperKickerup != true) {
			FlipperKickerup = true;
		}
	}
	//Sensors of ramp upstairs
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

		if (boolRampSensorBack != true) {
			boolRampSensorBack = true;
		}
	}
	//Sensors of ramp downstairs
	//Sensors Ramp Left
	if (bodyA == rampLeftSensor && bodyB == circles.getLast()->data ||
		bodyB == rampLeftSensor && bodyA == circles.getLast()->data ||
		bodyA == rampLeftSensor2 && bodyB == circles.getLast()->data ||
		bodyB == rampLeftSensor2 && bodyA == circles.getLast()->data) {

		if (boolRampLeftSensor != true) {
			boolRampLeftSensor = true;
		}
	}
	if (bodyA == rampLeftSensorBack && bodyB == circles.getLast()->data ||
		bodyB == rampLeftSensorBack && bodyA == circles.getLast()->data ||
		bodyA == rampLeftSensorBack2 && bodyB == circles.getLast()->data ||
		bodyB == rampLeftSensorBack2 && bodyA == circles.getLast()->data) {

		if (boolRampLeftSensorBack != true) {
			boolRampLeftSensorBack = true;
		}
	}

	if (bodyA != nullptr && bodyB != nullptr)
		if (bodyA->type == TypePhysbody::BOUNCER || bodyB->type == TypePhysbody::BOUNCER)
		{
			App->audio->PlayFx(bonus_fx);
			score += 10;
		}
	if (bodyA != nullptr && bodyB != nullptr)
		if (bodyA->type == TypePhysbody::BOUNCER_BALL || bodyB->type == TypePhysbody::BOUNCER_BALL)
		{
			App->audio->PlayFx(bonus_fx);
			score += 10;
		}
	//Sensors Ramp Right
	if (bodyA == rampRightSensor && bodyB == circles.getLast()->data ||
		bodyB == rampRightSensor && bodyA == circles.getLast()->data ||
		bodyA == rampRightSensor2 && bodyB == circles.getLast()->data ||
		bodyB == rampRightSensor2 && bodyA == circles.getLast()->data) {

		if (boolRampRightSensor != true) {
			boolRampRightSensor = true;
		}
	}
	if (bodyA == rampRightSensorBack && bodyB == circles.getLast()->data ||
		bodyB == rampRightSensorBack && bodyA == circles.getLast()->data ||
		bodyA == rampRightSensorBack2 && bodyB == circles.getLast()->data ||
		bodyB == rampRightSensorBack2 && bodyA == circles.getLast()->data) {

		if (boolRampRightSensorBack != true) {
			boolRampRightSensorBack = true;
		}
	}
	//Sensors Bouncers Balls
	if (bodyA == App->physics->bouncerBall1 && bodyB == circles.getLast()->data ||
		bodyB == App->physics->bouncerBall1 && bodyA == circles.getLast()->data) {
		if (bouncerBallHit1 != true) {
			bouncerBallHit1 = true;
		}
		timerBouncerBallHit1->Start();
	}
	if (bodyA == App->physics->bouncerBall2 && bodyB == circles.getLast()->data ||
		bodyB == App->physics->bouncerBall2 && bodyA == circles.getLast()->data) {
		if (bouncerBallHit2 != true) {
			bouncerBallHit2 = true;
		}
		timerBouncerBallHit2->Start();
	}
	if (bodyA == App->physics->bouncerBall3 && bodyB == circles.getLast()->data ||
		bodyB == App->physics->bouncerBall3 && bodyA == circles.getLast()->data) {
		if (bouncerBallHit3 != true) {
			bouncerBallHit3 = true;
		}
		timerBouncerBallHit3->Start();
	}
}

