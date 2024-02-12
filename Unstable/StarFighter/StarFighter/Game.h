#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include "SFML//Graphics.hpp"

#define GRID_NB_LINES				20
#define LEFT_MARGIN					150.f
#define UP_MARGIN					150.f
#define CELL_SIZE					20.f
#define GRID_THICKNESS				2.f
#define START_SIZE					3
#define TIMEOUT_TICK_RATIO			50

#define GAME_SPEED_TRAINING			10000.f//300.f
#define GAME_SPEED_FAST				50.f//300.f
#define GAME_SPEED_NORMAL			10.f

#define HUMAN_PLAYER_ONLY			false
#define WRITE_DNA					false

class Individual;

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

enum class Death : int
{
	WALL,
	OWN,
	TIMEOUT
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
	void update(sf::Time dt, Action action, bool bRealTime, bool bWritingDNA, int& score, Death& death, bool& bGameOver);//returns score on game over
	void draw();
	void reset();

	Direction getPlayerDirection()	{ return m_playerDirection; }
	sf::Vector2u spawnFood();
	void setPause(bool bPause)		{ m_bPaused = bPause; }
	bool isPaused()	const			{ return m_bPaused; }
	State getState() const			{ return m_state; }
	int getStateId() const;
	Action getAction(const int stateId, const Individual& individual);

private:
	sf::Vector2f getCellPos(int x, int y);
	sf::Vector2u getNextCell(Action action);
	bool isPlayerCell(int x, int y);
	bool isDangerCell(int x, int y);
	bool isCollidingWithSelf();
	void growSnake();
	State computeState();
	int computeScore();

	sf::RenderWindow* m_window;

	std::vector<sf::Vector2u> m_playerPos;
	sf::Vector2u m_foodPos;
	Direction m_playerDirection;
	int m_tick;
	int m_tickWithoutEating;
	int m_score;
	float m_tickTimer;
	bool m_bPaused;
	sf::Font* m_font;
	State m_state;
};

#endif // GAME_H_INCLUDED