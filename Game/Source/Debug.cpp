#define _CRT_SECURE_NO_WARNINGS

#include "Debug.h"
#include "Map.h"
#include "App.h"
#include "Player.h"
#include "Input.h"
#include "Collisions.h"
#include "Entities.h"
#include "GuiManager.h"
#include "ModuleUI.h"
#include "Log.h";
#include "Window.h"

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

	if (app->input->GetKey(SDL_SCANCODE_F8) == KEY_DOWN)
	{
		app->guimanager->showDebug = !app->guimanager->showDebug;
		app->ui->showDebug = !app->ui->showDebug;
	}

	if (app->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN)
	{
		ToggleColliders();
		showPaths = !showPaths;
	}

	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		GodMode();
	}

	if (app->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN)
	{
		if (capFrameRate)
		{
			storeFrameRateCap = app->cappedMs;
			app->cappedMs = 1000.0f / 30.0f;
			capFrameRate = false;
		}
		else
		{
			app->cappedMs = storeFrameRateCap;
			capFrameRate = true;
		}
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
	else if (app->collisions->showColliders == true)
	{
		app->collisions->showColliders = false;
	}
}

void Debug::GodMode()
{
	if (app->entities->GetPlayer()->godMode == false)
	{
		app->entities->GetPlayer()->ChangeState(app->entities->GetPlayer()->playerState, IDLE);
		app->entities->GetPlayer()->godMode = true;
		storeGravity = app->entities->GetPlayer()->gravity;
		app->entities->GetPlayer()->gravity = 0.0f;
		app->entities->GetPlayer()->verticalVelocity = 0.0f;
	}
	else if (app->entities->GetPlayer()->godMode == true)
	{
		app->entities->GetPlayer()->gravity = storeGravity;
		app->entities->GetPlayer()->godMode = false;
	}
}