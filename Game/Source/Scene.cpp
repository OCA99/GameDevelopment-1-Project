#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "Scene.h"
#include "Map.h"
#include "Player.h"
#include "Audio.h"
#include "Parallax.h"

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

	fullScreenRect = SDL_Rect({ 0, 0, app->render->camera.w, app->render->camera.h });
	app->audio->PlayMusic("Assets/audio/music/song.ogg");

	//screenTexture = app->tex->Load("Assets/title screen/title and end screen.png");
	screenTexture = app->tex->Load("Assets/title screen/reduced title screen.png");
	if (screenTexture == nullptr)LOG("could'nt load title screen");

	titleScreenAnim.PushBack({ 0,0,480,270});
	titleScreenAnim.PushBack({ 0,270, 480, 270 });

	/*titleScreenAnim.PushBack({ 0,0,206,78 });
	titleScreenAnim.PushBack({ 0,78,206,78 });*/

	gameOverAnim.PushBack({ 0,540,480,270 });
	gameOverAnim.PushBack({ 0,810,480,270 });

	logoAnim.PushBack({ 480,0,480,270 });

	turnOffAnim.PushBack({ 0,0,0,0, });


	titleScreenAnim.loop = gameOverAnim.loop = true;
	titleScreenAnim.speed = gameOverAnim.speed = 0.02f;

	screenDisplayAnim = &logoAnim;

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
	if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && gameplayState == LOGO_SCREEN)
	{
		FadeToNewState(TITLE_SCREEN);
	}
	else if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && gameplayState == TITLE_SCREEN)
	{
		FadeToNewState(PLAYING);
	}
	else if (app->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN && gameplayState == GAME_OVER_SCREEN)
	{
		FadeToNewState(PLAYING);
	}

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		app->RequestLoad();
		LOG("LOAD REQUESTED");
	}

	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN){
		app->RequestSave();
		LOG("SAVE REQUESTED");
	}

	if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		LoadLevel("level1.tmx");
	}

	if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) {
		LoadLevel("level2.tmx");
	}

	if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) {
		LoadLevel(currentLevel);
	}

	// 8 to volume down and 9 to volume up
	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_REPEAT) {
		app->audio->VolumeDown();
		LOG("Volume down");
	}if (app->input->GetKey(SDL_SCANCODE_9) == KEY_REPEAT) {
		app->audio->VolumeUp();
		LOG("Volume up");
	}

	if (gameplayState != targetState) {
		currentFade += 0.02f;
		if (currentFade >= 1.0f) {
			currentFade = 1.0f;
			ChangeGameplayState(targetState);
		}
	}
	else if (currentFade > 0.0f) {
		currentFade -= 0.02f;
	}
	else if (currentFade <= 0.0f) {
		currentFade = 0.0f;
		fading = false;
	}

	screenDisplayAnim->Update();

	return true;
}

void Scene::FadeToNewState(GameplayState newState) {
	if (gameplayState == newState) return;
	if (fading) return;
	targetState = newState;
	currentFade = 0.0f;
	fading = true;
}

void Scene::ChangeGameplayState(GameplayState newState) {
	if (gameplayState == newState) return;

	switch (newState) {
	case PLAYING:
		screenDisplayAnim = &turnOffAnim;
		app->parallax->enabled = true;
		gameplayState = PLAYING;
		currentLevel.Create("level1.tmx");
		app->map->Load("level1.tmx");
		app->player->Reload();
		break;
	case TITLE_SCREEN:
		screenDisplayAnim = &titleScreenAnim;
		gameplayState = TITLE_SCREEN;
		app->parallax->enabled = false;
		app->map->CleanUp();
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		break;
	case GAME_OVER_SCREEN:
		screenDisplayAnim = &gameOverAnim;
		gameplayState = GAME_OVER_SCREEN;
		app->parallax->enabled = false;
		app->map->CleanUp();
		app->render->camera.x = 0;
		app->render->camera.y = 0;
		break;
	}
}


void Scene::LoadLevel(SString name)
{
	
	app->player->isDead = false;
	currentLevel = name;
	app->map->Load(name.GetString());
	app->player->Reload();
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

	SDL_Rect rect = screenDisplayAnim->GetCurrentFrame();
	
	app->render->DrawTexture(screenTexture, 0, 0, &rect);

	float adjustedFade = currentFade;
	if (adjustedFade < 0.0f) adjustedFade = 0.0f;
	if (adjustedFade > 1.0f) adjustedFade = 1.0f;

	int alpha = adjustedFade * 255.0f;

	app->render->DrawRectangle(fullScreenRect, 0, 0, 0, alpha, true, false);
	
	

	return ret;
}

// Called before quitting
bool Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}
