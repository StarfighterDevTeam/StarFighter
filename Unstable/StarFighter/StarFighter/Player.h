#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SFML/Graphics.hpp>
#include <string.h>
#include "Globals.h"

class Player
{

public:

	void Init(sf::RenderWindow* mainWindow);

	sf::RenderWindow* m_playerWindow;
	std::string m_save_file;

};

#endif //PLAYER_H_INCLUDED