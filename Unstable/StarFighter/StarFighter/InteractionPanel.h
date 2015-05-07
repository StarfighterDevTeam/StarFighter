#ifndef INTERACTIONPANEL_H_INCLUDED
#define INTERACTIONPANEL_H_INCLUDED

#include <SFML/Graphics.hpp>
#include "Independant.h"

struct InteractionPanel
{
public:
	InteractionPanel();
	void SetPanelPosition(sf::Vector2f position);

	void Update(int max_unlocked_hazard_level);
	void Draw(sf::RenderTexture& screen);

	sf::RectangleShape m_panel;
	sf::Text m_text[NB_HAZARD_LEVELS];
	sf::Text m_textHelp;
	sf::Text m_textDestination;
	sf::Font* m_font;
	Independant* m_arrow;
	int m_selected_index;
	Directions m_direction;
};

#endif //INTERACTIONPANEL_H_INCLUDED