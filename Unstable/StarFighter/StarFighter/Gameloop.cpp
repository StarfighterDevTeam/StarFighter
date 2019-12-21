#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(1920, 1080));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject);
	(*CurrentGame).m_map_size = m_background->m_size;

	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Green, 16);
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerLayer, PlayerShip);

	if (LoadMap(PLAYER_SAVE_FILE) == false)
	{
		//create scenario
		AddDoor(pair<int, int>(0, 0), pair<int, int>(0, 1), 4, 1);

		AddDoor(pair<int, int>(0, 0), pair<int, int>(1, 0), 0, 1);
		AddDoor(pair<int, int>(2, 2), pair<int, int>(3, 2), 4, 1);

		Ship::SaveShip((*CurrentGame).m_playerShip);
	}
}

bool Gameloop::LoadMap(string map_filename)
{
	std::ifstream data(string(getSavesPath()) + map_filename, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			pair<int, int> tileA;
			pair<int, int> tileB;
			int frequency;
			int value;
			ss >> tileA.first >> tileA.second >> tileB.first >> tileB.second >> frequency >> value;

			AddDoor(tileA, tileB, frequency, value);
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool Gameloop::AddDoor(pair<int, int> tileA, pair<int, int> tileB, int frequency, int value)
{
	Door* door = new Door(tileA, tileB, frequency, value);
	(*CurrentGame).addToScene(door, DoorLayer, DoorObject);

	return true;
}

Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(sf::Time deltaTime)
{
	(*CurrentGame).GetMouseInputs(deltaTime);

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