#include "Gameloop.h"

extern Game* CurrentGame;

Gameloop::Gameloop()
{
	//background
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/background.png", (int)1920, (int)1080);
	m_background = new GameEntity(sf::Vector2f(1980, 1080), UI_None);
	m_background->setAnimation(texture, 1, 1);
	m_background->setPosition(sf::Vector2f(960, 540));

	//PIRATES
	InitWaterZones();
	m_warship = new Warship(DMS_Coord{0, 10, 0, 0, 8, 0 });
	m_ships.push_back(new Ship(DMS_Coord{ 0, 13, 0, 0, 8, 0 }, Ship_Goellete));
	m_tactical_ship = NULL;

	m_scale = Scale_Strategic;
}

Gameloop::~Gameloop()
{
	delete m_background;
	delete m_warship;
	delete m_island;
	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		delete (*it);
	}
}

void Gameloop::InitWaterZones()
{
	//zones and water
	vector<WaterZone*> vec;
	WaterZone* zone = new WaterZone(0, 0);
	vec.push_back(zone);
	(*CurrentGame).m_waterzones.push_back(vec);

	//icons
	TextureLoader *loader;
	loader = TextureLoader::getInstance();
	sf::Texture* texture = loader->loadTexture("2D/seaport_icon.png", (int)WATERTILE_SIZE, (int)WATERTILE_SIZE);

	//islands
	m_island = new Island(8, 9, 2, 2, 0, 0);
	m_island->AddSeaport(Seaport_Small);
}

void Gameloop::Update(sf::Time deltaTime)
{
	GameEntity* selection = (*CurrentGame).m_selected_ui;
	GameEntity* previous_selection = selection;
	GameEntity* hovered = (*CurrentGame).m_hovered_ui;
	MouseAction& mouse_click = (*CurrentGame).m_mouse_click;

	//Get mouse & keyboard inputs
	(*CurrentGame).GetMouseInputs(deltaTime);

	//Reset flags
	(*CurrentGame).m_hovered_ui = NULL;
	if ((*CurrentGame).m_selected_ui != NULL && mouse_click == Mouse_LeftClick)
	{
		(*CurrentGame).m_selected_ui->m_selected = false;
		(*CurrentGame).m_selected_ui->m_shape_container.setOutlineColor((*CurrentGame).m_selected_ui->m_default_color);
		(*CurrentGame).m_selected_ui = NULL;
	}

	//UPDATING GAME ENTITIES

	//change of scale?
	UpdateTacticalScale();

	//Rooms
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

	//Weapons
	for (vector<Weapon*>::iterator it = m_warship->m_weapons.begin(); it != m_warship->m_weapons.end(); it++)
	{
		(*it)->Update(deltaTime);
	}

	//Enemy rooms
	if (m_tactical_ship != NULL)
	{
		//Rooms
		for (vector<Room*>::iterator it = m_tactical_ship->m_rooms.begin(); it != m_tactical_ship->m_rooms.end(); it++)
		{
			(*it)->Update(deltaTime);
		}

		//Rooms connexions
		for (vector<RoomConnexion*>::iterator it = m_tactical_ship->m_connexions.begin(); it != m_tactical_ship->m_connexions.end(); it++)
		{
			(*it)->Update(deltaTime);
		}

		//Crew movement
		for (vector<CrewMember*>::iterator it = m_tactical_ship->m_crew.begin(); it != m_tactical_ship->m_crew.end(); it++)
		{
			(*it)->Update(deltaTime);
		}
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

	//Fire weapon
	if (mouse_click == Mouse_RightClick && hovered != NULL && hovered->m_UI_type == UI_Weapon)
	{
		Weapon* weapon = (Weapon*)hovered;
		m_warship->FireWeapon(weapon, deltaTime);
	}

	//Bullets
	for (vector<Ammo*>::iterator it = (*CurrentGame).m_bullets.begin(); it != (*CurrentGame).m_bullets.end(); it++)
	{
		if ((*it)->m_can_be_seen == true)
		{
			(*it)->Update(deltaTime, m_warship->m_DMS);
		}
	}

	//boat
	m_warship->Update(deltaTime);

	//water tiles
	m_warship->m_tiles_can_be_seen.clear();
	for (vector<vector<WaterTile*> >::iterator it = m_warship->m_tile->m_zone->m_watertiles.begin(); it != m_warship->m_tile->m_zone->m_watertiles.end(); it++)
	{
		for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
		{
			//can be seen? no need to update other tiles because they won't be drawn anyway
			if (m_warship->CanViewWaterTile(*it2) && m_scale == Scale_Strategic)
			{
				(*it2)->m_can_be_seen = true;
				m_warship->m_tiles_can_be_seen.push_back(*it2);

				//position on "radar"
				(*it2)->UpdatePosition(m_warship->m_DMS);

				//selection
				if (selection == m_warship && (*it2)->m_type == Water_Empty)// && m_warship->m_destination == NULL
				{
					ostringstream ss;
					ss << (*it2)->m_coord_x << ", " << (*it2)->m_coord_y;
					(*it2)->m_text.setString(ss.str());
					(*it2)->GameEntity::Update(deltaTime);
				}
				else
				{
					(*it2)->GameEntity::Update(deltaTime);
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

	//other ships
	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		(*it)->Update(deltaTime, m_warship->m_DMS);
	}

	//island
	if (m_island != NULL)
	{
		//position on "radar"
		WaterTile* tileUpLeft = m_warship->m_tile->m_zone->m_watertiles[m_island->m_upcorner_x][m_island->m_upcorner_y];
		WaterTile* tileDownRight = m_warship->m_tile->m_zone->m_watertiles[m_island->m_upcorner_x + m_island->m_width - 1][m_island->m_upcorner_y - m_island->m_height + 1];
		float pos_x = 0.5f * (tileDownRight->m_position.x + tileUpLeft->m_position.x);
		float pos_y = 0.5f * (tileDownRight->m_position.y + tileUpLeft->m_position.y);
		m_island->m_position = sf::Vector2f(pos_x, pos_y);
	
		m_island->Update(deltaTime);

		if (m_island->m_seaport != NULL)
		{
			Seaport* port = m_island->m_seaport;

			port->m_position = port->m_tile->m_position;
			port->UpdatePosition();
		}
	}

	//Move order
	if (mouse_click == Mouse_RightClick && hovered != NULL && hovered->m_UI_type == UI_WaterTile)
	{
		WaterTile* tile = (WaterTile*)hovered;
		m_warship->SetSailsToWaterTile(tile);
	}

	//Clean bullets that cannot be seen anymore
	CleanOldBullets();
}

void Gameloop::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//PIRATES

	//background
	m_background->Draw((*CurrentGame).m_mainScreen);

	//room tiles
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

	//weapons
	for (vector<Weapon*>::iterator it = m_warship->m_weapons.begin(); it != m_warship->m_weapons.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//crew
	for (vector<CrewMember*>::iterator it = m_warship->m_crew.begin(); it != m_warship->m_crew.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//enemy rooms
	if (m_tactical_ship != NULL)
	{
		//room tiles
		for (vector<RoomTile*>::iterator it = (*CurrentGame).m_enemy_tiles.begin(); it != (*CurrentGame).m_enemy_tiles.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}

		//rooms
		for (vector<Room*>::iterator it = m_tactical_ship->m_rooms.begin(); it != m_tactical_ship->m_rooms.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}

		//doors
		for (vector<RoomConnexion*>::iterator it = m_tactical_ship->m_connexions.begin(); it != m_tactical_ship->m_connexions.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}

		//weapons
		for (vector<Weapon*>::iterator it = m_tactical_ship->m_weapons.begin(); it != m_tactical_ship->m_weapons.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}

		//crew
		for (vector<CrewMember*>::iterator it = m_tactical_ship->m_crew.begin(); it != m_tactical_ship->m_crew.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}
	}

	//WATER PART
	//boat
	if (m_scale == Scale_Strategic)
	{
		//water tiles
		vector<Island*> islands;
		for (vector<WaterTile*>::iterator it = m_warship->m_tiles_can_be_seen.begin(); it != m_warship->m_tiles_can_be_seen.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);

			if ((*it)->m_island != NULL)
			{
				islands.push_back((*it)->m_island);
			}
		}

		//islands and ports
		for (vector<Island*>::iterator it = islands.begin(); it != islands.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);

			//seaport
			if ((*it)->m_seaport != NULL && (*it)->m_seaport->m_tile->m_can_be_seen == true)
			{
				(*it)->m_seaport->Draw((*CurrentGame).m_mainScreen);
			}
		}

		//player boat
		m_warship->Draw((*CurrentGame).m_mainScreen);

		//enemy
		for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
		{
			if ((*it)->m_can_be_seen == true)
			{
				(*it)->Draw((*CurrentGame).m_mainScreen);
			}
		}
	}
	
	//Bullets
	for (vector<Ammo*>::iterator it = (*CurrentGame).m_bullets.begin(); it != (*CurrentGame).m_bullets.end(); it++)
	{
		(*it)->Draw((*CurrentGame).m_mainScreen);
	}

	//compass
	m_warship->m_compass.Draw((*CurrentGame).m_mainScreen, m_warship->m_angle);

	//Display
	(*CurrentGame).m_mainScreen.display();
	sf::Sprite temp((*CurrentGame).m_mainScreen.getTexture());
	temp.scale((*CurrentGame).m_scale_factor.x, (*CurrentGame).m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	(*CurrentGame).getMainWindow()->draw(temp);
}

void Gameloop::CleanOldBullets()
{
	vector<Ammo*> old_bullets;

	for (vector<Ammo*>::iterator it = (*CurrentGame).m_bullets.begin(); it != (*CurrentGame).m_bullets.end(); it++)
	{
		old_bullets.push_back(*it);
	}

	(*CurrentGame).m_bullets.clear();

	for (vector<Ammo*>::iterator it = old_bullets.begin(); it != old_bullets.end(); it++)
	{
		if ((*it)->m_can_be_seen == true)
		{
			(*CurrentGame).m_bullets.push_back(*it);
		}
	}
}

bool Gameloop::UpdateTacticalScale()
{
	if (m_scale == Scale_Tactical)
	{
		//todo: exit conditions
		if (0)
		{
			m_tactical_ship = NULL;
			m_scale = Scale_Strategic;
		}

		return true;
	}

	int xA = 0;
	int yA = 0;
	int xB = 0;
	int yB = 0;

	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		if ((*it)->m_can_be_seen == false)
		{
			continue;
		}

		//in range for combat?
		float posxA = m_warship->m_DMS.m_minute_x * 60.f + m_warship->m_DMS.m_second_x;
		float posxB = (*it)->m_DMS.m_minute_x * 60.f + (*it)->m_DMS.m_second_x;
		if (abs(posxA - posxB) > 2.f * NB_WATERTILE_SUBDIVISION)
		{
			continue;
		}

		float posyA = m_warship->m_DMS.m_minute_y * 60.f + m_warship->m_DMS.m_second_y;
		float posyB = (*it)->m_DMS.m_minute_y * 60.f + (*it)->m_DMS.m_second_y;
		if (abs(posyA - posyB) > 2.f * NB_WATERTILE_SUBDIVISION)
		{
			continue;
		}

		//if (m_warship->m_DMS.m_minute_x * 60.f + m_warship->m_DMS.m_seconds.x (*it)->m_DMS.m_minute_x + )

		//position X on tactical scale
		if (posxA < posxB)
		{
			xA = 0;
			xB = 2;
		}
		else if (posxA > posxB)
		{
			xA = 2;
			xB = 0;
		}
		else
		{
			xA = 1;
			xB = 1;
		}

		//position Y on tactical scale
		if (posyA < posyB)
		{
			yA = 0;
			yB = 2;
		}
		else if (posyA > posyB)
		{
			yA = 2;
			yB = 0;
		}
		else
		{
			yA = 1;
			yB = 1;
		}

		//attack from behind? = start 1 step closer
		if (m_warship->m_speed.x > 0 && (*it)->m_speed.x > 0)//going right
		{
			if (m_warship->m_speed.x > (*it)->m_speed.x)//ship A overspeeding ship B from behind
			{
				xB--;
			}
			else if (m_warship->m_speed.x < (*it)->m_speed.x)//ship B overspeeding ship A from behind
			{
				xA--;
			}
		}
		else if (m_warship->m_speed.x < 0 && (*it)->m_speed.x < 0)//going left
		{
			if (m_warship->m_speed.x < (*it)->m_speed.x)//ship A overspeeding ship B from behind
			{
				xB++;
			}
			else if (m_warship->m_speed.x > (*it)->m_speed.x)//ship B overspeeding ship A from behind
			{
				xA++;
			}
		}
		
		if (m_warship->m_speed.y > 0 && (*it)->m_speed.y > 0)//going up
		{
			if (m_warship->m_speed.y > (*it)->m_speed.y)//ship A overspeeding ship B from behind
			{
				yB--;
			}
			else if (m_warship->m_speed.y < (*it)->m_speed.y)//ship B overspeeding ship A from behind
			{
				yA--;
			}
		}
		else if (m_warship->m_speed.y < 0 && (*it)->m_speed.y < 0)//going down
		{
			if (m_warship->m_speed.y < (*it)->m_speed.y)//ship A overspeeding ship B from behind
			{
				yB++;
			}
			else if (m_warship->m_speed.y >(*it)->m_speed.y)//ship B overspeeding ship A from behind
			{
				yA++;
			}
		}

		m_tactical_ship = *it;
		m_scale = Scale_Tactical;

		//todo: stop ship and cancel destination on strategic scale


		break;
	}

	return (m_scale == Scale_Tactical);
}