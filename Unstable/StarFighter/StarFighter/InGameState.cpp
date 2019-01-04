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
	m_warships[0].InitWarship();
	
}

void InGameState::InitWaterZones()
{
	//zones and water
	vector<WaterZone*> vec;
	WaterZone* zone = new WaterZone(0, 0);
	vec.push_back(zone);
	(*CurrentGame).m_waterzones.push_back(vec);

	//islands
	Island* island = new Island(7, 8, 2, 2, zone, 0, 0);
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

	//Updating game entities
	Warship& ship = m_warships[0];

	//Interaction with rooms
	for (vector<Room*>::iterator it = ship.m_rooms.begin(); it != ship.m_rooms.end(); it++)
	{
		ship.UpdateCrewMembersCountPerRoom(*it);
		(*it)->Update(deltaTime);
	}

	//Room connexions
	for (vector<RoomConnexion*>::iterator it = ship.m_connexions.begin(); it != ship.m_connexions.end(); it++)
	{
		(*it)->Update(deltaTime);
	}

	//Crew movement
	for (vector<CrewMember*>::iterator it = ship.m_crew.begin(); it != ship.m_crew.end(); it++)
	{
		(*it)->Update(deltaTime);
	}

	//TEMP DEBUG: crew movement feedback
	for (vector<RoomTile*>::iterator it = (*CurrentGame).m_tiles.begin(); it != (*CurrentGame).m_tiles.end(); it++)
	{
		(*it)->m_shape_container.setFillColor(sf::Color::Black);
		for (vector<CrewMember*>::iterator it2 = ship.m_crew.begin(); it2 != ship.m_crew.end(); it2++)
		{
			if ((*it2)->m_destination == (*it) && (*it2)->m_selected == true)
			{
				(*it)->m_shape_container.setFillColor(sf::Color::Green);
			}
		}
	}

	//boat
	ship.Update(deltaTime);

	//islands
	ship.Update(deltaTime);

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

	//WATER PART
	for (vector<vector<WaterTile*> >::iterator it = ship.m_zone->m_watertiles.begin(); it != ship.m_zone->m_watertiles.end(); it++)
	{
		for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			(*it2)->m_position.x = WATERTILE_OFFSET_X + WATERTILE_SIZE * (0.5f - (ship.m_DMS.m_minute_x - NB_WATERTILE_VIEW_RANGE) + (*it2)->m_coord_x);
			(*it2)->m_position.y = WATERTILE_OFFSET_Y + WATERTILE_SIZE * (0.5f + (ship.m_DMS.m_minute_y - NB_WATERTILE_VIEW_RANGE) - (*it2)->m_coord_y + 2.f * NB_WATERTILE_VIEW_RANGE);//from bottom to top
			float a = (*it2)->m_position.x;
			float b = (*it2)->m_position.y;

			//vector<Island*> islands_on_screen;
			//can be seen? no need to update other tiles because they won't be drawn anyway
			int distance = ship.GetDistanceToWaterTile(*it2);
			if (distance <= NB_WATERTILE_VIEW_RANGE)
			{
				//get islands (if any)
				//if ((*it2)->m_type == Water_Island)
				//{
				//	Island* island = (Island*)(*it2);
				//	bool island_found = false;
				//	for (vector<Island*>::iterator it3 = islands_on_screen.begin(); it3 != islands_on_screen.end(); it3++)
				//	{
				//		if (island == *it3)
				//		{
				//			island_found = true;
				//			break;
				//		}
				//	}
				//	if (island_found == false)
				//	{
				//		islands_on_screen.push_back(island);
				//	}
				//}

				//selection
				if (selection == &ship)
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
		}
	}

	//Actions
	if (mouse_click == Mouse_RightClick && selection != NULL && selection == &ship && hovered != NULL && hovered->m_UI_type == UI_WaterTile)
	{
		WaterTile* tile = (WaterTile*)hovered;
		printf("distance: %d\n", ship.GetDistanceToWaterTile(tile));
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
	Warship& ship = m_warships[0];
	for (vector<Room*>::iterator it = ship.m_rooms.begin(); it != ship.m_rooms.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//doors
	for (vector<RoomConnexion*>::iterator it = ship.m_connexions.begin(); it != ship.m_connexions.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//crew
	for (vector<CrewMember*>::iterator it = ship.m_crew.begin(); it != ship.m_crew.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//WATER PART

	//water tiles
	for (vector<vector<WaterTile*> >::iterator it = ship.m_zone->m_watertiles.begin(); it != ship.m_zone->m_watertiles.end(); it++)
	{
		//preliminary check that takes away a whole vector to check
		if (it->front()->m_coord_x < ship.m_tile->m_coord_x - NB_WATERTILE_VIEW_RANGE || it->front()->m_coord_x > ship.m_tile->m_coord_x + NB_WATERTILE_VIEW_RANGE)
		{
			continue;
		}

		for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			if (ship.CanViewWaterTile(*it2))
			{
				(*it2)->Draw((*CurrentGame).m_mainScreen);
			}
		}
	}

	//boat
	ship.Draw((*CurrentGame).m_mainScreen);

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