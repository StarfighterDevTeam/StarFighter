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
	sf::Vector2u spawnFood();

private:
	sf::Vector2f getCellPos(int x, int y);
	bool isPlayerCell(int x, int y);
	bool isCollidingWithSelf();
	void growSnake();

	sf::RenderWindow* m_window;

	std::vector<sf::Vector2u> playerPos;
	sf::Vector2u foodPos;
	Direction playerDirection;
	int tick;
	int score;
	float tickTimer;
	bool bPaused;
	sf::Font* m_font;
};

#endif // GAME_H_INCLUDED