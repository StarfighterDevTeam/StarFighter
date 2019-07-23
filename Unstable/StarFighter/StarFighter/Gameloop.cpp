#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(1980, 1080), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1920, 1080));
}

Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(sf::Time deltaTime)
{
	(*CurrentGame).updateScene(deltaTime);
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}