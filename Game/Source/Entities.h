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

private:
	List<Entity*> entityList;

	// Textures
	SDL_Texture* batTexture;
	SDL_Texture* fruitTexture;
};

#endif // __ENTITIES_H__