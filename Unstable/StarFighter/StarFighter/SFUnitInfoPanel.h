#ifndef SFUNITINFOPANEL_H_INCLUDED
#define SFUNITINFOPANEL_H_INCLUDED

#include "Game.h"
#include "Player.h"

class SFUnitInfoPanel : public SFPanel
{
public:
	SFUnitInfoPanel(sf::Vector2f size, SFPanelTypes panel_type, Player* player);
	~SFUnitInfoPanel();
	virtual void Update(sf::Time deltaTime);
	virtual void Draw(sf::RenderTexture& screen);
	GameObject* GetUnit() override;

	SFActionBoxWithSelection* m_actions_with_selection;
	GameObject* m_unit;
};

#endif // SFUNITINFOPANEL_H_INCLUDED