#include "Resources.h"

Resources::Resources()
{
	m_font = new sf::Font();
	if (!m_font->loadFromFile("Assets/Fonts/arial.ttf"))
		printf("ERROR LOADING FONT\n");

	m_view.setCenter(sf::Vector2f(WINDOW_RESOLUTION_X / 2, WINDOW_RESOLUTION_Y / 2));
	m_view.setSize(sf::Vector2f(WINDOW_RESOLUTION_X, WINDOW_RESOLUTION_Y));
}

Resources::~Resources()
{
	delete m_font;
}