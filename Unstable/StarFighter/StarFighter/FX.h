#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Game.h"

class FX : public GameObject
{
public:
	FX(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber = 1);
	void update(sf::Time deltaTime) override;

	FX* Clone();

private:
	sf::Clock m_deltaClockExploding;
	bool m_exploding;
	sf::Time m_duration;
};

class Aura : public FX
{
public:
	Aura(GameObject* target, std::string textureName, sf::Vector2f size, int frameNumber, int animationNumber = 1);
	void Init(std::string textureName, sf::Vector2f dsize, int frameNumber);
	void update(sf::Time deltaTime) override;
	Aura* Clone();

	GameObject* m_target;
	sf::Vector2f m_offset;
};

#endif // FX_H_INCLUDED