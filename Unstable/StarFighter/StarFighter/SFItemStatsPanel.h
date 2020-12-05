#ifndef SFITEMSTATSPANEL_H_INCLUDED
#define SFITEMSTATSPANEL_H_INCLUDED

#include "SFMapPanel.h"

class SFItemStatsPanel : public SFPanel
{
public:
	SFItemStatsPanel(GameObject* object, sf::Vector2f size, Ship* playerShip, FocusedItemStates item_state, GameObject* comparison_object);
	~SFItemStatsPanel();
	void DisplayItemStats(GameObject* object);
	void Draw(sf::RenderTexture& screen) override;

	int m_selected_option_index;
	GameObject* m_comparison_object;
	sf::RectangleShape m_overblock;
	sf::Text m_title_text_comparison;
};

#endif // SFITEMSTATSPANEL_H_INCLUDED