#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(1920, 1080));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject, false);
	(*CurrentGame).m_map_size = m_background->m_size;

	//ship
	(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Green, 16);
	(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerLayer, PlayerShip, false);

	//load level 1
	m_current_map_filename = PLAYER_SAVE_FILE;
	if (LoadMap(m_current_map_filename) == false)
	{
		Door::AddDoor(pair<int, int>(0, 0), pair<int, int>(0, 1), 4, 1, false);

		Door::AddDoor(pair<int, int>(0, 0), pair<int, int>(1, 0), 0, 1, false);
		Door::AddDoor(pair<int, int>(2, 2), pair<int, int>(3, 2), 4, 1, false);

		m_start = pair<int, int>(0, 0);
		m_finish = pair<int, int>(NB_TILES_X - 1, NB_TILES_Y - 1);

		SaveMap(m_current_map_filename);
	}
}

bool Gameloop::LoadMap(string map_filename)
{
	std::ifstream data(string(getSavesPath()) + map_filename, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		int i = 0;
		std::string line;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			i++;

			//start & finish tiles
			if (i == 1 || i == 2)
			{
				string word;
				pair<int, int> tile;
				ss >> word >> tile.first >> tile.second;

				if (i == 1)
				{
					m_start = tile;
					(*CurrentGame).m_playerShip->m_tile_coord = { m_start.first, m_start.second };
					(*CurrentGame).m_playerShip->setPosition((*CurrentGame).getTilePosition(m_start));
				}
					
				else
				{
					m_finish = tile;
					GameObject* exit = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Yellow, 16);
					(*CurrentGame).addToScene(exit, ExitLayer, ExitObject, false);
					exit->m_tile_coord = m_finish;
					exit->setPosition((*CurrentGame).getTilePosition(m_finish));
				}
					
			}
			else// doors
			{
				pair<int, int> tileA;
				pair<int, int> tileB;
				int frequency;
				int value;
				ss >> tileA.first >> tileA.second >> tileB.first >> tileB.second >> frequency >> value;

				Door::AddDoor(tileA, tileB, frequency, value, false);
			}
		}

		data.close();  // on ferme le fichier

		SaveMap(map_filename);
		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found. A new file is going to be created.\n" << endl;
		return false;
	}
}

bool Gameloop::SaveMap(string map_filename)
{
	printf("Save\n");

	ofstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		data << "start " << m_start.first << " " << m_start.second << endl;
		data << "finish " << m_finish.first << " " << m_finish.second << endl;

		vector<Door*> save_list;
		for (GameObject* object : (*CurrentGame).m_sceneGameObjectsTyped[DoorObject])
		{
			Door* door = (Door*)object;

			if (door->m_frequency >= 0 && door->m_visible == true)
				save_list.push_back(door);
		}
		for (GameObject* object : (*CurrentGame).m_tmp_sceneGameObjects)
		{
			if (object->m_collider_type == DoorObject && object->m_visible == true)
			{
				Door* door = (Door*)object;

				if (door->m_frequency >= 0)
					save_list.push_back(door);
			}
		}

		for (Door* door : save_list)
			data << door->m_tileA.first << " " << door->m_tileA.second << " " << door->m_tileB.first << " " << door->m_tileB.second << " " << door->m_frequency << " " << door->m_offset << endl;

		data.close();  // on ferme le fichier

		return true;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No existing save file founded. A new file is going to be created.\n" << endl;
	}

	return false;
}



Gameloop::~Gameloop()
{
	delete m_background;
}

void Gameloop::Update(sf::Time deltaTime)
{
	(*CurrentGame).GetMouseInputs(deltaTime);

	(*CurrentGame).updateScene(deltaTime);

	//UpdateCamera(deltaTime);

	//Load next level
	sf::Vector2f finish_vector = (*CurrentGame).m_playerShip->getPosition() - (*CurrentGame).getTilePosition(m_finish);
	if (GetVectorLengthSquared(finish_vector) <= (*CurrentGame).m_playerShip->m_size.x * (*CurrentGame).m_playerShip->m_size.x)
	{
		printf("Finish\n");
		//LoadMap(PLAYER_SAVE_FILE);
	}

	//auto save editior mode
	if ((*CurrentGame).m_playerShip->m_editor_mode == true)
	{
		for (int i = 0; i < NBVAL_ActionButtons; i++)
			if ((*CurrentGame).m_playerShip->m_inputs_states[i] != Input_Release)
			{
				SaveMap(m_current_map_filename);
				break;
			}
	}
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