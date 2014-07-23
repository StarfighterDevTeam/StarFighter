#ifndef BOT_H_INCLUDED
#define BOT_H_INCLUDED

#include "Independant.h"
#include "Weapon.h"

//forward declaration

class Independant;
class Ammo;
struct Weapon;

class Bot : public Independant
{
public:
	Bot::Bot(Independant* m_target, Weapon* m_weapon, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void update(sf::Time deltaTime) override;
	Weapon* weapon;
	int movepattern_type;
	Independant* target;
	
private:
		
};


#endif // BOT_H_INCLUDED