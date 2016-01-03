#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "Fluxor.h"
#include "Module.h"

class Ship : public GameObject
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~Ship();
	void update(sf::Time deltaTime) override;
	
	void ManageHudControls(sf::Vector2f inputs_directions);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void IdleDecelleration(sf::Time deltaTime);
	void ScreenBorderContraints();
	void MaxSpeedConstraints();
	void UpdateRotation();

	bool m_disable_inputs;
	ControlerType m_controllerType;
	void SetControllerType(ControlerType contoller);
	bool m_CtrlKey_released;
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);
	sf::Clock stroboscopic_effect_clock;

	//FLUX SPECIFIC
	unsigned int m_flux;
	unsigned int m_flux_max;
	void GetFluxor(GameObject* object) override;
	void GetModule(GameObject* object) override;
	sf::Vector2u m_curGridIndex;
	sf::Clock m_flux_transfer_limiter_clock;
	unsigned int m_upgrade_level;
	float m_speed_max;
	void UpdatePlayerStats();

	//HUD
	sf::Text m_flux_text;

	//construction
	//void ResolveProductionBufferList() override;
	vector<Module*> m_construction_buffer;

private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
