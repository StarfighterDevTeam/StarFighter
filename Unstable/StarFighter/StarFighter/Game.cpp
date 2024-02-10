#include "Game.h"
#include "assert.h"

#define GRID_NB_LINES	20
#define LEFT_MARGIN		150.f
#define UP_MARGIN		150.f
#define CELL_SIZE		20.f
#define GRID_THICKNESS	2.f
#define GAME_SPEED		5.f

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

	reset();
}

void Game::reset()
{
	playerPos = sf::Vector2u(0, 10);
	playerDirection = Direction::RIGHT;
	tick = 0;
	tickTimer = 0.f;
}

sf::Vector2f Game::getCellPos(int x, int y)
{
	assert(x >= 0 && x < GRID_NB_LINES && y >= 0 && y < GRID_NB_LINES);
	return sf::Vector2f(LEFT_MARGIN + 1.f * x * CELL_SIZE, UP_MARGIN + 1.f * y * CELL_SIZE);
}

void Game::update(sf::Time dt, Action action)
{
	tickTimer += dt.asSeconds();
	const float frameDuration = 1.f / GAME_SPEED;

	static Action actionApplied = Action::STRAIGHT;//store action if it's a turn and apply it once per tick
	if (action != Action::STRAIGHT)
		actionApplied = action;

	if (tickTimer > 1.f / GAME_SPEED)
	{
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
		switch (playerDirection)
		{
			case Direction::LEFT:
			{
				playerPos.x--;
				break;
			}
			case Direction::RIGHT:
			{
				playerPos.x++;
				break;
			}
			case Direction::UP:
			{
				playerPos.y--;
				break;
			}
			case Direction::DOWN:
			{
				playerPos.y++;
				break;
			}
		}

		//game over
		if (playerPos.x < 0 || playerPos.x >= GRID_NB_LINES || playerPos.y < 0 || playerPos.y >= GRID_NB_LINES)
		{
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
	sf::RectangleShape payerCell(sf::Vector2f(CELL_SIZE-GRID_THICKNESS, CELL_SIZE- GRID_THICKNESS));
	payerCell.setFillColor(sf::Color(0, 255, 0, 255));
	payerCell.setOutlineColor(sf::Color(255, 255, 255, 0));
	payerCell.setPosition(getCellPos(playerPos.x, playerPos.y));
	m_window->draw(payerCell);

	//texts
	sf::Text tickText;
	tickText.setFont(*m_font);
	tickText.setOrigin(sf::Vector2f(8.f, 8.f));
	tickText.setCharacterSize(16.f);
	tickText.setColor(sf::Color(255, 255, 255, 255));
	tickText.setString("Tessssssssssssst");
	tickText.setPosition(200.f, 20.f);
	m_window->draw(tickText);
	//tickText.setOutlineThickness(2);
	//tickText.setOutlineColor(COLOR_DARKBLUE);
}