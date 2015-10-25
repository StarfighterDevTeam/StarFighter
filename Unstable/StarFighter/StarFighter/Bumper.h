#ifndef BUMPER_H_INCLUDED
#define BUMPER_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum BumperType
{
	OnlyBlueTeamThrough,
	OnlyRedTeamThrough,
	OnlyPlayersThrough,
};

class Bumper : public GameObject
{
public :
	Bumper();
	Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Bumper(BumperType type, sf::Vector2f position, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Bumper();
	void update(sf::Time deltaTime) override;

	BumperType m_type;
};

#endif // BUMPER_H_INCLUDED
