#ifndef FLUXENTITY_H_INCLUDED
#define FLUXENTITY_H_INCLUDED

#include "Game.h"
#include "SFTextPop.h"

class FluxEntity : public GameObject
{
public:
	FluxEntity();
	FluxEntity(sf::Vector2f position, std::string textureName, sf::Vector2f size, sf::Vector2f origin, int frameNumber, int animationNumber);
	~FluxEntity();
	void FluxInitialization();

	unsigned int m_flux;
	unsigned int m_flux_max;

	bool m_displaying_flux;

	sf::Vector2u m_curGridIndex;

	//flux entity properties
	bool m_wasting_flux;
	int m_flux_waste;
	float m_flux_waste_delay;
	sf::Clock m_flux_waste_clock;
	void WastingFlux();
	bool m_isAutogeneratingFlux;
	virtual bool AutogenerateFlux();
	float m_flux_autogeneration_time;
	sf::Clock m_flux_autogeneration_clock;

	unsigned int m_transfer_buffer;
	unsigned int m_transfert_buffer_memory;
	sf::Clock m_flux_transfer_clock;

	//team & alliance
	virtual void SetTeam(PlayerTeams team, TeamAlliances alliance);

	//HUD
	SFText* m_flux_text;
	SFGauge* m_flux_gauge;
	void AddFluxGauge(GaugeStyles gauge, sf::Vector2f offset);
	GameObject* m_warning;
	sf::Clock m_warning_clock;
	void UpdateWarningFeedback();
	void CreateWarningFeedback(sf::Vector2f position);
};

#endif // FLUXENTITY_H_INCLUDED