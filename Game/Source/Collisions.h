#ifndef __COLLISIONS_H__
#define __COLLISIONS_H__

#include "Module.h"
#include "Collider.h"
#include "List.h"

class Collisions : public Module
{
public:
	Collisions();

	// Destructor
	virtual ~Collisions();

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

	// Adds a new collider to the list
	Collider* AddCollider(SDL_Rect rect, Collider::Type type, Module* listener = nullptr);
	void DrawCollider(const SDL_Rect* section);

	// Removes the collider memory and removes it from the colliders array
	void RemoveCollider(Collider* collider);

private:
	// All existing colliders in the scene
	List<Collider*> staticColliders;
	List<Collider*> dynamicColliders;
};

#endif // __COLLISIONS_H__