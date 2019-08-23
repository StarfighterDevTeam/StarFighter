#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//map
	(*CurrentGame).m_map_size = sf::Vector2f(1920, 1080);

	//ship
	(*CurrentGame).m_playerShip = new Player(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/V_Alpha2.png", sf::Vector2f(68, 84), sf::Vector2f(34, 42), 3, 1);
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShipObject);

	//enemy
	//Enemy* enemy = new Enemy(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/V_Alpha2.png", sf::Vector2f(68 * 3, 84), sf::Vector2f(34, 42), 3, 1);
	Enemy* enemy = new Enemy(Enemy_Alpha, sf::Vector2i(2, 0), 0);
	(*CurrentGame).addToScene(enemy, EnemyShipLayer, EnemyShip);

	//star
	//StarGenerator* generator = new StarGenerator();
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

	//create procedural content for new sectors
	for (sf::Vector2i sector_index : (*CurrentGame).m_star_sectors_to_create)
	{
		PopulateSector(sector_index);
	}
	(*CurrentGame).m_star_sectors_to_create.clear();

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

void Gameloop::PopulateSector(sf::Vector2i sector_index)
{
	//if (sector_index == sf::Vector2i(1, 0))
	Star* new_star = StarGenerator::CreateStar(sector_index);
}