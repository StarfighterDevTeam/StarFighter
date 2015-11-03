#ifndef DISCOBALL_H_INCLUDED
#define DISCOBALL_H_INCLUDED

#include "Globals.h"
#include "Game.h"
#include "GameObject.h"
#include "Stroboscopic.h"
#include "LevelPortal.h"

enum DiscoballStatus
{
	Discoballm_carriedBlueTeam,
	Discoballm_carriedRedTeam,
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

	Ship* m_carrier;
	float m_carrier_curAngle;
	sf::Vector2f m_carrier_curPosition;
	bool m_carried;
	DiscoballStatus m_status;
	void SetDiscoballStatus(DiscoballStatus status);

	void DiscoballBumper(GameObject* bumper) override;
	void UsingPortal(bool is_using) override;
	void GetPortal(GameObject* portal) override;
	bool m_isTouchingBumper;
	bool m_isUsingPortal;

	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);
	sf::Clock stroboscopic_effect_clock;
};

#endif // DISCOBALL_H_INCLUDED
