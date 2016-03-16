#ifndef SFDIALOG_H_INCLUDED
#define SFDIALOG_H_INCLUDED

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

	Dialog* m_dialog;
	GameObject m_picture;
};

#endif // SFDIALOG_H_INCLUDED