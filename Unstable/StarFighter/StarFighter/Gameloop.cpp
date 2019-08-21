#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//map
	(*CurrentGame).m_map_size = sf::Vector2f(1920, 1080);

	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/V_Alpha1.png", sf::Vector2f(68, 84), sf::Vector2f(34, 42), 1, 1);
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShipObject);

	//star
	StarGenerator* generator = new StarGenerator();
	generator->CreateStar();

	//star generator
	//for (int i = 0; i < 3; i++)
	//{
		//m_star_generator.push_back(new StarGenerator());
	//}
}

Gameloop::~Gameloop()
{
	for (StarGenerator* star_generator : m_star_generator)
	{
		delete star_generator;
	}
	m_star_generator.clear();
}

void Gameloop::Update(sf::Time deltaTime)
{
	//for (StarGenerator* star_generator : m_star_generator)
	//{
	//	star_generator->Update(deltaTime);
	//}
	
	(*CurrentGame).UpdateScene(deltaTime);

	//UpdateCamera(deltaTime);
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(sf::Time deltaTime)
{
	(*CurrentGame).m_view.move(sf::Vector2f((*CurrentGame).m_playerShip->m_speed.x * deltaTime.asSeconds(), (*CurrentGame).m_playerShip->m_speed.y * deltaTime.asSeconds()));

	//Map border constraints
	const float x = (*CurrentGame).m_view.getSize().x / 2;
	const float y = (*CurrentGame).m_view.getSize().y / 2;
	const float a = (*CurrentGame).m_playerShip->m_position.x;
	const float b = (*CurrentGame).m_playerShip->m_position.y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}