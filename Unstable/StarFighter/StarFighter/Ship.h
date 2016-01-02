#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "Fluxor.h"
#include "Module.h"

#define SHIP_START_X                990
#define SHIP_START_Y                540
#define SHIP_ACCELERATION	        2000.0f
#define SHIP_DECCELERATION_COEF		5000.0f
#define SHIP_MAX_SPEED				400.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f

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
