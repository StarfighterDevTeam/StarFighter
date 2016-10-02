#ifndef SESHIP_H_INCLUDED
#define SESHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "SEInputs.h"
#include "SEGame.h"
#include "SFTextPop.h"
#include "Enums.h"

#define SHIP_START_X                990
#define SHIP_START_Y                540
#define SHIP_ACCELERATION	        2000.0f
#define SHIP_DECCELERATION_COEF		5000.0f
#define SHIP_MAX_SPEED				400.0f
#define SHIP_MIN_SPEED				50.0f
#define SHIP_SPRITE_RATE_SEC        0.2f

class SEShip : public GameObject
{
public :
	SEShip();
	SEShip(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber = 1, int animationNumber = 1);
	SEShip(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size);
	void Init();
	virtual ~SEShip();
	virtual void update(sf::Time deltaTime) override;
	
	void ManageHudControls(sf::Vector2f inputs_directions);
	void ManageAcceleration(sf::Vector2f inputs_direction);
	void IdleDecelleration(sf::Time deltaTime);
	bool ScreenBorderContraints();
	void MaxSpeedConstraints();
	void UpdateRotation();

	bool m_disable_inputs;
	ControlerType m_controllerType;
	PlayerInputStates m_inputs_states[NBVAL_PlayerActions];
	bool m_actions_states[NBVAL_PlayerActions];

	void SetControllerType(ControlerType contoller);
	void GetInputState(bool input_guy_boolean, PlayerActions action);
	virtual void UpdateInputStates();
	bool UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition);
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);

	int Save();
	bool Load();

	sf::Clock m_stroboscopic_effect_clock;

	SFPanel* m_SFTargetPanel;
	SFPanelTypes m_is_asking_SFPanel;

protected:
	bool m_moving;
	bool m_movingX;
	bool m_movingY;
};

#endif // SESHIP_H_INCLUDED
