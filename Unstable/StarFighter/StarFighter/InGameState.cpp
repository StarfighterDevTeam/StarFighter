#include "InGameState.h"

extern Game* CurrentGame;

void InGameState::Initialize(Player player)
{
	this->mainWindow = player.m_playerWindow;
	(*CurrentGame).init(this->mainWindow);
	
	Ship* playerShip = new Ship(sf::Vector2f(SHIP_START_X, SHIP_START_Y), sf::Vector2f(0, 0), "2D/natalia.png", sf::Vector2f(64, 64), sf::Vector2f(32, 32), 3);
	(*CurrentGame).m_playerShip = playerShip;
	//(*CurrentGame).addToScene((*CurrentGame).m_playerShip, PlayerShipLayer, PlayerShip);

	//Load saved file
	//if (!Ship::LoadShip(playerShip))
	//{
	//	//or create a new save file
	//	Ship::SaveShip(playerShip);
	//}

	//Loading scripts
	LoadCSVFile(SHIP_CSV_FILE);

	GameObject* background = new GameObject(sf::Vector2f(990, 540), sf::Vector2f(0, 0), "2D/background.png", sf::Vector2f(1980, 1080), sf::Vector2f(990, 540));
	(*CurrentGame).addToScene(background, BackgroundLayer, BackgroundObject);

	(*CurrentGame).m_map_size = background->m_size;
	//(*CurrentGame).m_view.setCenter((*CurrentGame).m_playerShip->getPosition());
	//(*CurrentGame).m_playerShip->SetControllerType(AllControlDevices);

	//PIRATES
	InitWaterZones();
	m_warship = new Warship(DMS_Coord{0, 6, 0, 0, 6, 0 });
}

void InGameState::InitWaterZones()
{
	//zones and water
	vector<WaterZone*> vec;
	WaterZone* zone = new WaterZone(0, 0);
	vec.push_back(zone);
	(*CurrentGame).m_waterzones.push_back(vec);

	//islands
	m_island = new Island(8, 9, 2, 2, zone, 0, 0);
}

void InGameState::Update(sf::Time deltaTime)
{
	GameEntity* selection = (*CurrentGame).m_selected_ui;
	GameEntity* previous_selection = selection;
	GameEntity* hovered = (*CurrentGame).m_hovered_ui;
	MouseAction& mouse_click = (*CurrentGame).m_mouse_click;

	//Get mouse & keyboard inputs
	(*CurrentGame).GetMouseInputs(deltaTime);
	(*CurrentGame).m_playerShip->update(deltaTime);

	//Reset flags
	(*CurrentGame).m_hovered_ui = NULL;
	if ((*CurrentGame).m_selected_ui != NULL && mouse_click == Mouse_LeftClick)
	{
		(*CurrentGame).m_selected_ui->m_selected = false;
		(*CurrentGame).m_selected_ui->m_shape_container.setOutlineColor((*CurrentGame).m_selected_ui->m_default_color);
		(*CurrentGame).m_selected_ui = NULL;
	}

	//UPDATING GAME ENTITIES

	//Interaction with rooms
	for (vector<Room*>::iterator it = m_warship->m_rooms.begin(); it != m_warship->m_rooms.end(); it++)
	{
		m_warship->UpdateCrewMembersCountPerRoom(*it);
		(*it)->Update(deltaTime);
	}

	//Room connexions
	for (vector<RoomConnexion*>::iterator it = m_warship->m_connexions.begin(); it != m_warship->m_connexions.end(); it++)
	{
		(*it)->Update(deltaTime);
	}

	//Crew movement
	for (vector<CrewMember*>::iterator it = m_warship->m_crew.begin(); it != m_warship->m_crew.end(); it++)
	{
		(*it)->Update(deltaTime);
	}

	//TEMP DEBUG: crew movement feedback
	for (vector<RoomTile*>::iterator it = (*CurrentGame).m_tiles.begin(); it != (*CurrentGame).m_tiles.end(); it++)
	{
		(*it)->m_shape_container.setFillColor(sf::Color::Black);
		for (vector<CrewMember*>::iterator it2 = m_warship->m_crew.begin(); it2 != m_warship->m_crew.end(); it2++)
		{
			if ((*it2)->m_destination == (*it) && (*it2)->m_selected == true)
			{
				(*it)->m_shape_container.setFillColor(sf::Color::Green);
			}
		}
	}

	//ACTIONS
	//Crew move to room
	if (mouse_click == Mouse_RightClick && selection != NULL && selection->m_UI_type == UI_CrewMember && hovered != NULL && hovered->m_UI_type == UI_Room)
	{
		CrewMember* crew = (CrewMember*)selection;
		Room* room = (Room*)hovered;
		RoomTile* previous_destination = crew->m_destination;
		RoomTile* destination = crew->GetFreeRoomTile(room);

		//if destination is valid (it exists and we're not already on it and the room is not full), book the tile
		if (destination != NULL && destination->m_crew != crew && room->m_nb_crew[Crew_All] < room->m_nb_crew_max)
		{
			crew->m_destination = destination;
			destination->m_crew = crew;

			//...and free the previous booking (if any)
			if (previous_destination != NULL && previous_destination->m_crew != NULL)
			{
				previous_destination->m_crew = NULL;
			}
		}
	}

	//water tiles
	for (vector<vector<WaterTile*> >::iterator it = m_warship->m_zone->m_watertiles.begin(); it != m_warship->m_zone->m_watertiles.end(); it++)
	{
		for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			//position on "radar"
			(*it2)->m_position.x = WATERTILE_OFFSET_X + WATERTILE_SIZE * (0.5f - (m_warship->m_DMS.m_minute_x + m_warship->m_DMS.m_second_x / 60) + NB_WATERTILE_VIEW_RANGE + (*it2)->m_coord_x);
			(*it2)->m_position.y = WATERTILE_OFFSET_Y + WATERTILE_SIZE * (0.5f - (NB_WATERTILE_Y - m_warship->m_DMS.m_minute_y - m_warship->m_DMS.m_second_y / 60) + NB_WATERTILE_VIEW_RANGE - (*it2)->m_coord_y + NB_WATERTILE_Y);//from bottom to top

			//can be seen? no need to update other tiles because they won't be drawn anyway
#if !defined MAP_REVEAL_FLOATING
			float distance = m_warship->GetDistanceFloatToWaterTile(*it2);
			if (distance <= NB_WATERTILE_VIEW_RANGE + 0.3f || ((*it2)->m_can_be_seen == true && distance <= NB_WATERTILE_VIEW_RANGE + 0.95f))
#else
			int distance = m_warship->GetDistanceToWaterTile(*it2);
			if (distance <= NB_WATERTILE_VIEW_RANGE)
#endif
			{
				(*it2)->m_can_be_seen = true;

				//selection
				if (selection == m_warship && (*it2)->m_type == Water_Empty)// && m_warship->m_destination == NULL
				{
					//display distances to the boat
					if (distance != 0)
					{
						ostringstream ss;
						ss << distance;
						(*it2)->m_text.setString(ss.str());
					}

					(*it2)->GameEntity::Update(deltaTime);
				}
				else
				{
					(*it2)->GameEntity::UpdatePosition();
					(*it2)->m_text.setString("");

					//selection of water tiles is forbidden
					if ((*it2)->m_selected == true)
					{
						(*it2)->m_selected = false;
						(*it2)->m_shape_container.setOutlineColor((*it2)->m_default_color);
						(*CurrentGame).m_selected_ui = NULL;
					}
				}
			}
			else
			{
				(*it2)->m_can_be_seen = false;
			}
		}
	}

	//boat - must be done after water tiles update because it's attached to a tile
	m_warship->Update(deltaTime);

	//island
	if (m_island != NULL)
	{
		//position on "radar"
		WaterTile* tileUpLeft = m_warship->m_zone->m_watertiles[m_island->m_upcorner_x][m_island->m_upcorner_y];
		WaterTile* tileDownRight = m_warship->m_zone->m_watertiles[m_island->m_upcorner_x + m_island->m_width - 1][m_island->m_upcorner_y - m_island->m_height + 1];
		float pos_x = 0.5f * (tileDownRight->m_position.x + tileUpLeft->m_position.x);
		float pos_y = 0.5f * (tileDownRight->m_position.y + tileUpLeft->m_position.y);
		m_island->m_position = sf::Vector2f(pos_x, pos_y);
	
		m_island->Update(deltaTime);
	}

	//Actions
	if (mouse_click == Mouse_RightClick && selection != NULL && selection == m_warship && hovered != NULL && hovered->m_UI_type == UI_WaterTile)
	{
		WaterTile* tile = (WaterTile*)hovered;
		//printf("distance: %d\n", m_warship->GetDistanceToWaterTile(tile));
		m_warship->SetSailsToWaterTile(tile);
	}
}

void InGameState::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//Draw
	(*CurrentGame).drawScene();//background

	//PIRATES

	//tiles
	for (vector<RoomTile*>::iterator it = (*CurrentGame).m_tiles.begin(); it != (*CurrentGame).m_tiles.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//rooms
	for (vector<Room*>::iterator it = m_warship->m_rooms.begin(); it != m_warship->m_rooms.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//doors
	for (vector<RoomConnexion*>::iterator it = m_warship->m_connexions.begin(); it != m_warship->m_connexions.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//crew
	for (vector<CrewMember*>::iterator it = m_warship->m_crew.begin(); it != m_warship->m_crew.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//WATER PART

	//water tiles
	for (vector<vector<WaterTile*> >::iterator it = m_warship->m_zone->m_watertiles.begin(); it != m_warship->m_zone->m_watertiles.end(); it++)
	{
		//preliminary check that takes away a whole vector to check + 1 margin
		if (it->front()->m_coord_x < m_warship->m_DMS.m_minute_x - NB_WATERTILE_VIEW_RANGE - 1 || it->front()->m_coord_x > m_warship->m_DMS.m_minute_x + NB_WATERTILE_VIEW_RANGE + 1)
		{
			continue;
		}

		for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			if ((*it2)->m_can_be_seen == true)
			{
				(*it2)->Draw((*CurrentGame).m_mainScreen);
			}
		}
	}

	//boat
	m_warship->Draw((*CurrentGame).m_mainScreen);

	//island
	if (m_island != NULL)
	{
		m_island->Draw((*CurrentGame).m_mainScreen);
	}

	//Display
	(*CurrentGame).m_mainScreen.display();
	sf::Sprite temp((*CurrentGame).m_mainScreen.getTexture());
	temp.scale((*CurrentGame).m_scale_factor.x, (*CurrentGame).m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	(*CurrentGame).getMainWindow()->draw(temp);
}

void InGameState::Release()
{
	//TODO
}

void InGameState::UpdateCamera(sf::Time deltaTime)
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

void InGameState::DestroySFPanel(Ship* playerShip)
{
	if (playerShip->m_SFTargetPanel)
	{
		(*CurrentGame).removeFromFeedbacks(playerShip->m_SFTargetPanel);
		delete playerShip->m_SFTargetPanel;
		playerShip->m_SFTargetPanel = NULL;
	}
}

void InGameState::CreateSFPanel(SFPanelTypes panel_type, Ship* playerShip)
{
	switch (panel_type)
	{
		case SFPanel_Specific:
		{
			playerShip->m_SFTargetPanel = new SFPanelSpecific(sf::Vector2f(SFPANEL_SPECIFIC_WIDTH, SFPANEL_SPECIFIC_HEIGHT), SFPanel_Specific, playerShip);
			break;
		}
	}
	(*CurrentGame).addToFeedbacks((*CurrentGame).m_playerShip->m_SFTargetPanel);
}

void InGameState::LoadCSVFile(string scenes_file)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading scripts.");

	vector<vector<string> > allConfigs = *(FileLoaderUtils::FileLoader(scenes_file));
	size_t allConfigVectorSize = allConfigs.size();
	for (size_t i = 0; i < allConfigVectorSize; i++)
	{
		(*CurrentGame).m_gameObjectsConfig.insert(std::map<string, vector<string> >::value_type(allConfigs[i][0], allConfigs[i]));
	}

	allConfigs.clear();
}