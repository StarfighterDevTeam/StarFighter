#ifndef INDEPENDANT_H_INCLUDED
#define INDEPENDANT_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "TextureLoader.h"
#include "AnimatedSprite.hpp"
#include "TextUtils.h"
#include <vector>
#define _USE_MATH_DEFINES
#include <math.h>

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

enum MovePatternType {
	NoMove,
	SemiCircle,
	NBVAL_MovePattern
};

class Independant : public AnimatedSprite
{
public:
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin);
	Independant(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	Independant(sf::Vector2f position, sf::Vector2f speed, sf::Texture *texture);
	~Independant();

	virtual void update(sf::Time deltaTime, sf::Clock polarClock);
	virtual void updateAnimation(sf::Time deltaTime);
	bool visible;
	bool isOnScene;
	IndependantType collider_type;
	void setVisible(bool visible);
	virtual void damage_from (Independant& independant);
	sf::Vector2f m_size;
	sf::Vector2f getIndependantSpeed();
	int getIndependantDamage();
	int getIndependantArmor();
	int getIndependantShield();
	int getIndependantShieldMax();
	int getIndependantShieldRegen();
	string getName();
	virtual void Respawn();

	sf::Vector2f setMovePattern(sf::Clock, float radius, float triggerY, int pattern_id=MovePatternType::NoMove);

	int damage;
	int armor;
	int shield;
	int shield_max;
	int shield_regen;
	sf::Vector2f speed;

protected:
	bool startPattern;
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