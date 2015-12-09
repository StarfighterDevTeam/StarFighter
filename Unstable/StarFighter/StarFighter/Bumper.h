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
	Bumper(BumperType type, sf::Vector2f position, sf::Vector2f size);
	virtual ~Bumper();
	void update(sf::Time deltaTime) override;
	void CollisionResponse(Time deltaTime) override;

	BumperType m_type;
	Glow* m_glow_effect;
	int m_stroke_size;
};

#endif // BUMPER_H_INCLUDED
