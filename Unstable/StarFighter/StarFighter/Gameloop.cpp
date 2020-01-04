#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	m_background = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Black, sf::Vector2f(1920, 1080));
	(*CurrentGame).addToScene(m_background, BackgroundLayer, BackgroundObject, false);
	(*CurrentGame).m_map_size = m_background->m_size;

	//load level 1
	m_current_map_filename = PLAYER_SAVE_FILE;
	if (LoadMap(m_current_map_filename) == false)
		printf("Level loaded successfully\n");
	else
		printf("Level loading failed\n");

	//Editor mode init
	(*CurrentGame).m_editor_door = new Door(pair<int, int>(0, 0), pair<int, int>(0, 1), -1, 1, 0);
}

void Gameloop::ClearMap()
{
	delete (*CurrentGame).m_playerShip;
	delete (*CurrentGame).m_exit;
	
	for (Door* door : (*CurrentGame).m_doors)
		delete door;
	(*CurrentGame).m_doors.clear();

	for (GameObject* tile : (*CurrentGame).m_tiles)
		delete tile;
	(*CurrentGame).m_tiles.clear();
}

bool Gameloop::LoadMap(string map_filename)
{
	printf("Loading map: %s\n", map_filename.c_str());

	std::ifstream data(string(getSavesPath()) + map_filename, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		//Clear current map
		ClearMap();

		//Time offset to match the music
		//float song_offset = fmod(((*CurrentGame).m_time + SONG_OFFSET), (60 / SONG_BPM));
		float song_offset = SONG_OFFSET;
		song_offset -= (*CurrentGame).m_time;

		while (song_offset > 60.f / SONG_BPM)
			song_offset -= 60.f / SONG_BPM;
		while (song_offset < 0)
			song_offset += 60.f / SONG_BPM;

		(*CurrentGame).m_time = song_offset;

		//Tiles
		for (int i = 0; i < NB_TILES_X; i++)
			for (int j = 0; j < NB_TILES_Y; j++)
			{
				//GameObject* tile = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), sf::Color(128, 128, 128, 255), sf::Vector2f(TILE_SIZE, TILE_SIZE), 2);
				GameObject* tile = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/tile.png", sf::Vector2f(TILE_SIZE, TILE_SIZE));
				tile->m_tile_coord = { i, j };
				tile->setPosition(START_X + (i * TILE_SIZE), START_Y - (j * TILE_SIZE));
				(*CurrentGame).m_tiles.push_back(tile);
			}

		//Doors
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

					(*CurrentGame).m_playerShip = new Ship(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Green, 16);
					(*CurrentGame).m_playerShip->m_tile_coord = { m_start.first, m_start.second };
					(*CurrentGame).m_playerShip->setPosition((*CurrentGame).getTilePosition(m_start));
				}
					
				else
				{
					(*CurrentGame).m_exit = new GameObject(sf::Vector2f(960, 540), sf::Vector2f(0, 0), sf::Color::Yellow, 16);
					(*CurrentGame).m_exit->m_tile_coord = tile;
					(*CurrentGame).m_exit->setPosition((*CurrentGame).getTilePosition(tile));
				}
			}
			else// doors
			{
				pair<int, int> tileA;
				pair<int, int> tileB;
				int frequency;
				int value;
				ss >> tileA.first >> tileA.second >> tileB.first >> tileB.second >> frequency >> value;

				Door::AddDoor(tileA, tileB, frequency, value, false, song_offset);
			}
		}
		
		//Reset map infos
		m_current_map_filename = map_filename;
		
		data.close();  // on ferme le fichier

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
		data << "finish " << (*CurrentGame).m_exit->m_tile_coord.first << " " << (*CurrentGame).m_exit->m_tile_coord.second << endl;

		for (Door* door : (*CurrentGame).m_doors)
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

	//Collision of player with doors
	for (Door* door : (*CurrentGame).m_doors)
	{
		if (GetVectorLengthSquared(sf::Vector2f((*CurrentGame).m_playerShip->getPosition() - door->getPosition())) < (PLAYER_SIZE * 0.5 + DOOR_WIDTH * 0.5) * (PLAYER_SIZE * 0.5 + DOOR_WIDTH * 0.5))
		{
			float door_size = door->GetDoorSize();
			int vertical_door = (int)(door->getRotation() > 0);
			int horizontal_door = (int)!vertical_door;

			float x1 = door->m_door_UL.getGlobalBounds().left + door->m_door_UL.getOutlineThickness();
			float x2 = door->m_door_UL.getGlobalBounds().left + door->m_door_UL.getOutlineThickness() + 1.f * horizontal_door * door_size + 1.f * vertical_door * DOOR_WIDTH;
			float y1 = door->m_door_UL.getGlobalBounds().top + door->m_door_UL.getOutlineThickness();
			float y2 = door->m_door_UL.getGlobalBounds().top + door->m_door_UL.getOutlineThickness() + 1.f * horizontal_door * DOOR_WIDTH + 1.f * vertical_door * door_size;

			if (GetVectorLengthSquared(sf::Vector2f((*CurrentGame).m_playerShip->getPosition() - sf::Vector2f(x1, y1))) <= PLAYER_SIZE * 0.5 * PLAYER_SIZE * 0.5
				|| GetVectorLengthSquared(sf::Vector2f((*CurrentGame).m_playerShip->getPosition() - sf::Vector2f(x2, y1))) <= PLAYER_SIZE * 0.5 * PLAYER_SIZE * 0.5
				|| GetVectorLengthSquared(sf::Vector2f((*CurrentGame).m_playerShip->getPosition() - sf::Vector2f(x2, y1))) <= PLAYER_SIZE * 0.5 * PLAYER_SIZE * 0.5
				|| GetVectorLengthSquared(sf::Vector2f((*CurrentGame).m_playerShip->getPosition() - sf::Vector2f(x2, y2))) <= PLAYER_SIZE * 0.5 * PLAYER_SIZE * 0.5)
			{
				//Death
				LoadMap(m_current_map_filename);
				break;
			}
		}
	}

	//Load next level
	sf::Vector2f finish_vector = (*CurrentGame).m_playerShip->getPosition() - (*CurrentGame).m_exit->getPosition();
	if (GetVectorLengthSquared(finish_vector) <= (*CurrentGame).m_playerShip->m_size.x * (*CurrentGame).m_playerShip->m_size.x)
	{
		printf("Finish\n");
		LoadMap("Saves/Level2.txt");
	}

	//auto save editior mode
	if ((*CurrentGame).m_playerShip->m_editor_mode == true)
	{
		for (int i = Action_EDITOR_CHANGE_START; i < Action_EDITOR_CHANGE_END; i++)
			if ((*CurrentGame).m_playerShip->m_inputs_states[i] == Input_Tap)
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