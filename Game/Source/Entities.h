#ifndef __ENTIITES_H__
#define __ENTITIES_H__

#include "Module.h"
#include "List.h"
#include "Point.h"
#include "Entity.h"

class Entities : public Module
{
public:

	Entities();

	// Destructor
	virtual ~Entities()
	{}

	// Called before render is available
	bool Awake(pugi::xml_node&);

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();

	void AddEntity(fPoint position, Entity::Type type);

	void OnCollision(Collider* a, Collider* b, float dt);

	void ResetEntities();

	bool Load(pugi::xml_node& savedGame);
	bool Save(pugi::xml_node& savedGame);

private:
	int enemySpeed = 0;
	int pigHealth = 0;
	int enemyGravity = 0;
	int enemyJumpForce = 0;
	int knifeSpeed = 0;

	List<Entity*> entityList;

	// Textures
	SDL_Texture* batTexture;
	SDL_Texture* fruitTexture;
	SDL_Texture* heartTexture;
	SDL_Texture* pigTexture;
	SDL_Texture* knifetexture;

	const char* batTexturePath;
	const char* fruitTexturePath;
	const char* heartTexturePath;
	const char* pigTexturePath;
	const char* knifeTexturePath;
};

#endif // __ENTITIES_H__