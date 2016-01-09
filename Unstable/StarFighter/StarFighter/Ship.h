#ifndef SHIP_H_INCLUDED
#define SHIP_H_INCLUDED

#include "Globals.h"
#include "GameObject.h"
#include "InputGuy.h"
#include "Game.h"
#include "Fluxor.h"
#include "Module.h"
#include "SFTextPop.h"

enum PlayerConstructionFeedbacks
{
	Player_NotOverConstruction,
	Player_OverConstruction,
	Player_ConstructionInProgress,
	Player_NoRessourcesToBuild,
};

class Ship : public GameObject
{
public :
	Ship();
	Ship(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, sf::Vector2f origin, PlayerTeams team, int frameNumber = 1, int animationNumber = 1);
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
	bool m_SwitchKey_released;
	bool m_BuildKey_released;
	void PlayStroboscopicEffect(Time effect_duration, sf::Time time_between_poses);
	sf::Clock stroboscopic_effect_clock;

	//FLUX SPECIFIC
	unsigned int m_flux;
	unsigned int m_flux_max;
	unsigned int m_flux_autogeneration;
	void GetFluxor(GameObject* object) override;
	void GetModule(GameObject* object) override;
	bool TryBuildModule(int module_key);
	sf::Vector2u m_curGridIndex;
	float m_flux_transfer_time;
	sf::Clock m_flux_transfer_limiter_clock;
	void FluxAutogeneration();
	float m_flux_autogeneration_time;
	sf::Clock m_flux_autogeneration_clock;
	unsigned int m_upgrade_level;
	float m_speed_max;
	void UpdatePlayerStats();

	//HUD
	SFText* m_flux_text;
	SFText* m_build_text;
	PlayerConstructionFeedbacks m_build_text_status;
	SFGauge* m_flux_gauge;
	void SetTeam(PlayerTeams team, TeamAlliances alliance);

	//construction
	//void ResolveProductionBufferList() override;
	vector<Module*> m_construction_buffer;

private:
	bool moving;
	bool movingX;
	bool movingY;
};

#endif // SHIP_H_INCLUDED
