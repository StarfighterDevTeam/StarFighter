#include "Game.h"
#include "Globals.h"
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
	

	reset();
}

void Game::reset()
{
	
}

void Game::update(sf::Time dt)
{
	
}

void Game::draw()
{
	
}