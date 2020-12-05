#ifndef SFTRADE_H_INCLUDED
#define SFTRADE_H_INCLUDED

#include "SFHUDPanel.h"

class SFTradePanel : public SFInventoryPanel
{
public:
	SFTradePanel(sf::Vector2f size, Ship* playerShip);
	void Draw(sf::RenderTexture& screen) override;

	sf::Text m_title_text2;
	sf::RectangleShape m_separator;
};

#endif // SFTRADE_H_INCLUDED