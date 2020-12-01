#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

#include "Weapon.h"

//forward declaration

class GameObject;
class Ammo;
class Weapon;

class Bot : public GameObject
{
public:
	Bot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	~Bot();

	void update(sf::Time deltaTime, float hyperspeedMultiplier) override;
	void setGhost(bool ghost) override;
	Weapon* m_weapon;
	GameObject* m_target;
	Bot* Clone();
	float m_vspeed;
	void setTarget (GameObject* target);
	sf::Vector2f m_spread;
	bool m_automatic_fire;
	bool m_key_repeat;

	void Fire(sf::Time deltaTime, float hyperspeedMultiplier, bool firing);
};


#endif // BOT_H_INCLUDED