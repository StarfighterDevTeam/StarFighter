#ifndef SFUPGRADESPANEL_H_INCLUDED
#define SFUPGRADESPANEL_H_INCLUDED

#include "SFMenuPanel.h"

class SFUpgradesPanel : public SFPanel
{
public:
	SFUpgradesPanel(sf::Vector2f size, Ship* playership);
	void Update(sf::Time deltaTime, sf::Vector2f inputs_directions) override;
	void Draw(sf::RenderTexture& screen) override;

	bool BuyUpgrade() override;

	sf::RectangleShape m_upgrade_container[NB_UPGRADE_CHOICES];
	SFRectangle m_unavailable_mask[NB_UPGRADE_CHOICES];
	sf::Text m_upgrades[NB_UPGRADE_CHOICES];
	sf::Text m_upgrade_description[NB_UPGRADE_CHOICES];
	sf::Text m_upgrade_price[NB_UPGRADE_CHOICES];
	sf::RectangleShape m_upgrade_picture_container[NB_UPGRADE_CHOICES];
	GameObject* m_upgrade_picture[NB_UPGRADE_CHOICES];
	bool m_sold_out[NB_UPGRADE_CHOICES];
	SFText m_sold_out_text[NB_UPGRADE_CHOICES];
	sf::Text m_money_text;
	int m_selected_index;
	
};

#endif // SFUPGRADESPANEL_H_INCLUDED