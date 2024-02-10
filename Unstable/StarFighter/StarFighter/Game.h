#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "SFML//Graphics.hpp"

enum class Direction : int
{
	LEFT,
	UP,
	RIGHT,
	DOWN
};

enum class Action : int
{
	STRAIGHT,
	TURN_LEFT,
	TURN_RIGHT
};

class Game
{
public:
	Game();
	~Game();
	void init(sf::RenderWindow* pWindow);
	void update(sf::Time dt, Action action);
	void draw();
	void reset();

	Direction getPlayerDirection()	{ return playerDirection; }

private:
	sf::Vector2f getCellPos(int x, int y);

	sf::RenderWindow* m_window;

	sf::Vector2u playerPos;
	Direction playerDirection;
	int tick;
	float tickTimer;
	sf::Font* m_font;
};

#endif // GAME_H_INCLUDED