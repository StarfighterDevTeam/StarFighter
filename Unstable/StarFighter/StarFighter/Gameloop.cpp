#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(1920, 1080));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).m_map_size = m_background->m_size;

	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::White , sf::Vector2f(64, 64));
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);
	(*CurrentGame).m_playerShip->setColor(sf::Color(255, 255, 255, 0));

	//node
	CreateTerminal(sf::Vector2f(200, 440), true);
	CreateNode(sf::Vector2f(450, 540), true);
	CreateNode(sf::Vector2f(650, 340), true);

	CreateNode(sf::Vector2f(1920 - 400, 540), false);

}

Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(sf::Time deltaTime)
{
	(*CurrentGame).updateScene(deltaTime);

	UpdateCamera(deltaTime);
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
	const float a = (*CurrentGame).m_playerShip->getPosition().x;
	const float b = (*CurrentGame).m_playerShip->getPosition().y;
	if (a < x)
		(*CurrentGame).m_view.setCenter(x, (*CurrentGame).m_view.getCenter().y);
	if (a >(*CurrentGame).m_map_size.x - x)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_map_size.x - x, (*CurrentGame).m_view.getCenter().y);
	if (b < y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, y);
	if (b >(*CurrentGame).m_map_size.y - y)
		(*CurrentGame).m_view.setCenter((*CurrentGame).m_view.getCenter().x, (*CurrentGame).m_map_size.y - y);
}

//Liaison 16
GameObject* Gameloop::CreateNode(sf::Vector2f position, bool player)
{
	GameObject* node = new GameObject(position, sf::Vector2f(0, 0), player ? sf::Color::Blue : sf::Color::Red, 16, 4);
	(*CurrentGame).addToScene(node, NodeLayer, NodeObject);

	return node;
}

GameObject* Gameloop::CreateTerminal(sf::Vector2f position, bool player)
{
	GameObject* node = new GameObject(position, sf::Vector2f(0, 0), player ? sf::Color::Blue : sf::Color::Red, 32, 4);
	(*CurrentGame).addToScene(node, NodeLayer, NodeObject);

	return node;
}