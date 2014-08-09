#ifndef GAMESTATE_H_INCLUDED
#define GAMESTATE_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Player.h"

class GameManager;

class GameState
{

public:

	virtual void Initialize(Player player) = 0;

	virtual void Update(sf::Time deltatime) = 0;

	virtual void Draw() = 0;

	virtual void Release() = 0;

	GameManager* GameMgr;

};

#endif // GAMESTATE_H_INCLUDED