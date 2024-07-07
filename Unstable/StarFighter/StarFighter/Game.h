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
#define USE_BEHARIOR_AI				true

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
	bool bWallSeenStraight;
	bool bWallSeenLeft;
	bool bWallSeenRight;
};

class Game
{
public:
	Game();
	~Game();
	void init(sf::RenderWindow* pWindow);
	void update(sf::Time dt);
	void draw();
	void reset();

private:
	sf::Font* m_font;
	sf::RenderWindow* m_window;
};

#endif // GAME_H_INCLUDED