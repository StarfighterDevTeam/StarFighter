#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "SFML//Graphics.hpp"

#include "Player.h"

class Game
{
public:
	Game();
	~Game();
	void init(sf::RenderWindow* pWindow);
	void update(sf::Time dt);
	void draw();
	void reset(const int nbPlayers);

	std::vector<Player> m_players;

private:
	sf::Font* m_font;
	sf::RenderWindow* m_window;

	int m_currentPlayer;
	int m_nbPlayers;
};

#endif // GAME_H_INCLUDED