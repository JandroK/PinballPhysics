#include "Globals.h"
#include "Application.h"
#include "ModuleInput.h"
#include "ModuleRender.h"
#include "ModulePhysics.h"
#include "ModuleSceneIntro.h"
#include "ModuleAudio.h"
#include "p2Point.h"
#include "math.h"

#ifdef _DEBUG
#pragma comment( lib, "Box2D/libx86/Debug/Box2D.lib" )
#else
#pragma comment( lib, "Box2D/libx86/Release/Box2D.lib" )
#endif

ModulePhysics::ModulePhysics(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	world = NULL;
	mouse_joint = NULL;
	debug = true;
}

// Destructor
ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Start()
{
	LOG("Creating Physics 2D environment");

	world = new b2World(b2Vec2(GRAVITY_X, -GRAVITY_Y));
	world->SetContactListener(this);

	// needed to create joints like mouse joint
	b2BodyDef bd;
	ground = world->CreateBody(&bd);

	// outline of map
	int pinball_background[140] = {
		148,11,
		108,23,
		56,66,
		25,131,
		18,223,
		44,360,
		67,424,
		48,452,
		55,520,
		28,557,
		28,594,
		54,624,
		54,676,
		12,684,
		13,800,
		53,842,
		53,914,
		82,914,
		82,835,
		42,789,
		42,721,
		54,721,
		54,768,
		146,822,
		138,841,
		168,865,
		168,913,
		302,913,
		302,867,
		337,847,
		326,818,
		380,789,
		380,719,
		395,720,
		395,818,
		372,852,
		372,912,
		400,912,
		400,859,
		422,826,
		422,695,
		414,684,
		381,684,
		383,647,
		423,607,
		423,575,
		379,526,
		395,452,
		397,404,
		385,389,
		351,389,
		330,413,
		321,423,
		320,378,
		374,361,
		380,352,
		379,337,
		371,325,
		402,312,
		422,288,
		422,132,
		436,132,
		436,830,
		467,830,
		467,128,
		458,82,
		434,52,
		397,23,
		346,12,
		310,10
	};
	CreateChain(0, 0, pinball_background, 140);
	int triangleLeft[10] = {
		124,721,
		129,721,
		153,775,
		151,784,
		120,768
	};
	CreateChain(0, 0, triangleLeft, 10,2);
	int triangleRight[10] = {
		350,720,
		345,720,
		320,773,
		322,784,
		352,773
	};
	CreateChain(0, 0, triangleRight, 10,2);
	int segmentLeft[14] = {
		73,542,
		107,635,
		107,656,
		84,670,
		84,618,
		56,581,
		56,566
	};
	CreateChain(0, 0, segmentLeft, 14);
	int segmentRight[14] = {
		368,555,
		332,683,
		350,685,
		350,652,
		362,627,
		393,600,
		393,581
	};
	CreateChain(0, 0, segmentRight, 14);
	int segmentSmall[12] = {
		207,369,
		241,378,
		242,398,
		252,406,
		253,432,
		211,416
	};
	CreateChain(0, 0, segmentSmall, 12);
	int segmentBigCollision[52] = {
		190,330,
		175,372,
		175,405,
		86,341,
		61,290,
		51,235,
		52,170,
		68,111,
		95,75,
		133,49,
		171,45,
		310,42,
		310,57,
		204,57,
		173,66,
		154,83,
		147,105,
		125,107,
		101,117,
		85,138,
		72,170,
		68,206,
		68,250,
		77,276,
		112,296,
		160,317,
	};
	segmentBig=CreateChain(0, 0, segmentBigCollision, 52);
	int cornerCollision[12] = {
		309,88,
		309,150,
		293,123,
		270,103,
		147,103,
		207,88
	};
	corner=CreateChain(0, 0, cornerCollision, 12);
	int pieceCentralRed[26] = {
		105, 173,
		115, 152,
		139, 140,
		185, 133,
		232, 133,
		251, 146,
		255, 160,
		248, 169,
		164, 170,
		141, 160,
		120, 165,
		115, 175,
		107, 174
	};
	pieceRed = CreateChain(0, 0, pieceCentralRed, 26);
	int rampVertex[28] = {
		175,405,
		175,372,
		167,316,
		147,105,
		154,83,
		173,66,
		204,57,
		310,57,
		310,88,
		207,88,
		191,103,
		181,118,
		207,369,
		211,416
	};
	ramp=CreateChain(0, 0, rampVertex, 28);
	ramp->body->SetActive(false);
	// Bouncer bols
	float bouncerBallsRestitution = 2;
	bouncerBall1 = CreateCircle(138, 189, 15, false, bouncerBallsRestitution);
	bouncerBall2 = CreateCircle(118, 254, 15, false, bouncerBallsRestitution);
	bouncerBall3 = CreateCircle(188, 238, 15, false, bouncerBallsRestitution);

	//CreateFlipperL(154, 837);
	//CreateFlipperR(100, 100);

	CreatePairFlippers(154, 837, 169);
	CreatePairFlippers(197, 345, 173);

	return true;
}

update_status ModulePhysics::PreUpdate()
{
	world->Step(1.0f / 60.0f, 6, 2);

	for(b2Contact* c = world->GetContactList(); c; c = c->GetNext())
	{
		if(c->GetFixtureA()->IsSensor() && c->IsTouching())
		{
			PhysBody* pb1 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			PhysBody* pb2 = (PhysBody*)c->GetFixtureA()->GetBody()->GetUserData();
			if (pb1 && pb2 && pb1->listener)
			{
				pb1->listener->OnCollision(pb1, pb2);
				App->audio->PlayFx(App->scene_intro->bonus_fx);
			}
		}
	}
	
	return UPDATE_CONTINUE;
}

PhysBody* ModulePhysics::CreateCircle(int x, int y, int radius, bool dynamic, float _restitution)
{
	b2BodyDef body;
	if (dynamic == true)body.type = b2_dynamicBody;
	else body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	if (dynamic == true)b->IsBullet();
	b2CircleShape shape;
	shape.m_radius = PIXEL_TO_METERS(radius);
	b2FixtureDef fixture;
	fixture.shape = &shape;
	fixture.friction = 1.0f;
	fixture.restitution = _restitution;
	fixture.density = 8.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = radius;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangle(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 10.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

PhysBody* ModulePhysics::CreateRectangleSensor(int x, int y, int width, int height)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.density = 1.0f;
	fixture.isSensor = true;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width;
	pbody->height = height;

	return pbody;
}

PhysBody* ModulePhysics::CreateChain(int x, int y, int* points, int size, float _restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	b2Body* b = world->CreateBody(&body);

	b2ChainShape shape;
	b2Vec2* p = new b2Vec2[size / 2];

	for(uint i = 0; i < size / 2; ++i)
	{
		p[i].x = PIXEL_TO_METERS(points[i * 2 + 0]);
		p[i].y = PIXEL_TO_METERS(points[i * 2 + 1]);
	}

	shape.CreateLoop(p, size / 2);

	b2FixtureDef fixture;
	fixture.restitution = _restitution;
	fixture.density = 1.0f;
	fixture.shape = &shape;

	b->CreateFixture(&fixture);

	delete p;

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = pbody->height = 0;

	return pbody;
}

update_status ModulePhysics::PostUpdate()
{
	if(App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		debug = !debug;

	if(!debug)
		return UPDATE_CONTINUE;

	// Bonus code: this will iterate all objects in the world and draw the circles
	// You need to provide your own macro to translate meters to pixels
	for(b2Body* b = world->GetBodyList(); b; b = b->GetNext())
	{
		for(b2Fixture* f = b->GetFixtureList(); f; f = f->GetNext())
		{
			switch(f->GetType())
			{
				// Draw circles ------------------------------------------------
				case b2Shape::e_circle:
				{
					b2CircleShape* shape = (b2CircleShape*)f->GetShape();
					b2Vec2 pos = f->GetBody()->GetPosition();
					App->renderer->DrawCircle(METERS_TO_PIXELS(pos.x), METERS_TO_PIXELS(pos.y), METERS_TO_PIXELS(shape->m_radius), 255, 255, 255);
					
					p2List_item<PhysBody*>* c = circlesToDelete.getFirst();
					for (c; c; c = c->next)
					{
						if (b ==c->data->body )
						{
						
						/*	circlesToDelete.del(c);
							world->DestroyBody(c->data->body);
							*///world->DestroyBody(b);
						}


					}
				}
				break;

				// Draw polygons ------------------------------------------------
				case b2Shape::e_polygon:
				{
					b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();
					int32 count = polygonShape->GetVertexCount();
					b2Vec2 prev, v;

					for(int32 i = 0; i < count; ++i)
					{
						v = b->GetWorldPoint(polygonShape->GetVertex(i));
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);

						prev = v;
					}

					v = b->GetWorldPoint(polygonShape->GetVertex(0));
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 255, 100, 100);
				}
				break;

				// Draw chains contour -------------------------------------------
				case b2Shape::e_chain:
				{
					b2ChainShape* shape = (b2ChainShape*)f->GetShape();
					b2Vec2 prev, v;

					for(int32 i = 0; i < shape->m_count; ++i)
					{
						v = b->GetWorldPoint(shape->m_vertices[i]);
						if(i > 0)
							App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
						prev = v;
					}

					v = b->GetWorldPoint(shape->m_vertices[0]);
					App->renderer->DrawLine(METERS_TO_PIXELS(prev.x), METERS_TO_PIXELS(prev.y), METERS_TO_PIXELS(v.x), METERS_TO_PIXELS(v.y), 100, 255, 100);
				}
				break;

				// Draw a single segment(edge) ----------------------------------
				case b2Shape::e_edge:
				{
					b2EdgeShape* shape = (b2EdgeShape*)f->GetShape();
					b2Vec2 v1, v2;

					v1 = b->GetWorldPoint(shape->m_vertex0);
					v1 = b->GetWorldPoint(shape->m_vertex1);
					App->renderer->DrawLine(METERS_TO_PIXELS(v1.x), METERS_TO_PIXELS(v1.y), METERS_TO_PIXELS(v2.x), METERS_TO_PIXELS(v2.y), 100, 100, 255);
				}
				break;
			}

			// TODO 1: If mouse button 1 is pressed test if the current body contains mouse position
			if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_DOWN && isClicked == false)
			{
				bodyClicked = new PhysBody();
				bodyClicked->body = b;
				b2Vec2 mousePos;
				mousePos.x = App->input->GetMouseX();
				mousePos.y = App->input->GetMouseY();
				if (bodyClicked->Contains(mousePos.x, mousePos.y))
				{
					def.bodyA = ground;
					def.bodyB = bodyClicked->body;
					def.target.x = PIXEL_TO_METERS(mousePos.x);
					def.target.y = PIXEL_TO_METERS(mousePos.y);
					def.dampingRatio = 0.5f;
					def.frequencyHz = 2.0f;
					def.maxForce = 100.0f * bodyClicked->body->GetMass();
					mouse_joint = (b2MouseJoint*)world->CreateJoint(&def);
					isClicked = true;
				}
				else
				{
					delete bodyClicked;
					bodyClicked = nullptr;
				}
			}
			
		}
	}

	// If a body was selected we will attach a mouse joint to it
	// so we can pull it around
	// TODO 2: If a body was selected, create a mouse joint
	// using mouse_joint class property


	// TODO 3: If the player keeps pressing the mouse button, update
	// target position and draw a red line between both anchor points
	if (isClicked == true)
	{
		if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
		{
			b2Vec2 mousePos;
			mousePos.x = App->input->GetMouseX();
			mousePos.y = App->input->GetMouseY();
			mouse_joint->SetTarget(PIXEL_TO_METERS(mousePos));
			App->renderer->DrawLine(METERS_TO_PIXELS(def.bodyB->GetPosition().x), METERS_TO_PIXELS(def.bodyB->GetPosition().y), mousePos.x, mousePos.y, 255, 0, 0);
		}
		else if (App->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_UP)
		{
			world->DestroyJoint(mouse_joint);
			mouse_joint = NULL;
			isClicked = false;
		}
	}
	
	// TODO 4: If the player releases the mouse button, destroy the joint


	return UPDATE_CONTINUE;
}

// Called before quitting
bool ModulePhysics::CleanUp()
{
	LOG("Destroying physics world");

	// Delete the whole physics world!
	delete world;

	return true;
}

void PhysBody::GetPosition(int& x, int &y) const
{
	b2Vec2 pos = body->GetPosition();
	x = METERS_TO_PIXELS(pos.x) - (width);
	y = METERS_TO_PIXELS(pos.y) - (height);
}



float PhysBody::GetRotation() const
{
	return RADTODEG * body->GetAngle();
}

bool PhysBody::Contains(int x, int y) const
{
	// TODO 1: Write the code to return true in case the point
	// is inside ANY of the shapes contained by this body

	b2Vec2 pos = { PIXEL_TO_METERS(x),PIXEL_TO_METERS(y) };
	bool ret = false;
	for (b2Fixture* firstFisxture = this->body->GetFixtureList(); firstFisxture != nullptr; firstFisxture = firstFisxture->GetNext())
	{
		if (firstFisxture->GetShape()->TestPoint(this->body->GetTransform(), pos)) return true;
	}

	return false;
}

int PhysBody::RayCast(int x1, int y1, int x2, int y2, float& normal_x, float& normal_y) const
{
	int ret = -1;

	b2RayCastInput input;
	b2RayCastOutput output;

	input.p1.Set(PIXEL_TO_METERS(x1), PIXEL_TO_METERS(y1));
	input.p2.Set(PIXEL_TO_METERS(x2), PIXEL_TO_METERS(y2));
	input.maxFraction = 1.0f;

	const b2Fixture* fixture = body->GetFixtureList();

	while(fixture != NULL)
	{
		if(fixture->GetShape()->RayCast(&output, input, body->GetTransform(), 0) == true)
		{
			// do we want the normal ?

			float fx = x2 - x1;
			float fy = y2 - y1;
			float dist = sqrtf((fx*fx) + (fy*fy));

			normal_x = output.normal.x;
			normal_y = output.normal.y;

			return output.fraction * dist;
		}
		fixture = fixture->GetNext();
	}

	return ret;
}

void ModulePhysics::BeginContact(b2Contact* contact)
{
	PhysBody* physA = (PhysBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysBody* physB = (PhysBody*)contact->GetFixtureB()->GetBody()->GetUserData();

	if(physA && physA->listener != NULL)
		physA->listener->OnCollision(physA, physB);

	if(physB && physB->listener != NULL)
		physB->listener->OnCollision(physB, physA);
}



void ModulePhysics::CreateFlipperL(int x, int y)
{
	// Pivot
	int w = 2;
	int h = 2;
	b2BodyDef body;
	b2FixtureDef fixture;
	b2PolygonShape box;

	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x + 0.16f), PIXEL_TO_METERS(y + 0.08f));
	pivotJoins.add(world->CreateBody(&body));
	box.SetAsBox(PIXEL_TO_METERS(w) * 0.5f, PIXEL_TO_METERS(h) * 0.5f);
	fixture.shape = &box;

	pivotJoins.getLast()->data->CreateFixture(&fixture);

	// Flipper

	x += 35;
	w = 75;
	h = 15;

	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	PhysBody* pbody = new PhysBody();
	pbody->body = world->CreateBody(&body);

	flippersL.add(pbody);
	box.SetAsBox(PIXEL_TO_METERS(w) * 0.5f, PIXEL_TO_METERS(h) * 0.5f);
	fixture.shape = &box;
	fixture.density = 4;

	flippersL.getLast()->data->body->CreateFixture(&fixture);

	// Join

	JoinFlipper.Initialize(pivotJoins.getLast()->data, flippersL.getLast()->data->body, pivotJoins.getLast()->data->GetWorldCenter());
	JoinFlipper.lowerAngle = -0.25f * b2_pi; 
	JoinFlipper.upperAngle = 0.14f * b2_pi; 
	JoinFlipper.enableLimit = true;
	JoinFlipper.maxMotorTorque = 8.0f;
	JoinFlipper.motorSpeed = 3.5f;
	JoinFlipper.enableMotor = true;

	world->CreateJoint(&JoinFlipper);

	flippersL.getLast()->data->body->IsBullet();
}

void ModulePhysics::CreateFlipperR(int x, int y)
{
	// Pivot
	int w = 2;
	int h = 2;
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x-0.18f), PIXEL_TO_METERS(y+0.10f));


	pivotJoins.add(world->CreateBody(&body));

	b2FixtureDef fixture;
	b2PolygonShape box;

	box.SetAsBox(PIXEL_TO_METERS(w) * 0.5f, PIXEL_TO_METERS(h) * 0.5f);
	fixture.shape = &box;

	pivotJoins.getLast()->data->CreateFixture(&fixture);

	// Flipper

	x -= 35;
	w = 75;
	h = 15;

	body.type = b2_dynamicBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));

	PhysBody* pbody = new PhysBody();
	pbody->body = world->CreateBody(&body);

	flippersR.add(pbody);
	box.SetAsBox(PIXEL_TO_METERS(w) * 0.5f, PIXEL_TO_METERS(h) * 0.5f);
	fixture.shape = &box;
	fixture.density = 4;

	flippersR.getLast()->data->body->CreateFixture(&fixture);

	// Join

	JoinFlipper.Initialize(pivotJoins.getLast()->data, flippersR.getLast()->data->body, pivotJoins.getLast()->data->GetWorldCenter());
	JoinFlipper.lowerAngle = -0.14f * b2_pi; 
	JoinFlipper.upperAngle = 0.25f * b2_pi; 
	JoinFlipper.enableLimit = true;
	JoinFlipper.maxMotorTorque = 8.0f;
	JoinFlipper.motorSpeed = -3.5f;
	JoinFlipper.enableMotor = true;

	world->CreateJoint(&JoinFlipper);

	flippersR.getLast()->data->body->IsBullet();
}

void ModulePhysics::CreatePairFlippers(int x, int y, int separation)
{
	CreateFlipperL(x, y);
	CreateFlipperR(x+separation, y);
}
PhysBody* ModulePhysics::CreateStaticRectangle(int x, int y, int width, int height, int _restitution)
{
	b2BodyDef body;
	body.type = b2_staticBody;
	body.position.Set(PIXEL_TO_METERS(x), PIXEL_TO_METERS(y));
	b2Body* b = world->CreateBody(&body);
	b2PolygonShape box;
	box.SetAsBox(PIXEL_TO_METERS(width) * 0.5f, PIXEL_TO_METERS(height) * 0.5f);

	b2FixtureDef fixture;
	fixture.shape = &box;
	fixture.restitution = _restitution;
	fixture.density = 1.0f;

	b->CreateFixture(&fixture);

	PhysBody* pbody = new PhysBody();
	pbody->body = b;
	b->SetUserData(pbody);
	pbody->width = width * 0.5f;
	pbody->height = height * 0.5f;

	return pbody;
}

b2PrismaticJoint* ModulePhysics::CreatePrismaticJoint(PhysBody* anchor, PhysBody* body, int max_move, int min_move, int motor_speed, int max_force)
{
	b2PrismaticJointDef def;

	def.bodyA = anchor->body;
	def.bodyB = body->body;

	def.collideConnected = false;

	def.enableLimit = true;
	def.enableMotor = true;


	b2Vec2 localAxis(0, 1);
	def.localAxisA = localAxis;

	//set limmits
	def.lowerTranslation = PIXEL_TO_METERS(min_move);
	def.upperTranslation = PIXEL_TO_METERS(max_move);

	//set values
	def.motorSpeed = motor_speed;
	def.maxMotorForce = max_force;

	return (b2PrismaticJoint*)world->CreateJoint(&def);
}