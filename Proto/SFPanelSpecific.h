#ifndef SFMENU_H_INCLUDED
#define SFMENU_H_INCLUDED

#include "SEGame.h"
#include "Enums.h"

class SFPanelSpecific : public SFPanel
{
public:
	SFPanelSpecific(sf::Vector2f size, SFPanelTypes panel_type, SEShip* playerShip);
	virtual ~SFPanelSpecific();
	virtual void Update(sf::Time deltaTime) override;
	virtual void Draw(sf::RenderTexture& screen) override;

	SFActionBoxWithSelection* m_actions_with_selection;
};

#endif // SFMENU_H_INCLUDED