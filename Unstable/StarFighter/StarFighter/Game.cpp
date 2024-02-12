#include "Game.h"
#include "Globals.h"
#include "Individual.h"
#include "assert.h"

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
	if (!m_font->loadFromFile("Assets/Fonts/arial.ttf")) {}
	m_bPaused = false;

	reset();
}

void Game::reset()
{
	m_playerPos.clear();
	m_playerPos.push_back(sf::Vector2u(START_SIZE - 1, 10));
	m_playerDirection = Direction::RIGHT;
	m_tick = 0;
	m_tickTimer = 0.f;
	m_tickWithoutEating = 0;
	m_score = 0;

	for (int i = 0; i < START_SIZE - 1; i++)
		growSnake();

	spawnFood();
	m_state = computeState();
}

sf::Vector2f Game::getCellPos(int x, int y)
{
	assert(x >= 0 && x < GRID_NB_LINES && y >= 0 && y < GRID_NB_LINES);
	return sf::Vector2f(LEFT_MARGIN + 1.f * x * CELL_SIZE, UP_MARGIN + 1.f * y * CELL_SIZE);
}

bool Game::isPlayerCell(int x, int y)
{
	assert(x >= 0 && x < GRID_NB_LINES && y >= 0 && y < GRID_NB_LINES);
	for (sf::Vector2u pos : m_playerPos)
	{
		if (pos.x == x && pos.y == y)
		{
			return true;
		}	
	}

	return false;
}

bool Game::isDangerCell(int x, int y)
{
	return x < 0 || x > GRID_NB_LINES - 1 || y < 0 || y > GRID_NB_LINES - 1 || isPlayerCell(x, y);
}

bool Game::isCollidingWithSelf()
{
	int size = (int)m_playerPos.size();

	for (int i = 0; i < size; i++)
	{
		if (i == 0)
			continue;

		if (m_playerPos[0] == m_playerPos[i])
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

	m_foodPos = sf::Vector2u(x, y);
	return m_foodPos;
}

void Game::growSnake()
{
	int size = (int)m_playerPos.size();
	if (size == 1)
	{
		sf::Vector2u playerPosFront = m_playerPos[0];
		switch (m_playerDirection)
		{
			case Direction::LEFT:
			{
				m_playerPos.push_back(sf::Vector2u(playerPosFront.x + 1, playerPosFront.y));
				break;
			}
			case Direction::RIGHT:
			{
				m_playerPos.push_back(sf::Vector2u(playerPosFront.x - 1, playerPosFront.y));
				break;
			}
			case Direction::UP:
			{
				m_playerPos.push_back(sf::Vector2u(playerPosFront.x, playerPosFront.y + 1));
				break;
			}
			case Direction::DOWN:
			{
				m_playerPos.push_back(sf::Vector2u(playerPosFront.x, playerPosFront.y - 1));
				break;
			}
		}
	}
	else
	{
		sf::Vector2u deltaPos = m_playerPos[size - 1] - m_playerPos[size - 2];
		sf::Vector2u tailPos = m_playerPos[size - 1] - deltaPos;
		m_playerPos.push_back(tailPos);
	}
}

sf::Vector2u Game::getNextCell(Action action)
{
	sf::Vector2u nextPos = m_playerPos[0];

	Direction directionApplied = m_playerDirection;
	switch (action)
	{
		case Action::STRAIGHT:
		{
			break;
		}
		case Action::TURN_LEFT:
		{
			directionApplied = (int)m_playerDirection > 0 ? (Direction)((int)m_playerDirection - 1) : (Direction)3;
			break;
		}
		case Action::TURN_RIGHT:
		{
			directionApplied = (int)m_playerDirection < 3 ? (Direction)((int)m_playerDirection + 1) : (Direction)0;
			break;
		}
	}

	switch (directionApplied)
	{
		case Direction::LEFT:
		{
			nextPos.x--;
			break;
		}
		case Direction::RIGHT:
		{
			nextPos.x++;
			break;
		}
		case Direction::UP:
		{
			nextPos.y--;
			break;
		}
		case Direction::DOWN:
		{
			nextPos.y++;
			break;
		}
	}

	return nextPos;
}

void Game::update(sf::Time dt, Action action, bool bRealTime, bool bWritingDNA, int& score, Death& death, bool& bGameOver)
{
	bGameOver = false;
	m_tickTimer += dt.asSeconds();
	const float frameDuration = 1.f / (bRealTime ? (bWritingDNA ? GAME_SPEED_NORMAL : GAME_SPEED_FAST) : GAME_SPEED_TRAINING);

	static Action actionApplied = Action::STRAIGHT;//store action if it's a turn and apply it once per tick
	if (action != Action::STRAIGHT)
	{
		actionApplied = action;
		//printf("action: %d\n", (int)action);
	}

	if (m_tickTimer > frameDuration)
	{
		m_tickTimer = 0.f;
		m_tick++;

		//inputs
		switch (actionApplied)
		{
			case Action::STRAIGHT:
			{
				break;
			}
			case Action::TURN_LEFT:
			{
				m_playerDirection = (int)m_playerDirection > 0 ? (Direction)((int)m_playerDirection - 1) : (Direction)3;
				break;
			}
			case Action::TURN_RIGHT:
			{
				m_playerDirection = (int)m_playerDirection < 3 ? (Direction)((int)m_playerDirection + 1) : (Direction)0;
				break;
			}
		}

		//move
		std::vector<sf::Vector2u> oldPlayerPos = m_playerPos;
		{
			//head
			m_playerPos[0] = getNextCell(Action::STRAIGHT);

			//body
			int size = (int)m_playerPos.size();
			for (int i = 1; i < size; i++)
				m_playerPos[i] = oldPlayerPos[i - 1];
		}

		//eat food
		if (m_playerPos[0] == m_foodPos)
		{
			m_score++;
			growSnake();
			spawnFood();
			m_tickWithoutEating = 0;
		}
		else
			m_tickWithoutEating++;

		//game over conditions
		const bool bCollisionWithWall = m_playerPos[0].x < 0 || m_playerPos[0].x >= GRID_NB_LINES || m_playerPos[0].y < 0 || m_playerPos[0].y >= GRID_NB_LINES;
		const bool bCollisionWithOwnBody = bCollisionWithWall ? false : isCollidingWithSelf();
		const bool bTimeOut = m_tickWithoutEating > (m_score + 1) * TIMEOUT_TICK_RATIO;

		//score
		score = computeScore();

		//death
		if (bCollisionWithWall || bCollisionWithOwnBody || bTimeOut)
		{
			//if (bCollisionWithWall)
			//	printf("--- GAME OVER : Collision with Wall --- Score: %d, Ticks: %d\n", m_score, m_tick);
			//else if (bCollisionWithOwnBody)
			//	printf("--- GAME OVER : Collision with Body --- Score: %d, Ticks: %d\n", m_score, m_tick);
			//else if (bTimeOut)
			//	printf("--- GAME OVER : Time Out --- Score: %d, Ticks: %d\n", m_score, m_tick);

			if (bCollisionWithWall)
				death = Death::WALL;
			else if (bCollisionWithOwnBody)
				death = Death::OWN;
			else
				death = Death::TIMEOUT;

			bGameOver = true;
			return;
		}

		//compute new state
		m_state = computeState();

		//reset action for next tick
		actionApplied = Action::STRAIGHT;
	};
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
	int snakeSize = (int)m_playerPos.size();
	for (int i = 0; i < snakeSize; i++)
	{
		sf::RectangleShape payerCell(sf::Vector2f(CELL_SIZE - GRID_THICKNESS, CELL_SIZE - GRID_THICKNESS));
		payerCell.setFillColor(sf::Color(0, 255, 0, 255));
		payerCell.setOutlineColor(sf::Color(255, 255, 255, 0));
		payerCell.setPosition(getCellPos(m_playerPos[i].x, m_playerPos[i].y));
		m_window->draw(payerCell);
	}

	//food
	sf::RectangleShape foodCell(sf::Vector2f(CELL_SIZE - GRID_THICKNESS, CELL_SIZE - GRID_THICKNESS));
	foodCell.setFillColor(sf::Color(255, 0, 0, 255));
	foodCell.setOutlineColor(sf::Color(255, 255, 255, 0));
	foodCell.setPosition(getCellPos(m_foodPos.x, m_foodPos.y));
	m_window->draw(foodCell);

	//text
	sf::Text scoreText;
	scoreText.setFont(*m_font);
	scoreText.setCharacterSize(20.f);
	scoreText.setColor(sf::Color::White);
	scoreText.setPosition(50.f, 50.f);
	scoreText.setString("TEST");
}

State Game::computeState()
{
	State state;

	sf::Vector2u nextCellStraight = getNextCell(Action::STRAIGHT);
	state.bDangerStraight = isDangerCell(nextCellStraight.x, nextCellStraight.y);

	sf::Vector2u nextCellLeft = getNextCell(Action::TURN_LEFT);
	state.bDangerLeft = isDangerCell(nextCellLeft.x, nextCellLeft.y);

	sf::Vector2u nextCellRight = getNextCell(Action::TURN_RIGHT);
	state.bDangerRight = isDangerCell(nextCellRight.x, nextCellRight.y);

	state.bDirectionLeft = m_playerDirection == Direction::LEFT;
	state.bDirectionRight = m_playerDirection == Direction::RIGHT;
	state.bDirectionUp = m_playerDirection == Direction::UP;
	state.bDirectionDown = m_playerDirection == Direction::DOWN;

	sf::Vector2u headPos = m_playerPos[0];
	state.bFoodLeft = m_foodPos.x < headPos.x;
	state.bFoodRight = m_foodPos.x > headPos.x;
	state.bFoodUp = m_foodPos.y < headPos.y;
	state.bFoodDown = m_foodPos.y > headPos.y;

	return state;
}

int Game::getStateId() const
{
	const int decimal =
			(int)m_state.bDangerStraight
		+	(int)m_state.bDangerLeft * 2
		+	(int)m_state.bDangerRight * 2 * 2
		+	(int)m_state.bDirectionLeft * 2 * 2 * 2
		+	(int)m_state.bDirectionRight * 2 * 2 * 2 * 2
		+	(int)m_state.bDirectionUp * 2 * 2 * 2 * 2 * 2
		+	(int)m_state.bDirectionDown * 2 * 2 * 2 * 2 * 2 * 2
		+	(int)m_state.bFoodLeft * 2 * 2 * 2 * 2 * 2 * 2 * 2
		+	(int)m_state.bFoodRight * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2
		+	(int)m_state.bFoodUp * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2
		+	(int)m_state.bFoodDown * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2 * 2;

	return decimal;
}

Action Game::getAction(const int stateId, const Individual& individual)
{
	Action action = (Action)individual.m_dna[stateId];
	return action;
}

int Game::computeScore()
{
	const int tickScore = m_tick;
	const int foodScore = m_score;
	const int efficiencyMalus = m_score > 0 ? m_tick / m_score : 0;
	const int finalScore = m_tick + 100 * m_score - efficiencyMalus;
	return finalScore;
}