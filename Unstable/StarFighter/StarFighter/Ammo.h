#ifndef AMMO_H_INCLUDED
#define AMMO_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "FX.h"

class Ammo : public GameObject
{
public :
	Ammo(GameObject* owner, float speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ammo(GameObject* owner, sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	~Ammo();
	void update(sf::Time deltaTime) override;

	GameObject* m_owner;
	GameObject* m_target;
	float m_ref_speed;
};

#endif // AMMO_H_INCLUDED
