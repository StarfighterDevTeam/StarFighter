#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

#include "Independant.h"
#include "Weapon.h"
#include "InputGuy.h"

//forward declaration

class Independant;
class Ammo;
struct Weapon;

class Bot : public Independant
{
public:
	Bot::Bot(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void update(sf::Time deltaTime) override;
	Weapon* weapon;
	Independant* target;
	Bot* Clone();
	float radius;
	float angspeed;
	float vspeed;
	bool hasTarget;
	bool hasWeapon;
	void setTarget (Independant* m_target);
	void setRadius (float m_radius, float clockwise);
	sf::Vector2f spread;

private:
		
};


#endif // BOT_H_INCLUDED