#ifndef DISCOBALL_H_INCLUDED
#define DISCOBALL_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"

enum DiscoballStatus
{
	DiscoballCarriedTeamBlue,
	DiscoballCarriedTeamRed,
	DiscoballLost,
	DiscoballFree,
};

class Ship;

class Discoball : public GameObject
{
public :
	Discoball();
	Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Discoball(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Discoball();
	void update(sf::Time deltaTime) override;

	float discoball_curAngularSpeed;
	float coeff_friction;

	Ship* carrier;
	float carrier_curAngle;
	sf::Vector2f carrier_curPosition;
	bool carried;
	DiscoballStatus m_status;
	void SetDiscoballStatus(DiscoballStatus status);
};

#endif // DISCOBALL_H_INCLUDED
