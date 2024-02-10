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

struct State
{
public:
	bool bDangerStraight;
	bool bDangerLeft;
	bool bDangerRight;
	bool bDirectionLeft;
	bool bDirectionRight;
	bool bDirectionUp;
	bool bDirectionDown;
	bool bFoodLeft;
	bool bFoodRight;
	bool bFoodUp;
	bool bFoodDown;
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

	Direction getPlayerDirection()	{ return m_playerDirection; }
	sf::Vector2u spawnFood();
	void setPause(bool bPause)		{ m_bPaused = bPause; }
	bool isPaused()					{ return m_bPaused; }

private:
	sf::Vector2f getCellPos(int x, int y);
	sf::Vector2u getNextCell(Action action);
	bool isPlayerCell(int x, int y);
	bool isDangerCell(int x, int y);
	bool isCollidingWithSelf();
	void growSnake();
	State computeState();

	sf::RenderWindow* m_window;

	std::vector<sf::Vector2u> m_playerPos;
	sf::Vector2u m_foodPos;
	Direction m_playerDirection;
	int m_tick;
	int m_score;
	float m_tickTimer;
	bool m_bPaused;
	sf::Font* m_font;
	State m_state;
};

#endif // GAME_H_INCLUDED