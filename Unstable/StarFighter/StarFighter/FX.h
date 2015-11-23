#ifndef FX_H_INCLUDED
#define FX_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "GameObject.h"
#include "Game.h"

class Aura : public GameObject
{
public:
	Aura(GameObject* target, std::string textureName, sf::Vector2f size, int frameNumber = 1, int animationNumber = 1);
	void update(sf::Time deltaTime) override;
	GameObject* m_target;
};

#endif // FX_H_INCLUDED