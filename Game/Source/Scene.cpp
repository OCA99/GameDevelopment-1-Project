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
#include "ModuleUI.h"
#include "GuiManager.h"

#include "Optick/include/optick.h"

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
bool Scene::Awake(pugi::xml_node& config)
{
	LOG("Loading Scene");

	pugi::xml_node music = config.child("music");

	musicPath = music.attribute("path").as_string();

	pugi::xml_node texture = config.child("texture");

	screenTexturePath = texture.attribute("screen").as_string();

	bool ret = true;

	return ret;
}

// Called before the first frame
bool Scene::Start()
{

	fullScreenRect = SDL_Rect({ 0, 0, app->render->camera.w, app->render->camera.h });

	screenTexture = app->tex->Load(screenTexturePath);
	if (screenTexture == nullptr)
		LOG("Couldn't load title screen");

	titleScreenAnim.PushBack({ 0,0,480,270});
	titleScreenAnim.PushBack({ 0,270, 480, 270 });

	gameOverAnim.PushBack({ 0,540,480,270 });
	gameOverAnim.PushBack({ 0,810,480,270 });

	logoAnim.PushBack({ 480,0,480,270 });
	logoAnim.PushBack({ 480,270,480,270 });

	turnOffAnim.PushBack({ 0,0,0,0, });

	app->audio->PlayMusic(musicPath);

	titleScreenAnim.loop = gameOverAnim.loop = logoAnim.loop = true;

	logoAnim.speed = 6.0f;
	titleScreenAnim.speed = 6.0f;
	gameOverAnim.speed = 1.8f;

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

	if (gameplayState == PLAYING)
	{
		//botones para el options
	/*	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		{
			app->guimanager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 204, 69, 73, 35 }), 1);
			app->guimanager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 197, 105, 87, 35 }), 2);
			app->guimanager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 177, 141, 127, 35 }), 3);
			app->guimanager->CreateGuiControl(GuiControlType::BUTTON, 0, 0, SDL_Rect({ 212, 177, 56, 35 }), 4);

		}*/

		//borrar temporal
		if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		{
			app->guimanager->DestroyGuiControl(app->guimanager->controls[0]);
			app->guimanager->DestroyGuiControl(app->guimanager->controls[0]);
			app->guimanager->DestroyGuiControl(app->guimanager->controls[0]);
			app->guimanager->DestroyGuiControl(app->guimanager->controls[0]);
		}

		//botones para el settings
		if (app->input->GetKey(SDL_SCANCODE_U) == KEY_DOWN)
		{
			app->guimanager->CreateGuiControl(GuiControlType::CHECKBOX, 0, 0, SDL_Rect({ 275, 166, 20, 20 }), 1);
			app->guimanager->CreateGuiControl(GuiControlType::CHECKBOX, 0, 0, SDL_Rect({ 238, 189, 20, 20 }), 2);
		}

		if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		{
			app->RequestLoad();
			LOG("LOAD REQUESTED");
		}

		if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
		{
			app->player->saveOnce1 = true;
			app->player->saveOnce2 = true;
			app->RequestSave();
			LOG("SAVE REQUESTED");
		}

		if (app->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN)
		{
			app->ui->currentLevel = 1;
			LoadLevel("level1.tmx");
		}

		if (app->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN)
		{
			app->ui->currentLevel = 2;
			LoadLevel("level2.tmx");
		}

		if (app->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN)
		{
			LoadLevel(currentLevel);
		}
	}

	// 8 to volume down and 9 to volume up
	if (app->input->GetKey(SDL_SCANCODE_8) == KEY_REPEAT)
	{
		app->audio->VolumeDown();
		LOG("Volume down");
	}

	if (app->input->GetKey(SDL_SCANCODE_9) == KEY_REPEAT)
	{
		app->audio->VolumeUp();
		LOG("Volume up");
	}

	if (gameplayState != targetState)
	{
		currentFade += 1.3f * dt;
		if (currentFade >= 1.0f)
		{
			currentFade = 1.0f;
			ChangeGameplayState(targetState);
		}
	}
	else if (currentFade > 0.0f)
	{
		currentFade -= 1.3f * dt;
	}
	else if (currentFade <= 0.0f)
	{
		currentFade = 0.0f;
		fading = false;
	}

	screenDisplayAnim->Update(dt);

	return true;
}

bool Scene::Load(pugi::xml_node& savedGame)
{
	FadeToNewState(PLAYING);
	LoadLevel(savedGame.attribute("currentLevel").as_string("level1.tmx"), false);

	return true;
}

bool Scene::Save(pugi::xml_node& savedGame)
{
	pugi::xml_attribute lvl = savedGame.append_attribute("currentLevel");
	lvl.set_value(currentLevel.GetString());

	return true;
}

void Scene::FadeToNewState(GameplayState newState)
{
	if (gameplayState == newState) return;
	if (fading) return;
	targetState = newState;
	currentFade = 0.0f;
	fading = true;
}

bool Scene::OnGuiMouseClickEvent(GuiControl* control)
{

	switch (control->type)
	{
		case GuiControlType::BUTTON:
			switch (control->id)
			{
				case 1:
					app->guimanager->DestroyAllGuiControls();
					app->guimanager->DestroyAllGuiControls();
					app->ui->uiToRender = 0;
					break;

				case 2:
					app->guimanager->DestroyAllGuiControls();
					app->guimanager->CreateGuiControl(GuiControlType::CHECKBOX, 0, 0, SDL_Rect({ 275, 166, 20, 20 }), 1);
					app->guimanager->CreateGuiControl(GuiControlType::CHECKBOX, 0, 0, SDL_Rect({ 238, 189, 20, 20 }), 2);
					app->ui->uiToRender = 3;
					break;

				case 3:
					//lo q se tenga q hacer antes de cambiar
					//gameplayState = TITLE_SCREEN;
					break;

				case 4:
					exit = true;
					break;

				default:
					break;
			}
			break;

		case GuiControlType::CHECKBOX:
			switch (control->id)
			{
				case 1:
					//music
					break;

				case 2:
					//fx
					break;

				case 3:
					//do fullscreen
					break;

				case 4:
					app->render->vSync = !app->render->vSync;
					break;

				default:
					break;
			}
			break;

		default: break;
	}

	return true;
}

void Scene::ChangeGameplayState(GameplayState newState)
{
	if (gameplayState == newState) return;

	switch (newState)
	{
		case PLAYING:
			screenDisplayAnim = &turnOffAnim;
			app->parallax->enabled = true;
			gameplayState = PLAYING;
			currentLevel.Create("level1.tmx");
			app->map->Load("level1.tmx");
			app->player->Reload();
			app->player->unlockedChekpoint1 = false;
			app->player->unlockedChekpoint2 = false;
			break;
		case TITLE_SCREEN:
			screenDisplayAnim = &titleScreenAnim;
			gameplayState = TITLE_SCREEN;
			app->parallax->enabled = false;
			app->map->CleanUp();
			app->render->camera.x = 0;
			app->render->camera.y = 0;
			app->ui->uiToRender = 0;
			break;
		case GAME_OVER_SCREEN:
			screenDisplayAnim = &gameOverAnim;
			gameplayState = GAME_OVER_SCREEN;
			app->parallax->enabled = false;
			app->map->CleanUp();
			app->render->camera.x = 0;
			app->render->camera.y = 0;
			app->ui->uiToRender = 0;
			break;
	}
}


void Scene::LoadLevel(SString name, bool loadEntities)
{
	app->player->unlockedChekpoint1 = false;
	app->player->unlockedChekpoint2 = false;
	app->player->isDead = false;
	currentLevel = name;
	app->map->Load(name.GetString(), loadEntities);
	app->player->Reload();
}

// Called each loop iteration
bool Scene::PostUpdate()
{
	OPTICK_EVENT("ScenePostUpdate", Optick::Category::Rendering);

	bool ret = true;

	if(exit)
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
