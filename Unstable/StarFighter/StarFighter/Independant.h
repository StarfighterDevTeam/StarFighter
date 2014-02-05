#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"

#define ENEMYX_FILENAME				"Assets/2D/enemyX.png"

enum IndependantType {
	Background,
	LootObject,
	PlayerShip,
	FriendlyFire,
	Neutral,
	EnemyFire,
	EnemyObject,
	NBVAL_Independant
};

class Independant : public AnimatedSprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	~Independant();

	virtual void update(sf::Time deltaTime);
	virtual void updateAnimation(sf::Time deltaTime);
	bool visible;
	bool isOnScene;
	IndependantType collider_type;
	void setVisible(bool visible);
	bool collide_with (const Independant& independant);
	virtual void damage_from (Independant& independant);
	sf::Vector2f m_size;
	sf::Vector2f getIndependantSpeed();
	int getIndependantDamage();
	int getIndependantArmor();
	int getIndependantShield();
	virtual void Respawn();

protected:
	sf::Vector2f speed;
	int damage;
	int armor;
	int shield;
	int shield_regen;
	std::string textureName;
	Animation defaultAnimation;
	Animation* currentAnimation;
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	void Init(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture, int frameNumber);
	void Init(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, int frameNumber = 1);
	
	bool immune;
	sf::Clock immunityTimer;
	

};



#endif // INDEPENDANT_H_INCLUDED