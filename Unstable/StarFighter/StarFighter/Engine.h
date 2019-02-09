#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED

#include "Game.h"
#include "Room.h"

class Engine : public GameEntity
{
public:
	Engine();

	GameEntity* m_lifebar;
	GameEntity* m_rofbar;
};

#endif //SHIP_H_INCLUDED