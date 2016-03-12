#ifndef SFPANEL_H_INCLUDED
#define SFPANEL_H_INCLUDED

#include "HudGrid.h"

class Ship;

enum SFPanelTypes
{
	SFPanel_None,
	SFPanel_Inventory,
	SFPanel_DoubleInventory,
	SFPanel_Portal,
};

class SFPanel : public sf::RectangleShape
{
public:
	SFPanel(sf::Vector2f size, SFPanelTypes panel_type);
	~SFPanel();
	virtual void Update(sf::Time deltaTime, sf::Vector2f inputs_directions);
	virtual void Draw(sf::RenderTexture& screen);
	
	sf::Text m_title_text;
	sf::Text m_text;
	sf::Text m_actions_text;
	bool m_visible;
	SFPanelTypes m_panel_type;

	Ship* m_playerShip;

	virtual void SetSelectedOptionIndex(int index);
	virtual int GetSelectedOptionIndex();
};

#endif // SFPANEL_H_INCLUDED