#ifndef SFDIALOGPANEL_H_INCLUDED
#define SFDIALOGPANEL_H_INCLUDED

#include "Scene.h"
#include "Dialog.h"

class SFDialogPanel : public SFPanel
{
public:
	SFDialogPanel(sf::Vector2f size, Ship* playerShip);
	~SFDialogPanel();
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;
	Dialog* GetDialog() override;
	float GetDuration() override;
	float GetDurationClockElpased() override;

	Dialog* m_dialog;
	GameObject m_picture;
	sf::Vector2f m_target_position;
	bool m_is_downscreen;
	sf::Clock m_duration_clock;

	sf::Text m_new_action_texts;
	GameObject m_buttons;
};

#endif // SFDIALOGPANEL_H_INCLUDED