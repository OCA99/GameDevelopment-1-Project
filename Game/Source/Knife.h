#ifndef __KNIFE_H__
#define __KNIFE_H__

#include "Point.h"
#include "Entity.h"
#include "Animation.h"
#include "SDL_image/include/SDL_image.h"

class Knife : Entity
{

public:

	Knife(Module* parent, fPoint position, SDL_Texture* texture, Type type);

	bool Start();

	bool Update(float dt);

	bool Draw();

	void Collision(Collider* other);

};

#endif // !__KNIFE_H__