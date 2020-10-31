#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"

#include "Defs.h"
#include "Log.h"

Scene::Scene() : Module()
{
	name.Create("scene");
}

// Destructor
Scene::~Scene()
{}

// Called before render is available
bool Scene::Awake()
{
	LOG("Loading Scene");
	
	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	app->map->Load("level1.tmx");

	return true;
}

// Called each loop iteration
bool Scene::PreUpdate()
{
	return true;
}

// Called each loop iteration
bool Scene::Update(float dt)
{
	if(app->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		app->render->camera.y += 20;

	if(app->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		app->render->camera.y -= 20;

	if(app->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		app->render->camera.x += 40;

	if(app->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		app->render->camera.x -= 40;

	if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN) {
		app->RequestLoad();
		LOG("LOAD REQUESTED");
	}

	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN){
		app->RequestSave();
		LOG("SAVE REQUESTED");
	}

	// 8 to volume down and 9 to volume up
	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_DOWN) {
		app->audio->VolumeDown();
		LOG("Volume down");
	}if (app->input->GetKey(SDL_SCANCODE_9) == KEY_DOWN) {
		app->audio->VolumeUp();
		LOG("Volume up");
	}

	return true;
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
