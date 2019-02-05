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
	m_ships.push_back(new Ship(DMS_Coord{ 0, 13, 0, 0, 8, 0 }, Ship_FirstClass, Alliance_Enemy, "L'Esquif"));
	m_ships.push_back(new Ship(DMS_Coord{ 0, 16, 0, 0, 11, 0 }, Ship_FirstClass, Alliance_Enemy, "Le Goelan"));
	m_tactical_ship = NULL;

	m_scale = Scale_Strategic;

	m_pause_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_pause_text.setCharacterSize(50);
	m_pause_text.setColor(sf::Color::Black);
	m_pause_text.setString("P A U S E");
	m_pause_text.setPosition(sf::Vector2f(WINDOW_RESOLUTION_X * 0.5f, WINDOW_RESOLUTION_Y * 0.5f));
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
	(*CurrentGame).GetControllerInputs();

	//Pause
	if ((*CurrentGame).m_input_actions[Action_Pause] == Input_Tap)
	{
		(*CurrentGame).m_pause = !(*CurrentGame).m_pause;
	}

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
		int flood = 0;

		m_warship->UpdateCrewMembersCountPerRoom(*it);
		//(*it)->Update(deltaTime);
		(*it)->UpdatePosition();

		//Room tiles
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			if ((*it2)->m_flood > 0)
			{
				flood++;
			}

			(*it2)->m_shape_container.setFillColor(sf::Color::Black);

			if ((*it2)->m_pierced == true)
			{
				(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Pierced]);
			}
			else if ((*it2)->m_flood > 0)
			{
				(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Flood]);
			}

			if ((*it2)->m_system_tile != NULL)
			{
				if ((*it2)->m_crew != NULL && (*it2)->m_crew->m_tile == *it2)
				{
					(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Green_System]);
				}
				else
				{
					switch ((*it2)->m_system)
					{
						case System_Weapon:
						{
							(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Orange_System]);
							break;
						}
						case System_Navigation:
						{
							(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Cyan_System]);
							break;
						}
					}
				}
			}

			//crew move order (hover tile) feedback
			if (selection != NULL && selection->m_UI_type == UI_CrewMember)
			{
				(*it2)->Update(deltaTime);
			}
			else
			{
				(*it2)->m_shape_container.setOutlineColor((*it2)->m_default_color);
				(*it2)->m_shape_container.setOutlineThickness(-1.f);
			}
		}

		(*it)->m_is_flooded = flood == (*it)->m_tiles.size();
	}

	//Room connexions
	for (vector<RoomConnexion*>::iterator it = m_warship->m_connexions.begin(); it != m_warship->m_connexions.end(); it++)
	{
		(*it)->Update(deltaTime);
	}

	//Enemy rooms
	if (m_tactical_ship != NULL)
	{
		//Rooms
		bool one_room_hovered = false;
		for (vector<Room*>::iterator it = m_tactical_ship->m_rooms.begin(); it != m_tactical_ship->m_rooms.end(); it++)
		{
			int flood = 0;

			(*it)->UpdatePosition();

			//weapon selected? highlight targeted room
			bool room_hovered = false;
			if (selection != NULL && selection->m_UI_type == UI_Weapon && (*it)->IsHoveredByMouse() == true)
			{
				room_hovered = true;
				one_room_hovered = true;

				if (mouse_click == Mouse_RightClick)
				{
					Weapon* weapon = (Weapon*)selection;
					weapon->m_target_room = *it;
				}
			}
			
			//Room tiles
			for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
			{
				if ((*it2)->m_flood > 0)
				{
					flood++;
				}

				if (room_hovered == true)
				{
					(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Yellow_Target]);
				}
				else
				{
					(*it2)->m_shape_container.setFillColor(sf::Color::Black);

					if ((*it2)->m_pierced == true)
					{
						(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Pierced]);
					}
					else if ((*it2)->m_flood > 0)
					{
						(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Flood]);
					}
					else if ((*it2)->m_system_tile != NULL)//position of a weapon gunner?
					{
						if ((*it2)->m_crew != NULL && (*it2)->m_crew->m_tile == *it2)
						{
							(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Green_System]);//occupied by a crew
						}
						else
						{
							switch ((*it2)->m_system)
							{
								case System_Weapon:
								{
									(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Orange_System]);
									break;
								}
								case System_Navigation:
								{
									(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Cyan_System]);
									break;
								}
							}
						}
					}
				}
			}

			(*it)->m_is_flooded = flood == (*it)->m_tiles.size();
		}

		//by default, if no room is highlighted, we highlight the current targeted room. Else, we keep the current targeted room highlighted in grey while hovering other rooms in yellow
		if (selection != NULL && selection->m_UI_type == UI_Weapon)
		{
			Weapon* weapon = (Weapon*)selection;
			if (weapon->m_target_room != NULL)
			{
				if (one_room_hovered == false)
				{
					for (vector<RoomTile*>::iterator it2 = weapon->m_target_room->m_tiles.begin(); it2 != weapon->m_target_room->m_tiles.end(); it2++)
					{
						(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Yellow_Target]);
					}
				}
				else if (weapon->m_target_room->IsHoveredByMouse() == false)
				{
					for (vector<RoomTile*>::iterator it2 = weapon->m_target_room->m_tiles.begin(); it2 != weapon->m_target_room->m_tiles.end(); it2++)
					{
						(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Grey_Target]);
					}
				}
			}
		}

		//Rooms connexions
		for (vector<RoomConnexion*>::iterator it = m_tactical_ship->m_connexions.begin(); it != m_tactical_ship->m_connexions.end(); it++)
		{
			(*it)->Update(deltaTime);
		}
	}

	//Crew movement (and removing the dead);
	vector<CrewMember*> old_crew;
	for (vector<CrewMember*>::iterator it = m_warship->m_crew.begin(); it != m_warship->m_crew.end(); it++)
	{
		old_crew.push_back(*it);
	}
	m_warship->m_crew.clear();
	for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
	{
		if ((*it)->m_health > 0)
		{
			(*it)->Update(deltaTime);

			//create or update HUD for crew details
			if ((*it)->m_hovered == true && m_warship->m_crew_interface.m_crew != *it)
			{
				m_warship->m_crew_interface.Init(*it);
			}
			else if ((*it)->m_selected == true && m_warship->m_crew_interface.m_crew != *it && m_warship->m_crew_interface.m_crew->m_hovered == false)
			{
				m_warship->m_crew_interface.Init(*it);
			}
		}

		if ((*it)->m_health > 0)//second check because he could drown during the update
		{
			m_warship->m_crew.push_back(*it);
		}
		else
		{
			m_warship->m_nb_crew--;
			if (m_warship->m_crew_interface.m_crew == *it)//remove from interface before destroying object
			{
				m_warship->m_crew_interface.m_crew = NULL;
			}
			delete *it;
		}
	}

	//enemy crew movement
	if (m_tactical_ship != NULL)
	{
		old_crew.clear();
		for (vector<CrewMember*>::iterator it = m_tactical_ship->m_crew.begin(); it != m_tactical_ship->m_crew.end(); it++)
		{
			old_crew.push_back(*it);
		}
		m_tactical_ship->m_crew.clear();
		for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
		{
			if ((*it)->m_health > 0)
			{
				(*it)->Update(deltaTime);

				//Crew AI
				if ((*it)->m_destination == NULL)
				{
					//first look into current room if there is any interesting task to do. Then we'll do the same search across all rooms
					RoomTile* destination = NULL;

					//not already busy reparing?
					if (((*it)->m_tile != NULL && (*it)->m_tile->m_crew == *it && (*it)->m_tile->m_pierced == true) == false)
					{
						//Hull to repair?
						for (vector<RoomTile*>::iterator it2 = (*CurrentGame).m_enemy_tiles.begin(); it2 != (*CurrentGame).m_enemy_tiles.end(); it2++)
						{
							if ((*it2)->m_pierced == true && (*it2)->m_crew == NULL)
							{
								destination = (*it2);
								break;
							}
						}

						//Weapon to fire
						if (destination == NULL && ((*it)->m_tile->m_system_tile != NULL && (*it)->m_tile->m_crew == *it) == false)//not already busy as a weapon gunner?
						{
							for (vector<RoomTile*>::iterator it2 = (*CurrentGame).m_enemy_tiles.begin(); it2 != (*CurrentGame).m_enemy_tiles.end(); it2++)
							{
								if ((*it2)->m_operator_tile != NULL && (*it2)->m_operator_tile->m_crew == NULL && (*it2)->m_weapon != NULL && (*it2)->m_weapon->m_health > 0)
								{
									destination = (*it2)->m_operator_tile;
									break;
								}
							}
						}

						//Give move order
						if (destination != NULL)
						{
							//tile is free?
							if (destination->m_crew == NULL && destination->m_weapon == NULL)
							{
								//book new destination
								destination->m_crew = *it;

								//assign destination for pathfind
								(*it)->m_destination = destination;
							}
						}
					}
				}
			}

			//remove the dead
			if ((*it)->m_health > 0)
			{
				m_tactical_ship->m_crew.push_back(*it);
			}
			else
			{
				m_tactical_ship->m_nb_crew--;
				delete *it;
			}
		}
	}
	
	//Fire weapon
	//if (m_scale == Scale_Tactical)
	//{
	//	if (mouse_click == Mouse_RightClick && hovered != NULL && hovered->m_UI_type == UI_Weapon)
	//	{
	//		Weapon* weapon = (Weapon*)hovered;
	//		m_warship->FireWeapon(weapon, deltaTime, m_tactical_ship);
	//	}
	//}

	//Player Weapons
	for (vector<Weapon*>::iterator it = m_warship->m_weapons.begin(); it != m_warship->m_weapons.end(); it++)
	{
		(*it)->Update(deltaTime);

		if (m_tactical_ship != NULL)
		{
			if ((*it)->CanFire() == true)
			{
				if ((*it)->m_target_room == NULL)
				{
					//do nothing
				}
				else
				{
					m_warship->FireWeapon(*it, deltaTime, m_tactical_ship);
				}
			}
		}
	}
	if (m_tactical_ship != NULL)
	{
	//Enemy weapons
		for (vector<Weapon*>::iterator it = m_tactical_ship->m_weapons.begin(); it != m_tactical_ship->m_weapons.end(); it++)
		{
			(*it)->Update(deltaTime);
			if ((*it)->CanFire() == true)
			{
				//randomly change target sometimes
				if ((*it)->m_target_room == NULL || ((*it)->m_rof_timer <= 0 && RandomizeFloatBetweenValues(sf::Vector2f(0.f, 1.f)) < AI_CHANGE_TARGETROOM_PERCENTAGE))
				{
					int r = RandomizeIntBetweenValues(0, m_warship->m_rooms.size() - 1);
					(*it)->m_target_room = m_warship->m_rooms[r];
				}

				m_tactical_ship->FireWeapon(*it, deltaTime, m_warship);
			}
		}
	}

	//ACTIONS
	//Crew move to room
	if (mouse_click == Mouse_RightClick && selection != NULL && selection->m_UI_type == UI_CrewMember && hovered != NULL && hovered->m_UI_type == UI_RoomTile)
	{
		CrewMember* crew = (CrewMember*)selection;
		RoomTile* destination = (RoomTile*)hovered;

		//tile is free?
		if (destination->m_crew == NULL && destination->m_weapon == NULL)
		{
			//free previous destination's booking (if any)
			if (crew->m_destination != NULL)
			{
				crew->m_destination->m_crew = NULL;
			}

			//book new destination
			destination->m_crew = crew;

			//assign destination for pathfind
			crew->m_destination = destination;
		}
	}
	//Close/open doors
	if (mouse_click == Mouse_LeftClick && hovered != NULL && hovered->m_UI_type == UI_Connexion)
	{
		RoomConnexion* connexion = (RoomConnexion*)hovered;
		if (connexion->m_ship == m_warship)
		{
			connexion->SetLock(!connexion->m_locked);
		}
	}

	//Bullets + cleaning old bullets
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
			(*it)->Update(deltaTime);

			//Hit effects
			if ((*it)->m_phase == Shoot_Hit)
			{
				//compute hit tiles
				int x = (*it)->m_target_tile->m_coord_x;
				int y = (*it)->m_target_tile->m_coord_y;

				int radius = (*it)->m_radius - 1;

				int a = x - radius;
				int b = x + radius;
				for (int i = x - radius; i < x + radius + 1; i++)
				{
					for (int j = y - radius; j < y + radius + 1; j++)
					{
						if (i >= 0 && i <= (*it)->m_target_ship->m_rooms_size.x - 1 && j >= 0 && j <= (*it)->m_target_ship->m_rooms_size.y - 1 && (*it)->m_target_ship->m_tiles[i][j] != NULL)
						{
							//"boom": apply bullet damage and side effects
							RoomTile* tile = (*it)->m_target_ship->m_tiles[i][j];

							//damage
							(*it)->m_target_ship->m_health -= Min((*it)->m_damage, (*it)->m_target_ship->m_health);
							tile->m_health -= Min((*it)->m_damage, tile->m_health);

							//damage to weapon
							if (tile->m_weapon != NULL)
							{
								Weapon* weapon = tile->m_weapon;
								weapon->m_health -= Min((*it)->m_damage, weapon->m_health);
								if (weapon->m_health == 0)
								{
									weapon->setColor(sf::Color::Red);
								}
							}

							//piercing hull
							if (tile->m_hull != Hull_None && tile->m_pierced == false && tile->m_health == 0 && tile->m_weapon == NULL)//cannot pierce a tile where a weapon is standing
							{
								tile->m_pierced = true;
								if (tile->m_crew != NULL)
								{
									tile->m_crew->m_repair_timer = HULL_REPAIR_TIMER;
								}
							}

							//killing crew
							for (vector<CrewMember*>::iterator it2 = (*it)->m_target_ship->m_crew.begin(); it2 != (*it)->m_target_ship->m_crew.end(); it2++)
							{
								float xA1 = tile->getPosition().x - tile->m_size.x * 0.5f;
								float xA2 = tile->getPosition().x + tile->m_size.x * 0.5f;
								float yA1 = tile->getPosition().y - tile->m_size.y * 0.5f;
								float yA2 = tile->getPosition().y + tile->m_size.y * 0.5f;
								float xB1 = (*it2)->getPosition().x - (*it2)->m_size.x * 0.5f;
								float xB2 = (*it2)->getPosition().x + (*it2)->m_size.x * 0.5f;
								float yB1 = (*it2)->getPosition().y - (*it2)->m_size.y * 0.5f;
								float yB2 = (*it2)->getPosition().y + (*it2)->m_size.y * 0.5f;

								//collision?
								if ((xA1 < xB1 && xB1 < xA2 && yB1 > yA1 && yB1 < yA2)
									|| (xA1 < xB2 && xB2 < xA2 && yB1 > yA1 && yB1 < yA2)
									|| (xA1 < xB1 && xB1 < xA2 && yB2 > yA1 && yB2 < yA2)
									|| (xA1 < xB2 && xB2 < xA2 && yB2 > yA1 && yB2 < yA2))
								{
									if ((*it2)->m_health > 0)
									{
										(*it2)->m_health--;
									}
								}
							}
						}
					}
				}
			}
		}
		else
		{
			delete *it;
		}
	}

	//FX + clean old FX
	vector<FX*> old_FX;
	for (vector<FX*>::iterator it = (*CurrentGame).m_FX.begin(); it != (*CurrentGame).m_FX.end(); it++)
	{
		old_FX.push_back(*it);
	}
	(*CurrentGame).m_FX.clear();
	for (vector<FX*>::iterator it = old_FX.begin(); it != old_FX.end(); it++)
	{
		if ((*it)->m_can_be_seen == true)
		{
			(*CurrentGame).m_FX.push_back(*it);
			(*it)->Update(deltaTime);
		}
		else
		{
			delete *it;
		}
	}

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
					if ((*it2)->m_type == Water_Empty)
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
					else
					{
						(*it2)->GameEntity::UpdatePosition();
					}
				}
			}
			else
			{
				(*it2)->m_can_be_seen = false;
			}
		}
	}

	//boat
	m_warship->Update(deltaTime, m_scale == Scale_Tactical);

	//other ships
	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		(*it)->Update(deltaTime, m_warship->m_DMS, m_scale == Scale_Tactical);
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
}

void Gameloop::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//PIRATES

	//background
	m_background->Draw((*CurrentGame).m_mainScreen);

	//room tiles
	GameEntity* focused_room_tile = NULL;
	for (vector<RoomTile*>::iterator it = (*CurrentGame).m_tiles.begin(); it != (*CurrentGame).m_tiles.end(); it++)
	{
		if (((*CurrentGame).m_hovered_ui != NULL && (*CurrentGame).m_hovered_ui == *it) || ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui == *it))
		{
			focused_room_tile = *it;
		}
		else
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}
	}
	if (focused_room_tile != NULL)
	{
		focused_room_tile->Draw((*CurrentGame).m_mainScreen);
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

	//lifebars & rofbars
	for (vector<Weapon*>::iterator it = m_warship->m_weapons.begin(); it != m_warship->m_weapons.end(); it++)
	{
		if (m_scale == Scale_Tactical)
		{
			(*it)->m_rofbar->Draw((*CurrentGame).m_mainScreen);
		}

		if ((*it)->m_health < (*it)->m_health_max)
		{
			(*it)->m_lifebar->Draw((*CurrentGame).m_mainScreen);
		}
	}
	for (vector<CrewMember*>::iterator it = m_warship->m_crew.begin(); it != m_warship->m_crew.end(); it++)
	{
		if ((*it)->m_health < (*it)->m_health_max)
		{
			(*it)->m_lifebar->Draw((*CurrentGame).m_mainScreen);
		}
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

		//lifebars & rofbars
		if (m_scale == Scale_Tactical)
		{
			for (vector<Weapon*>::iterator it = m_tactical_ship->m_weapons.begin(); it != m_tactical_ship->m_weapons.end(); it++)
			{
				(*it)->m_rofbar->Draw((*CurrentGame).m_mainScreen);
				if ((*it)->m_health < (*it)->m_health_max)
				{
					(*it)->m_lifebar->Draw((*CurrentGame).m_mainScreen);
				}
			}
			for (vector<CrewMember*>::iterator it = m_tactical_ship->m_crew.begin(); it != m_tactical_ship->m_crew.end(); it++)
			{
				if ((*it)->m_health < (*it)->m_health_max)
				{
					(*it)->m_lifebar->Draw((*CurrentGame).m_mainScreen);
				}
			}
		}
	}

	//WATER PART
	//boat
	if (m_scale == Scale_Strategic)
	{
		//water tiles
		vector<Island*> islands;
		GameEntity* focused_water_tile = NULL;
		for (vector<WaterTile*>::iterator it = m_warship->m_tiles_can_be_seen.begin(); it != m_warship->m_tiles_can_be_seen.end(); it++)
		{
			if (((*CurrentGame).m_hovered_ui != NULL && (*CurrentGame).m_hovered_ui == *it) || ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui == *it))
			{
				focused_water_tile = *it;
			}
			else
			{
				(*it)->Draw((*CurrentGame).m_mainScreen);
			}

			if ((*it)->m_island != NULL)
			{
				islands.push_back((*it)->m_island);
			}
		}
		if (focused_water_tile != NULL)
		{
			focused_water_tile->Draw((*CurrentGame).m_mainScreen);
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

		//compass
		m_warship->m_compass.Draw((*CurrentGame).m_mainScreen, m_warship->m_angle);

		//enemy
		for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
		{
			if ((*it)->m_can_be_seen == true)
			{
				(*it)->Draw((*CurrentGame).m_mainScreen);
			}
		}
	}
	//Tactical scale only
	else
	{
		//Bullets
		for (vector<Ammo*>::iterator it = (*CurrentGame).m_bullets.begin(); it != (*CurrentGame).m_bullets.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}

		//combat interface
		m_warship->m_combat_interface.Draw((*CurrentGame).m_mainScreen);
	}

	//crew interface
	if (((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui->m_UI_type == UI_CrewMember) || ((*CurrentGame).m_hovered_ui != NULL && (*CurrentGame).m_hovered_ui->m_UI_type == UI_CrewMember))
	{
		m_warship->m_crew_interface.Draw((*CurrentGame).m_mainScreen);
	}
	
	//FX
	for (vector<FX*>::iterator it = (*CurrentGame).m_FX.begin(); it != (*CurrentGame).m_FX.end(); it++)
	{
		(*it)->FX::Draw((*CurrentGame).m_mainScreen);
	}

	//PAUSE
	if ((*CurrentGame).m_pause == true)
	{
		(*CurrentGame).m_mainScreen.draw(m_pause_text);
	}

	//Display
	(*CurrentGame).m_mainScreen.display();
	sf::Sprite temp((*CurrentGame).m_mainScreen.getTexture());
	temp.scale((*CurrentGame).m_scale_factor.x, (*CurrentGame).m_scale_factor.y);
	temp.setPosition(sf::Vector2f(0, 0));
	(*CurrentGame).getMainWindow()->draw(temp);
}

bool Gameloop::UpdateTacticalScale()
{
	if (m_scale == Scale_Tactical)
	{
		//win conditions
		if (m_tactical_ship->m_health < m_tactical_ship->m_health_max * 0.9f || m_tactical_ship->m_flood > m_tactical_ship->m_flood_max * 0.5f || m_tactical_ship->m_nb_crew < m_tactical_ship->m_nb_crew_max * 0.3)
		{
			//delete enemy from ships existing
			vector<Ship*> old_ships;
			for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
			{
				old_ships.push_back(*it);
			}
			m_ships.clear();
			for (vector<Ship*>::iterator it = old_ships.begin(); it != old_ships.end(); it++)
			{
				if (*it != m_tactical_ship)
				{
					m_ships.push_back(*it);
				}
				else
				{
					delete m_tactical_ship;
					m_tactical_ship = NULL;
				}
			}

			//restore destroyed weapons
			m_warship->RestoreWeaponsHealth();

			//switch back to strategic scale
			m_scale = Scale_Strategic;
		}
		//lose conditions
		else if (m_warship->m_health < m_warship->m_health_max * 0.3f || m_warship->m_flood > m_warship->m_flood_max * 0.5f)
		{
			//restore health
			m_warship->RestoreHealth();

			//switch back to strategic scale
			m_scale = Scale_Strategic;
			m_tactical_ship = NULL;
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
		m_warship->m_combat_interface.Init(m_warship, *it);
		m_scale = Scale_Tactical;
		break;
	}

	if (m_scale == Scale_Tactical)
	{
		//stop strategic movement
		m_warship->m_destination = NULL;
		m_warship->m_speed = sf::Vector2f(0, 0);
		m_warship->m_current_path.clear();

		m_warship->InitCombat();//init cooldowns
		m_tactical_ship->BuildShip();//generate enemy ship's rooms, crew and weapons
		m_tactical_ship->InitCombat();//init cooldowns

		//fill enemy ship room tiles for pathfind
		(*CurrentGame).m_enemy_tiles.clear();
		for (vector<Room*>::iterator it = m_tactical_ship->m_rooms.begin(); it != m_tactical_ship->m_rooms.end(); it++)
		{
			for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
			{
				(*CurrentGame).m_enemy_tiles.push_back(*it2);
			}
		}
	}

	return (m_scale == Scale_Tactical);
}