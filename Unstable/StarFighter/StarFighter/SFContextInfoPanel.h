#ifndef SFCONTEXTINFOPANEL_H_INCLUDED
#define SFCONTEXTINFOPANEL_H_INCLUDED

#include "Game.h"
#include "Ship.h"

class SFContextInfoPanel : public SFPanel
{
public:
	SFContextInfoPanel(sf::Vector2f size, SFPanelTypes panel_type, Ship* playerShip);
	~SFContextInfoPanel();
	virtual void Update(sf::Time deltaTime);
	virtual void Draw(sf::RenderTexture& screen);
	GameObject* GetUnit() override;

	SFActionBoxWithSelection* m_actions_with_selection;
	GameObject* m_unit;
};

#endif // SFCONTEXTINFOPANEL_H_INCLUDED