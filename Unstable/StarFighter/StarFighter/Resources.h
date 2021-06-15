#ifndef RESOURCES_H_INCLUDED
#define RESOURCES_H_INCLUDED

#include <SFML/Graphics.hpp>

#define WINDOW_RESOLUTION_X		1534
#define WINDOW_RESOLUTION_Y		864

class Resources
{
public:
	Resources();
	~Resources();
	sf::Font* m_font;
	sf::View m_view;
};

#endif // RESOURCES_H_INCLUDED