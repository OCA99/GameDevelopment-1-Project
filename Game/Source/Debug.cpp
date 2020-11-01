#define _CRT_SECURE_NO_WARNINGS

#include "Debug.h"
#include "Map.h"
#include "App.h"
#include "Player.h"
#include "Input.h"
#include "Collisions.h"
#include "Log.h";

Debug::Debug() : Module()
{
	name.Create("debug");
}


Debug::~Debug()
{

}

bool Debug::Awake()
{
	return true;
}

bool Debug::Start()
{
	return true;
}

bool Debug::PreUpdate()
{
	return true;
}

bool Debug::Update(float dt)
{
	bool ret = true;

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		ToggleColliders();
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode();
	}

	return ret;
}

bool Debug::PostUpdate()
{

	return true;
}

bool Debug::CleanUp()
{
	return true;
}

void Debug::ToggleColliders()
{
	if (app->collisions->showColliders == false)
	{
		app->collisions->showColliders = true;
	}
	else if (app->collisions->showColliders == true) {
		app->collisions->showColliders = false;
	}
}

void Debug::GodMode()
{
	if (app->player->godMode == false)
	{
		app->player->godMode = true;
		storeGravity = app->player->gravity;
		app->player->gravity = 0;
	}
	else if (app->player->godMode == true) {
		app->player->gravity = storeGravity;
		app->player->godMode = false;
	}
}