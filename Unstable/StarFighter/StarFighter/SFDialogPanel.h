#ifndef SFDIALOGPANEL_H_INCLUDED
#define SFDIALOGPANEL_H_INCLUDED

#include "SFTradePanel.h"

class SFDialogPanel : public SFPanel
{
public:
	SFDialogPanel(sf::Vector2f size, Ship* playerShip);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	Dialog* GetDialog() override;
	float GetDuration() override;
	float GetDurationTimer() override;

	Dialog* m_dialog;
	GameObject m_picture;
	sf::Vector2f m_target_position;
	bool m_is_downscreen;
	float m_durationTimer;
};

#endif // SFDIALOGPANEL_H_INCLUDED