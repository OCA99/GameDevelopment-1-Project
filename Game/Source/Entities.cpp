#include "Entities.h"

#include "Log.h"
#include "App.h"
#include "Textures.h"

#include "Bat.h"
#include "Fruit.h"
#include "Heart.h"
#include "Pig.h"
#include "Knife.h"

#include "Player.h"

#include "ModuleUI.h"

#include "Optick/include/optick.h"

#include <stdlib.h>
#include <time.h>

Entities::Entities() : Module()
{
	name.Create("entities");
}

bool Entities::Awake(pugi::xml_node& config)
{
	pugi::xml_node entities = config.child("entities");

	enemySpeed = entities.attribute("enemySpeed").as_int();
	pigHealth = entities.attribute("pigHealth").as_int();
	enemyGravity = entities.attribute("enemyGravity").as_int();
	enemyJumpForce = entities.attribute("enemyJumpForce").as_int();
	knifeSpeed = entities.attribute("knifeSpeed").as_int();

	batTexturePath = config.child("batTexture").child_value();
	pigTexturePath = config.child("pigTexture").child_value();
	fruitTexturePath = config.child("fruitTexture").child_value();
	heartTexturePath = config.child("heartTexture").child_value();
	knifeTexturePath = config.child("knifeTexture").child_value();

	playerConfig = config.child("player");
	placeholderPlayer = new Player((Module*)this, fPoint(-1000.0f, -1000.0f), Entity::Type::PLAYER);
	placeholderPlayer->Awake(playerConfig);

	return true;
}

bool Entities::Load(pugi::xml_node& savedGame)
{
	pugi::xml_node ents = savedGame.child("entities");

	pugi::xml_node e;

	for (e = ents.child("entity"); e; e = e.next_sibling("entity"))
	{
		int type = e.attribute("type").as_int();
		float x = e.attribute("x").as_float();
		float y = e.attribute("y").as_float();
		fPoint position = fPoint(x, y);
		int r;
		Entity* en;
		int dir = 1;
		int h;
		switch (type)
		{
		case 0:
			r = rand() % 3;
			en = (Entity*)(new Fruit((Module*)this, position, fruitTexture, Entity::Type::FRUIT, r));
			entityList.Add(en);
			break;
		case 1:
			en = (Entity*)(new Heart((Module*)this, position, heartTexture, Entity::Type::HEART));
			entityList.Add(en);
			break;
		case 2:
			dir = e.attribute("direction").as_int();
			en = (Entity*)(new Knife((Module*)this, position, knifetexture, Entity::Type::KNIFE, dir, knifeSpeed));
			entityList.Add(en);
			break;
		case 3:
			en = (Entity*)(new Bat((Module*)this, position, batTexture, Entity::Type::BAT, enemySpeed));
			entityList.Add(en);
			break;
		case 4:
			h = e.attribute("health").as_int();
			en = (Entity*)(new Pig((Module*)this, position, pigTexture, Entity::Type::PIG, enemySpeed, h, enemyGravity, enemyJumpForce));
			entityList.Add(en);
			break;
		case 5:
			placeholderPlayer->position = position;
			int verticalVelocity = e.attribute("verticalVelocity").as_int();
			placeholderPlayer->verticalVelocity = verticalVelocity;
			int availableJumps = e.attribute("availableJumps").as_int();
			placeholderPlayer->availableJumps = availableJumps;
			int score = e.attribute("score").as_int();
			app->ui->score = score;
			int health = e.attribute("health").as_int();
			placeholderPlayer->health = health;
			bool checkpoint1 = e.attribute("checkpoint1").as_bool();
			placeholderPlayer->unlockedChekpoint1 = checkpoint1;
			bool checkpoint2 = e.attribute("checkpoint2").as_bool();
			placeholderPlayer->unlockedChekpoint2 = checkpoint2;
			float timer = e.attribute("timer").as_float();
			app->ui->timer = timer;
			int highScore = e.attribute("highscore").as_int();
			app->ui->highScore = highScore;
			entityList.Add(placeholderPlayer);
			placeholderPlayer->Reload();
			break;
		}
	}

	return true;
}

bool Entities::Save(pugi::xml_node& savedGame)
{
	pugi::xml_node ents = savedGame.append_child("entities");

	for (int i = 0; i < entityList.Count(); i++)
	{
		Entity* e = entityList[i];
		pugi::xml_node eNode = ents.append_child("entity");
		pugi::xml_attribute x = eNode.append_attribute("x");
		x.set_value(e->position.x);
		pugi::xml_attribute y = eNode.append_attribute("y");
		y.set_value(e->position.y);
		pugi::xml_attribute type = eNode.append_attribute("type");
		type.set_value((int)e->type);
		pugi::xml_attribute h;
		Pig* p;
		Knife* k;
		pugi::xml_attribute dir;
		switch (e->type)
		{
		case Entity::Type::PIG:
			p = (Pig*)e;
			h = eNode.append_attribute("health");
			h.set_value(p->health);
			break;
		case Entity::Type::KNIFE:
			k = (Knife*)e;
			dir = eNode.append_attribute("direction");
			dir.set_value(k->knifeDirection);
			break;
		case Entity::Type::PLAYER:
			Player* p = (Player*)e;
			pugi::xml_attribute vert = eNode.append_attribute("verticalVelocity");
			vert.set_value(p->verticalVelocity);
			pugi::xml_attribute availableJumps = eNode.append_attribute("availableJumps");
			availableJumps.set_value(p->availableJumps);
			pugi::xml_attribute score = eNode.append_attribute("score");
			score.set_value(app->ui->score);
			pugi::xml_attribute health = eNode.append_attribute("health");
			health.set_value(p->health);
			pugi::xml_attribute checkpoint1 = eNode.append_attribute("checkpoint1");
			checkpoint1.set_value(p->unlockedChekpoint1);
			pugi::xml_attribute checkpoint2 = eNode.append_attribute("checkpoint2");
			checkpoint2.set_value(p->unlockedChekpoint2);
			pugi::xml_attribute timer = eNode.append_attribute("timer");
			timer.set_value(app->ui->timer);
			pugi::xml_attribute highScore = eNode.append_attribute("highscore");
			highScore.set_value(app->ui->highScore);
			break;
		}
	}

	return true;
}

bool Entities::Start()
{
	batTexture = app->tex->Load(batTexturePath);
	pigTexture = app->tex->Load(pigTexturePath);
	fruitTexture = app->tex->Load(fruitTexturePath);
	heartTexture = app->tex->Load(heartTexturePath);
	knifetexture = app->tex->Load(knifeTexturePath);

	srand(time(NULL));

	placeholderPlayer->Start();

	return true;
}

bool Entities::PreUpdate()
{
	return true;
}

bool Entities::Update(float dt)
{
	OPTICK_EVENT("EntitiesUpdate", Optick::Category::GameLogic);

	if (app->ui->paused)
		return true;

	ListItem<Entity*>* start = entityList.start;

	while (start != nullptr)
	{
		if (start->data->pendingToDelete)
		{
			delete start->data;
			entityList.Del(start);
			start = start->next;
			continue;
		}

		start->data->Update(dt);
		start = start->next;
	}

	return true;
}

bool Entities::PostUpdate()
{
	OPTICK_EVENT("EntitiesPostUpdate", Optick::Category::Rendering);

	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* e = entityList.At(i);
		e->data->Draw();
	}

	return true;
}

bool Entities::CleanUp()
{
	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* e = entityList.At(i);
		if (e->data->type != Entity::Type::PLAYER)
			delete e->data;
	}

	entityList.Clear();

	return true;
}

void Entities::AddEntity(fPoint position, Entity::Type type)
{
	Entity* e;
	int r;
	int knifeDirection;

	switch (type)
	{
	case Entity::Type::BAT:
		e = (Entity*)(new Bat((Module*)this, position, batTexture, type, enemySpeed));
		entityList.Add(e);
		break;
	case Entity::Type::FRUIT:
		r = rand() % 3;
		e = (Entity*)(new Fruit((Module*)this, position, fruitTexture, type, r));
		entityList.Add(e);
		break;
	case Entity::Type::HEART:
		e = (Entity*)(new Heart((Module*)this, position, heartTexture, type));
		entityList.Add(e);
		break;
	case Entity::Type::PIG:
		e = (Entity*)(new Pig((Module*)this, position, pigTexture, type, enemySpeed, pigHealth, enemyGravity, enemyJumpForce));
		entityList.Add(e);
		break;
	case Entity::Type::KNIFE:
		knifeDirection = GetPlayer()->knifeDirection;
		e = (Entity*)(new Knife((Module*)this, position, knifetexture, type, knifeDirection, knifeSpeed));
		entityList.Add(e);
		break;
	case Entity::Type::PLAYER:
		placeholderPlayer->position = position;
		e = (Entity*)(placeholderPlayer);
		entityList.Add(e);
		break;
	}
}

void Entities::OnCollision(Collider* a, Collider* b, float dt)
{
	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* e = entityList.At(i);

		if (e->data->collider == a && b != nullptr)
		{
			e->data->Collision(b, dt);
		}
	}
}

void Entities::ResetEntities()
{
	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* e = entityList.At(i);

		e->data->Reset();
	}
}

Player* Entities::GetPlayer()
{
	for (int i = 0; i < entityList.Count(); i++)
	{
		ListItem<Entity*>* e = entityList.At(i);

		if (e->data->type == Entity::Type::PLAYER)
		{
			return (Player*)(e->data);
		}
	}

	return placeholderPlayer;
}