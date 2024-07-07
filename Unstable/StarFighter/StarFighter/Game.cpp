#include "Game.h"
#include "Globals.h"
#include "assert.h"
#include <sstream>

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
	
	reset(4);

	printf("");
}

void Game::reset(const int nbPlayers)
{
	m_currentPlayer = 0;

	for (int playerIndex = 0; playerIndex < nbPlayers; playerIndex++)
	{
		m_players.push_back(Player());

		//TODO
		m_players[playerIndex].addToDiscard(Card(Air, Power1));
		m_players[playerIndex].addToDiscard(Card(Air, Power2));
		m_players[playerIndex].addToDiscard(Card(Air, Power3));
		
		m_players[playerIndex].addToDiscard(Card(Fire, Power1));
		m_players[playerIndex].addToDiscard(Card(Fire, Power2));
		m_players[playerIndex].addToDiscard(Card(Fire, Power3));

		m_players[playerIndex].addToDiscard(Card(Water, Power1));
		m_players[playerIndex].addToDiscard(Card(Water, Power2));
		m_players[playerIndex].addToDiscard(Card(Water, Power3));

		//Shuffle
		m_players[playerIndex].remakeDrawPile();

		for (int drawIndex = 0; drawIndex < 6; drawIndex++)
			m_players[playerIndex].draw();
	}
}

void Game::update(sf::Time dt)
{
	
}

void Game::draw()
{
	//Draw Pile
	sf::RectangleShape drawPileRect;
	drawPileRect.setSize(sf::Vector2f(150, 50));
	drawPileRect.setOutlineThickness(2);
	drawPileRect.setFillColor(sf::Color(255, 255, 255, 0));
	drawPileRect.setPosition(50, WINDOW_RESOLUTION_Y - 200);

	sf::Text drawPileText;
	drawPileText.setFont(*m_font);
	drawPileText.setCharacterSize(20);
	std::ostringstream ss_draw;
	ss_draw << "Draw pile (" << (int)m_players[m_currentPlayer].m_draw.size() << ")";
	drawPileText.setString(ss_draw.str());
	drawPileText.setPosition(50, WINDOW_RESOLUTION_Y - 200);

	m_window->draw(drawPileRect);
	m_window->draw(drawPileText);

	//Discard Pile
	sf::RectangleShape discardPileRect;
	discardPileRect.setSize(sf::Vector2f(150, 50));
	discardPileRect.setOutlineThickness(2);
	discardPileRect.setFillColor(sf::Color(255, 255, 255, 0));
	discardPileRect.setPosition(50, WINDOW_RESOLUTION_Y - 100);

	sf::Text discardPileText;
	discardPileText.setFont(*m_font);
	discardPileText.setCharacterSize(20);
	std::ostringstream ss_discard;
	ss_discard << "Discard pile (" << (int)m_players[m_currentPlayer].m_discard.size() << ")";
	discardPileText.setString(ss_discard.str());
	discardPileText.setPosition(50, WINDOW_RESOLUTION_Y - 100);

	m_window->draw(discardPileRect);
	m_window->draw(discardPileText);
}