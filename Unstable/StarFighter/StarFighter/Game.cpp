#include "Game.h"
#include "Globals.h"
#include "assert.h"

#define GRID_NB_LINES	20
#define LEFT_MARGIN		150.f
#define UP_MARGIN		150.f
#define CELL_SIZE		20.f
#define GRID_THICKNESS	2.f
#define START_SIZE		3

#define GAME_SPEED		6.f

Game::Game()
{
}

Game::~Game()
{
	delete m_font;
}

void Game::init(sf::RenderWindow* pWindow)
{
	m_window = pWindow;

	m_font = new sf::Font();
	if (!m_font->loadFromFile("Fonts/arial.ttf")) {}
	bPaused = false;

	reset();
}

void Game::reset()
{
	playerPos.clear();
	playerPos.push_back(sf::Vector2u(START_SIZE - 1, 10));
	playerDirection = Direction::RIGHT;
	tick = 0;
	tickTimer = 0.f;
	score = 0;

	for (int i = 0; i < START_SIZE - 1; i++)
		growSnake();

	spawnFood();
}

sf::Vector2f Game::getCellPos(int x, int y)
{
	assert(x >= 0 && x < GRID_NB_LINES && y >= 0 && y < GRID_NB_LINES);
	return sf::Vector2f(LEFT_MARGIN + 1.f * x * CELL_SIZE, UP_MARGIN + 1.f * y * CELL_SIZE);
}

bool Game::isPlayerCell(int x, int y)
{
	assert(x >= 0 && x < GRID_NB_LINES && y >= 0 && y < GRID_NB_LINES);
	for (sf::Vector2u pos : playerPos)
	{
		if (pos.x == x && pos.y == y)
		{
			return true;
		}	
	}

	return false;
}

bool Game::isCollidingWithSelf()
{
	int size = (int)playerPos.size();

	for (int i = 0; i < size; i++)
	{
		if (i == 0)
			continue;

		if (playerPos[0] == playerPos[i])
			return true;
	}

	return false;
}

sf::Vector2u Game::spawnFood()
{
	bool bFound = false;
	int x, y = 0;
	while (!bFound)
	{
		x = RandomizeIntBetweenValues(0, GRID_NB_LINES - 1);
		y = RandomizeIntBetweenValues(0, GRID_NB_LINES - 1);

		if (!isPlayerCell(x, y))
			bFound = true;
	}

	foodPos = sf::Vector2u(x, y);
	return foodPos;
}

void Game::growSnake()
{
	int size = (int)playerPos.size();
	if (size == 1)
	{
		sf::Vector2u playerPosFront = playerPos[0];
		switch (playerDirection)
		{
			case Direction::LEFT:
			{
				playerPos.push_back(sf::Vector2u(playerPosFront.x + 1, playerPosFront.y));
				break;
			}
			case Direction::RIGHT:
			{
				playerPos.push_back(sf::Vector2u(playerPosFront.x - 1, playerPosFront.y));
				break;
			}
			case Direction::UP:
			{
				playerPos.push_back(sf::Vector2u(playerPosFront.x, playerPosFront.y + 1));
				break;
			}
			case Direction::DOWN:
			{
				playerPos.push_back(sf::Vector2u(playerPosFront.x, playerPosFront.y - 1));
				break;
			}
		}
	}
	else
	{
		sf::Vector2u deltaPos = playerPos[size - 1] - playerPos[size - 2];
		sf::Vector2u tailPos = playerPos[size - 1] - deltaPos;
		playerPos.push_back(tailPos);
	}
}

void Game::update(sf::Time dt, Action action)
{
	tickTimer += dt.asSeconds();
	const float frameDuration = 1.f / GAME_SPEED;

	static Action actionApplied = Action::STRAIGHT;//store action if it's a turn and apply it once per tick
	if (action != Action::STRAIGHT)
	{
		actionApplied = action;
		printf("action: %d\n", (int)action);
	}

	if (tickTimer > 1.f / GAME_SPEED)
	{
		printf("tick: %d\n", tick);

		tickTimer = 0.f;
		tick++;

		//inputs
		switch (actionApplied)
		{
			case Action::STRAIGHT:
			{
				break;
			}
			case Action::TURN_LEFT:
			{
				playerDirection = (int)playerDirection > 0 ? (Direction)((int)playerDirection - 1) : (Direction)3;
				break;
			}
			case Action::TURN_RIGHT:
			{
				playerDirection = (int)playerDirection < 3 ? (Direction)((int)playerDirection + 1) : (Direction)0;
				break;
			}
		}

		//move
		std::vector<sf::Vector2u> oldPlayerPos = playerPos;
		switch (playerDirection)//head
		{
			case Direction::LEFT:
			{
				playerPos[0].x--;
				break;
			}
			case Direction::RIGHT:
			{
				playerPos[0].x++;
				break;
			}
			case Direction::UP:
			{
				playerPos[0].y--;
				break;
			}
			case Direction::DOWN:
			{
				playerPos[0].y++;
				break;
			}
		}

		int size = (int)playerPos.size();//body
		for (int i = 1; i < size; i++)
		{
			playerPos[i] = oldPlayerPos[i - 1];
		}

		//eat food
		if (playerPos[0] == foodPos)
		{
			score++;
			growSnake();
			spawnFood();
		}

		//collision (= game over)
		const bool bCollisionWithWall = playerPos[0].x < 0 || playerPos[0].x >= GRID_NB_LINES || playerPos[0].y < 0 || playerPos[0].y >= GRID_NB_LINES;
		const bool bCollisionWithOwnBody = bCollisionWithWall ? false : isCollidingWithSelf();
		if (bCollisionWithWall || bCollisionWithOwnBody)
		{
			if (bCollisionWithWall)
				printf("--- GAME OVER : Collision with Wall --- Score: %d, Ticks: %d\n", score, tick);
			else if (bCollisionWithOwnBody)
				printf("--- GAME OVER : Collision with Body --- Score: %d, Ticks: %d\n", score, tick);

			reset();
		}

		//reset action for next tick
		actionApplied = Action::STRAIGHT;
	}
}

void Game::draw()
{
	//m_window->draw();

	//grid
	for (int i = 0; i < GRID_NB_LINES; i++)
	{
		for (int j = 0; j < GRID_NB_LINES; j++)
		{
			sf::RectangleShape cell(sf::Vector2f(CELL_SIZE, CELL_SIZE));
			cell.setFillColor(sf::Color(0, 0, 0, 255));
			cell.setOutlineColor(sf::Color(255, 255, 255, 255));
			cell.setOutlineThickness(2.f);
			cell.setPosition(LEFT_MARGIN + CELL_SIZE * i, UP_MARGIN + CELL_SIZE * j);
			m_window->draw(cell);
		}
	}

	//player
	int snakeSize = (int)playerPos.size();
	for (int i = 0; i < snakeSize; i++)
	{
		sf::RectangleShape payerCell(sf::Vector2f(CELL_SIZE - GRID_THICKNESS, CELL_SIZE - GRID_THICKNESS));
		payerCell.setFillColor(sf::Color(0, 255, 0, 255));
		payerCell.setOutlineColor(sf::Color(255, 255, 255, 0));
		payerCell.setPosition(getCellPos(playerPos[i].x, playerPos[i].y));
		m_window->draw(payerCell);
	}

	//food
	sf::RectangleShape foodCell(sf::Vector2f(CELL_SIZE - GRID_THICKNESS, CELL_SIZE - GRID_THICKNESS));
	foodCell.setFillColor(sf::Color(255, 0, 0, 255));
	foodCell.setOutlineColor(sf::Color(255, 255, 255, 0));
	foodCell.setPosition(getCellPos(foodPos.x, foodPos.y));
	m_window->draw(foodCell);
}