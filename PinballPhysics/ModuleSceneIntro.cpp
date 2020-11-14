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
	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT+35, SCREEN_WIDTH, 25);

	//Kicker
	kikerInvisble = {281,202,31,47};
	kikerRect = { 281,87,31,96 };
	kiker.anchor = App->physics->CreateStaticRectangle(455, 820, 5, 5);
	kiker.body = App->physics->CreateRectangle(455, 750, 20, 10);
	kiker.joint = App->physics->CreatePrismaticJoint(kiker.anchor, kiker.body, 1, -80, -20, 50);

	circles.add(App->physics->CreateCircle(450, 730, 12, true));
	circles.getLast()->data->listener = this;

	// Bouncer bols
	float bouncerBallsRestitution=2;
	App->physics->CreateCircle(138, 189, 15, false, bouncerBallsRestitution);
	App->physics->CreateCircle(118, 254, 15, false, bouncerBallsRestitution);
	App->physics->CreateCircle(188, 238, 15, false, bouncerBallsRestitution);


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
	if (App->input->GetKey(SDL_SCANCODE_4) == KEY_REPEAT)
	{
		
		
	}

	// Circle  -----------------------------------------//////
	p2List_item<PhysBody*>* c = circles.getFirst();
	SDL_Rect rect = {1723,18,27,27};
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		//if(c->data->Contains(App->input->GetMouseX(), App->input->GetMouseY()))
		App->renderer->Blit(assets, x, y, &rect, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	

	
	/// ------ Pala------------
	 c= App->physics->flippersL.getFirst();
	 rect = {1899,12,73,39};
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(assets, x-39, y-25, &rect, 1.0f, c->data->GetRotation()- (RADTODEG*0.50));
		c = c->next;
	}
	 c= App->physics->flippersR.getFirst();
	 rect = { 2067,12,73,39};
	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(assets, x-39, y-22, &rect, 1.0f, c->data->GetRotation()+ (RADTODEG*0.50));
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

	/// Delete Ball / Spawn next Ball
	if (sensed)
	{
		circles.getLast()->data->body->GetWorld()->DestroyBody(circles.getLast()->data->body);
		circles.del(circles.getLast());
		circles.add(App->physics->CreateCircle(450, 720, 12, true));
		circles.getLast()->data->listener = this;

		sensed = false;
	}
	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;
	
		if (bodyA == sensor  ||	bodyB == sensor ) {

			App->audio->PlayFx(bonus_fx);
			sensed = true;

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
