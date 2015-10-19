#ifndef BUMPER_H_INCLUDED
#define BUMPER_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

class Bumper : public GameObject
{
public :
	Bumper(float angle = 0.f);
	Bumper(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, float angle = 0.f, int frameNumber = 1, int animationNumber = 1);
	Bumper(sf::Vector2f position, std::string textureName, sf::Vector2f size, float angle = 0.f);
	void Init();
	virtual ~Bumper();
	void update(sf::Time deltaTime) override;

	float m_reflection_angle;
};

#endif // BUMPER_H_INCLUDED
