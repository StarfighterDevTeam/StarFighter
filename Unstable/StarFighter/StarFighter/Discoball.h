#ifndef DISCOBALL_H_INCLUDED
#define DISCOBALL_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

#define	DISCOBALL_GRAVITATION_DISTANCE					70.f
#define	DISCOBALL_BASE_ANGULAR_SPEED					M_PI
#define	CARRY_TIME_FOR_ACCELERATION						1
#define	CARRY_MAX_TIME_FOR_ACCELERATION					5
#define	CARRY_ANGULAR_ACCELERATION						1.f
#define	CARRY_THROW_ACCELERATION_BONUS					6.f
#define	CARRY_CATCH_ACCELERATION_MALUS					8.f


class Discoball : public GameObject
{
public :
	Discoball();
	Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Discoball();
	void update(sf::Time deltaTime) override;

	float cartesian_speed;
	float polar_angle;

	float carrier_curAngle;
	sf::Vector2f carrier_curPosition;

	bool carried;
};

#endif // DISCOBALL_H_INCLUDED
