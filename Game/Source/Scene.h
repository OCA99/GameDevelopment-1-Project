#ifndef __SCENE_H__
#define __SCENE_H__

#include "Module.h"
#include "Animation.h"

struct SDL_Texture;
struct Animation;

enum GameplayState
{
	TITLE_SCREEN,
	PLAYING,
	GAME_OVER_SCREEN
};

class Scene : public Module
{
public:

	Scene();

	// Destructor
	virtual ~Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void LoadLevel(SString name);

public:
	bool gameStarted = false;
	bool gameOver = false;
	SString currentLevel;
private:
	SDL_Texture* screenTexture = nullptr;

	Animation* screenDisplayAnim;
	Animation titleScreenAnim;
	Animation gameOverAnim;
	Animation turnOffAnim;

	GameplayState gameplayState = TITLE_SCREEN;


};

#endif // __SCENE_H__