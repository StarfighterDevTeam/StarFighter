#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(512, 512), sf::Vector2f(0, 0), "2D/water.jpg", sf::Vector2u(1024, 1024));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).m_map_size = m_background->m_size;

	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), "2D/pirate_ship_100_136.png", sf::Vector2u(100, 136));
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);
}

Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(const float DTIME)
{
	(*CurrentGame).GetMouseInputs(DTIME);

	(*CurrentGame).updateScene(DTIME);

	UpdateCamera(DTIME);
}

void Gameloop::Draw()
{
	(*CurrentGame).drawScene();
}

void Gameloop::UpdateCamera(const float DTIME)
{
	(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
}