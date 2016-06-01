#ifndef SFMENU_H_INCLUDED
#define SFMENU_H_INCLUDED

#include "Game.h"

class SFPanelSpecific : public SFPanel
{
public:
	SFPanelSpecific(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip);
	~SFPanelSpecific();
	virtual void Update(sf::Time deltaTime);
	virtual void Draw(sf::RenderTexture& screen);

	SFActionBoxWithSelection* m_actions_with_selection;
};

#endif // SFMENU_H_INCLUDED