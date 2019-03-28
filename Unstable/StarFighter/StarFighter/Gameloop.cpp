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
	if (LoadPlayerData(m_warship) == 0)
	{
		m_warship->Init();
		SavePlayerData(m_warship);
	}
	
	m_resources_interface.Init(m_warship);

	m_ships.push_back(m_warship);
	m_ships.push_back(new Ship(DMS_Coord{ 0, 13, 0, 0, 8, 0 }, Ship_FirstClass, Alliance_Enemy, "L'Esquif"));
	m_ships.push_back(new Ship(DMS_Coord{ 0, 16, 0, 0, 11, 0 }, Ship_FirstClass, Alliance_Enemy, "Le Goelan"));
	m_tactical_ship = NULL;

	m_scale = Scale_Strategic;
	m_menu = Menu_None;

	m_pause_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	m_pause_text.setCharacterSize(50);
	m_pause_text.setColor(sf::Color::Black);
	m_pause_text.setString("P A U S E");
	m_pause_text.setPosition(sf::Vector2f(WINDOW_RESOLUTION_X * 0.5f, WINDOW_RESOLUTION_Y * 0.5f));

	//Contextual orders
	m_contextual_order = new ContextualOrder();
}

Gameloop::~Gameloop()
{
	delete m_background;
	
	for (vector<Island*>::iterator it = m_islands.begin(); it != m_islands.end(); it++)
	{
		delete (*it);
	}

	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		delete (*it);
	}

	delete m_contextual_order;

	m_resources_interface.Destroy();
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
	m_islands.push_back(new Island(8, 9, 2, 2, 0, 0));
}

bool Gameloop::GetSaveOrLoadInputs()
{
	if ((*CurrentGame).m_input_actions[Action_Saving] == Input_Tap && (*CurrentGame).m_window_has_focus == true)
	{
		SavePlayerData(m_warship);
		return true;
	}
	else if (m_resources_interface.m_save_button->IsHoveredByMouse() == true && (*CurrentGame).m_mouse_click == Mouse_LeftClick && (*CurrentGame).m_window_has_focus == true)
	{
		SavePlayerData(m_warship);
		return true;
	}
	else if (m_resources_interface.m_load_button->IsHoveredByMouse() == true && (*CurrentGame).m_mouse_click == Mouse_LeftClick && (*CurrentGame).m_window_has_focus == true)
	{
		LoadPlayerData(m_warship);
		return true;
	}

	return false;
}

void Gameloop::Update(sf::Time deltaTime)
{
	GameEntity* selection = (*CurrentGame).m_selected_ui;
	GameEntity* previous_selection = selection;
	GameEntity* hovered = (*CurrentGame).m_hovered_ui;
	MouseAction& mouse_click = (*CurrentGame).m_mouse_click;

	m_contextual_order->SetContextualOrder(Order_None, m_contextual_order->m_position, true);

	//Get mouse & keyboard inputs
	(*CurrentGame).GetMouseInputs(deltaTime);
	(*CurrentGame).GetControllerInputs();

	//Saving?
	GetSaveOrLoadInputs();

	//Pause
	if ((*CurrentGame).m_input_actions[Action_Pause] == Input_Tap && (*CurrentGame).m_window_has_focus == true)
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

	bool warship_is_in_port = m_warship->m_seaport != NULL;
	
	//UPDATING GAME ENTITIES

	//change of scale?
	UpdateTacticalScale();

	//Water tiles
	m_warship->m_tiles_can_be_seen.clear();
	if (m_scale == Scale_Strategic)
	{
		for (vector<vector<WaterTile*> >::iterator it = m_warship->m_tile->m_zone->m_watertiles.begin(); it != m_warship->m_tile->m_zone->m_watertiles.end(); it++)
		{
			for (vector<WaterTile*>::iterator it2 = it->begin(); it2 != it->end(); it2++)
			{
				//can be seen? no need to update other tiles because they won't be drawn anyway
				if (m_warship->CanViewWaterTile(*it2))
				{
					(*it2)->m_can_be_seen = true;
					m_warship->m_tiles_can_be_seen.push_back(*it2);

					//position on "radar"
					(*it2)->UpdatePosition(m_warship->m_DMS);

					//selection
					if (selection == m_warship && (*it2)->m_type == Water_Empty)// && m_warship->m_destination == NULL
					{
						//display tile coords
						//ostringstream ss;
						//ss << (*it2)->m_coord_x << ", " << (*it2)->m_coord_y;
						//(*it2)->m_text.setString(ss.str());
						(*it2)->GameEntity::Update(deltaTime);
					}
					else
					{
						(*it2)->GameEntity::UpdatePosition();

						if ((*it2)->m_type == Water_Empty)
						{
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
				else
				{
					(*it2)->m_can_be_seen = false;
				}
			}
		}
	}

	//Islands
	for (vector<Island*>::iterator it = m_islands.begin(); it != m_islands.end(); it++)
	{
		//position on "radar"
		WaterTile* tileUpLeft = m_warship->m_tile->m_zone->m_watertiles[(*it)->m_upcorner_x][(*it)->m_upcorner_y];
		WaterTile* tileDownRight = m_warship->m_tile->m_zone->m_watertiles[(*it)->m_upcorner_x + (*it)->m_width - 1][(*it)->m_upcorner_y - (*it)->m_height + 1];
		float pos_x = 0.5f * (tileDownRight->m_position.x + tileUpLeft->m_position.x);
		float pos_y = 0.5f * (tileDownRight->m_position.y + tileUpLeft->m_position.y);
		(*it)->m_position = sf::Vector2f(pos_x, pos_y);

		(*it)->UpdatePosition();

		if ((*it)->m_seaport != NULL)
		{
			Seaport* port = (*it)->m_seaport;

			port->m_position = port->m_tile->m_position;
			port->UpdatePosition();
		}
	}

	//Canceling weapon target
	if (selection != NULL && selection->m_UI_type == UI_Weapon && hovered == NULL)
	{
		if (mouse_click == Mouse_RightClick)
		{
			Weapon* weapon = (Weapon*)selection;
			weapon->m_target_room = NULL;
		}
	}

	//Ships update
	int shipsVectorSize = m_ships.size();
	for (int i = 0; i < shipsVectorSize; i++)
	{
		Ship* ship = m_ships[i];

		//Strategical scale update
		if (m_scale == Scale_Strategic)
		{
			ship->UpdateStrategical(deltaTime);

			if (i == 0)
			{
				//Player ship UI
				ostringstream ss;
				ss << "\n\n\n";
				ss << ship->m_DMS.m_degree_y << "°" << ship->m_DMS.m_minute_y << "' " << (int)ship->m_DMS.m_second_y << "\"\N";
				ss << "\n";
				ss << ship->m_DMS.m_degree_x << "°" << ship->m_DMS.m_minute_x << "' " << (int)ship->m_DMS.m_second_x << "\"\E";
				ship->m_text.setString(ss.str());
	
				//Orders
				if (ship->m_selected == true && hovered != NULL && hovered->m_UI_type == UI_WaterTile)
				{
					WaterTile* tile_hovered = (WaterTile*)hovered;

					//Hovering a water tile that is not our current tile?
					if (WaterTile::SameDMS(m_warship->m_DMS, tile_hovered->m_DMS) == false)
					{
						//Scan all enemy ships to see if we're in range for combat with anyone
						Ship* ship_in_combat_range = NULL;
						for (vector<Ship*>::iterator it3 = m_ships.begin(); it3 != m_ships.end(); it3++)
						{
							if ((*it3) == m_warship || (*it3)->m_can_be_seen == false)
							{
								continue;
							}

							ship_in_combat_range = IsDMSInCombatRange(tile_hovered->m_DMS, (*it3)->m_DMS);
							if (ship_in_combat_range != NULL)
							{
								break;
							}
						}

						//Contextual orders' feedback
						if (m_menu == Menu_None && m_warship->m_speed == sf::Vector2f(0, 0))
						{
							int cost = m_warship->GetShortestPathLength(m_warship->m_tile, tile_hovered);
							
							if (ship_in_combat_range == NULL)
							{
								if (tile_hovered->m_seaport == NULL)
								{
									m_contextual_order->SetContextualOrder(Order_Sail, tile_hovered->m_position, true, cost);
								}
								else
								{
									m_contextual_order->SetContextualOrder(Order_Dock, tile_hovered->m_position, true, cost);
								}
							}
							else
							{
								m_contextual_order->SetContextualOrder(Order_Engage, tile_hovered->m_position, true, cost);
							}

							//Sail order
							if (mouse_click == Mouse_RightClick && tile_hovered->m_type == Water_Empty)
							{
								ship->SetSailsToWaterTile(tile_hovered);
								ship->PayUpkeepCost(cost);
							}
						}
					}
				}
			}
			else
			{
				//other ships update their position and visibility respect to player ship
				ship->m_can_be_seen = ship->m_tile->m_can_be_seen;
				ship->UpdatePosition(m_warship->m_DMS);

				//todo: AI strategical movement
			}

			ship->GameEntity::Update(deltaTime);
		}

		//Tactical scale update (and player)
		if (ship != m_warship && ship != m_tactical_ship)
		{
			continue;
		}

		if (ship->m_sinking_timer <= 0)
		{
			//Rooms
			Room* room_hovered = NULL;
			for (vector<Room*>::iterator it = ship->m_rooms.begin(); it != ship->m_rooms.end(); it++)
			{
				int flood = 0;
				(*it)->UpdatePosition();

				//weapon selected and hovering enemy room? highlight targeted room
				if (selection != NULL && selection->m_UI_type == UI_Weapon && (hovered == NULL || hovered->m_UI_type != UI_Connexion) && (*it)->m_alliance == Alliance_Enemy && (*it)->IsHoveredByMouse() == true && (*it)->m_type != Room_PrisonCell)//prison cells can't be targeted
				{
					room_hovered = *it;
				}

				//Room tiles
				for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
				{
					if ((*it2)->m_flood > 0)
					{
						flood++;
					}

					UpdateRoomTileFeedback(*it2, deltaTime, ship);

					//crew move order feedback (hovering tile)
					if (i == 0 && ship->m_is_fleeing == false)
					{
						if (selection != NULL && selection->m_UI_type == UI_CrewMember)
						{
							(*it2)->Update(deltaTime);

							//contextuel order feedback
							CrewMember* crew = (CrewMember*)selection;
							if ((*it2)->IsHoveredByMouse() == true && crew->m_alliance == (*it2)->m_room->m_alliance)
							{
								UpdateContextualOrderFeedback(crew, *it2);
							}
						}
						else
						{
							(*it2)->m_shape_container.setOutlineColor((*it2)->m_default_color);
							(*it2)->m_shape_container.setOutlineThickness(-1.f);
							(*it2)->UpdatePosition();
						}
					}
					else
					{
						(*it2)->UpdatePosition();
					}
				}

				(*it)->m_is_flooded = flood == (*it)->m_tiles.size();
			}

			//Update weapon's target room and feedbacks
			if (selection != NULL && selection->m_UI_type == UI_Weapon)
			{
				Weapon* weapon = (Weapon*)selection;
				UpdateTargetRoom(weapon, room_hovered);
			}

			//Room connexions
			for (vector<RoomConnexion*>::iterator it = ship->m_connexions.begin(); it != ship->m_connexions.end(); it++)
			{
				if (i == 0 && ship->m_is_fleeing == false)
				{
					(*it)->Update(deltaTime);
					if ((*it)->IsHoveredByMouse() == true && (*it)->m_destroyed == false && (*it)->m_tiles.first->m_room->m_alliance == Alliance_Player)
					{
						m_contextual_order->SetContextualOrder((*it)->m_is_locked == true ? Order_OpenRoomConnexion : Order_CloseRoomConnexion, (*it)->m_shape_container.getPosition(), true);
					}
				}
				else
				{
					(*it)->UpdatePosition();
				}
			}

			//Close/open connexion
			if (i == 0 && mouse_click == Mouse_LeftClick && hovered != NULL && hovered->m_UI_type == UI_Connexion && ship->m_is_fleeing == false)
			{
				RoomConnexion* connexion = (RoomConnexion*)hovered;
				if (connexion->m_ship == ship)
				{
					connexion->SetLock(!connexion->m_is_locked);
				}
			}

			//Crew update (and reomving the dead)
			for (int j = 0; j < 2; j++)
			{
				vector<CrewMember*> old_crew;
				for (vector<CrewMember*>::iterator it = ship->m_crew[j].begin(); it != ship->m_crew[j].end(); it++)
				{
					old_crew.push_back(*it);
				}
				ship->m_crew[j].clear();

				for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
				{
					//prisoners vs non-prisoners opponent?
					if ((*it)->m_melee_opponent == NULL)
					{
						for (vector<CrewMember*>::iterator it2 = ship->m_crew[(j + 1) % 2].begin(); it2 != ship->m_crew[(j + 1) % 2].end(); it2++)
						{
							if (Room::IsConnectedToRoomTile((*it)->m_tile, (*it2)->m_tile) == true)
							{
								(*it)->m_melee_opponent = *it2;
								if ((*it2)->m_melee_opponent == NULL)
								{
									(*it2)->m_melee_opponent = *it;
								}
								break;
							}
						}
					}

					//opponent catch us on melee?
					if ((*it)->m_melee_opponent != NULL)
					{
						//cancel movement
						if ((*it)->m_destination != NULL)
						{
							(*it)->m_destination->m_crew = NULL;
						}
						(*it)->m_destination = NULL;
						(*it)->m_speed = sf::Vector2f(0, 0);

						//fight
						(*it)->UpdateMelee(deltaTime);

						//update feedbacks and life bar
						(*it)->GameEntity::Update(deltaTime);
						(*it)->UpdateLifeBar();
					}
					else
					{
						//update: crew movement/heal/repair
						if (ship->m_is_fleeing == false)
						{
							(*it)->Update(deltaTime);
							if (j == 1)
							{
								//prisoners only: escape and roam if the prison cell is open
								ship->UpdatePrisonerEscape(*it, deltaTime);
							}
						}
						else
						{
							(*it)->UpdatePosition();
						}
					}

					if (ship->m_is_fleeing == false)
					{
						if (i == 0)
						{
							//create or update HUD for crew details
							if ((*it)->m_hovered == true && m_warship->m_crew_interface.m_crew != *it)
							{
								m_warship->m_crew_interface.Destroy();
								m_warship->m_crew_interface.Init(*it);
							}
							else if ((*it)->m_selected == true && m_warship->m_crew_interface.m_crew != *it && (m_warship->m_crew_interface.m_crew == NULL || m_warship->m_crew_interface.m_crew->m_hovered == false))
							{
								m_warship->m_crew_interface.Destroy();
								m_warship->m_crew_interface.Init(*it);
							}
							else if (m_warship->m_crew_interface.m_crew == *it && (*it)->m_selected == false && (*it)->m_hovered == false)
							{
								m_warship->m_crew_interface.Destroy();
							}

							if (m_warship->m_crew_interface.m_crew == *it)
							{
								m_warship->m_crew_interface.Update();
							}
						}
						else
						{
							//Enemy AI
							UpdateAICrew(*it);
						}
					}

					if ((*it)->m_health > 0)
					{
						ship->m_crew[j].push_back(*it);
					}
					else
					{
						ship->m_nb_crew--;
						if ((*CurrentGame).m_selected_ui == *it)
						{
							(*CurrentGame).m_selected_ui = NULL;
						}
						if ((*CurrentGame).m_hovered_ui == *it)
						{
							(*CurrentGame).m_hovered_ui = NULL;
						}

						if (i == 0)
						{
							Warship* warship = (Warship*)ship;
							if (warship->m_crew_interface.m_crew == *it)
							{
								m_warship->m_crew_interface.Destroy();
							}
						}

						delete *it;
					}
				}
			}

			//Move order for player's crew
			if (i == 0 && mouse_click == Mouse_RightClick && selection != NULL && selection->m_UI_type == UI_CrewMember && hovered != NULL && hovered->m_UI_type == UI_RoomTile && ship->m_is_fleeing == false)
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

			//Weapons
			if (m_scale == Scale_Tactical)
			{
				for (vector<Weapon*>::iterator it = ship->m_weapons.begin(); it != ship->m_weapons.end(); it++)
				{
					if (ship->m_is_fleeing == false)
					{
						(*it)->Update(deltaTime);

						Ship* enemy_ship = i == 0 ? m_tactical_ship : m_warship;
						if (enemy_ship != NULL && enemy_ship->m_is_fleeing == false && enemy_ship->m_sinking_timer <= 0)
						{
							if ((*it)->m_target_room == NULL && i == 1)
							{
								//AI choosing a target for each weapon
								UpdateAITargetRoom(*it);
							}

							if ((*it)->CanFire() == true && (*it)->m_target_room != NULL && (*CurrentGame).m_pause == false)
							{
								if (ship->FireWeapon(*it, deltaTime, enemy_ship) == true && i == 1)
								{
									//After firing, it's the AI, randomly reset the target at random to change target next frame
									if ((*it)->m_type == Weapon_Torpedo || RandomizeFloatBetweenValues(0.f, 1.f) < AI_CHANGE_TARGETROOM_PERCENTAGE)
									{
										(*it)->m_target_room = NULL;
									}
								}
							}
						}
					}
					else
					{
						(*it)->UpdatePosition();
					}
				}
			}
			else
			{
				for (vector<Weapon*>::iterator it = ship->m_weapons.begin(); it != ship->m_weapons.end(); it++)
				{
					(*it)->GameEntity::Update(deltaTime);
				}
			}

			//Engines
			for (vector<Engine*>::iterator it = ship->m_engines.begin(); it != ship->m_engines.end(); it++)
			{
				if (ship->m_is_fleeing == false)
				{
					(*it)->Update(deltaTime);

					//Fleeing order for human
					if (i == 0 && (*it)->IsHoveredByMouse() == true && m_scale == Scale_Tactical)
					{
						bool can_flee = ship->m_flee_count == ENGINE_FLEE_COUNT && ship->IsSystemOperational(System_Engine, (*it)->m_tile);
						m_contextual_order->SetContextualOrder(Order_Flee, (*it)->m_shape_container.getPosition(), can_flee == true);
						if (mouse_click == Mouse_RightClick && can_flee == true)
						{
							ship->m_is_fleeing = true;

							//remove hovering feedback
							(*it)->m_shape_container.setOutlineColor((*it)->m_default_color);
							(*it)->m_shape_container.setOutlineThickness(-1.f);
						}
					}
					else if (i == 1)
					{
						//IA fleeing (todo)
					}
				}
				else
				{
					(*it)->UpdatePosition();
					(*it)->m_systembar->UpdatePosition();
				}
			}

			//Rudder
			if (ship->m_rudder != NULL)
			{
				if (ship->m_is_fleeing == false)
				{
					ship->m_rudder->Update(deltaTime);
				}
				else
				{
					ship->m_rudder->UpdatePosition();
				}
			}

			//Combat interface
			ship->m_combat_interface.Update(ship->m_health, ship->m_health_max, ship->m_flood, ship->m_flood_max, ship->m_nb_crew, ship->m_nb_crew_max);
		}

		ship->UpdateTactical(deltaTime);
	}

	//Bullets + cleaning old bullets
	if ((*CurrentGame).m_pause == false)
	{
		vector<Ammo*> old_bullets;
		for (vector<Ammo*>::iterator it = (*CurrentGame).m_bullets.begin(); it != (*CurrentGame).m_bullets.end(); it++)
		{
			old_bullets.push_back(*it);
		}
		(*CurrentGame).m_bullets.clear();
		for (vector<Ammo*>::iterator it = old_bullets.begin(); it != old_bullets.end(); it++)
		{
			if ((*it)->m_can_be_seen == true && (*it)->m_target_ship->m_is_fleeing == false && m_scale == Scale_Tactical)
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

					vector<RoomTile*> tiles_hit;
					for (int i = x - radius; i < x + radius + 1; i++)
					{
						for (int j = y - radius; j < y + radius + 1; j++)
						{
							if (i >= 0 && i <= (*it)->m_target_ship->m_rooms_size.x - 1 && j >= 0 && j <= (*it)->m_target_ship->m_rooms_size.y - 1 && (*it)->m_target_ship->m_tiles[i][j] != NULL)
							{
								//"boom": apply bullet damage and side effects
								RoomTile* tile = (*it)->m_target_ship->m_tiles[i][j];

								if (tile == NULL)
								{
									continue;
								}

								//walls block splash damage
								if ((*it)->m_radius > 1 && tile->m_room != (*it)->m_target_tile->m_room)
								{
									continue;
								}

								tiles_hit.push_back(tile);

								//damage to ship
								int damage_ship = Min((*it)->m_damage, (*it)->m_target_ship->m_health);
								if (damage_ship > 0)
								{
									(*it)->m_target_ship->m_health -= damage_ship;

									//pop feedback
									ostringstream ss;
									ss << damage_ship;
									SFTextPop::CreateSFTextPop(*(*CurrentGame).m_font[Font_Arial], SFTEXTPOP_SIZE_2, sf::Text::Bold, sf::Color::Red, ss.str(), 30.f, 20.f, 1.f, tile, SFTEXTPOP_OFFSET_2);

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

									//destroying door
									if (tile->m_connexion != NULL)
									{
										tile->m_connexion->Destroy();
									}
								}

								//hull damage
								int damage_hull = Min((*it)->m_hull_damage, tile->m_health);
								if (damage_hull > 0)
								{
									tile->m_health -= damage_hull;
								}

								//piercing hull
								if (tile->m_hull != Hull_None && tile->m_is_pierced == false && tile->m_health == 0 && tile->m_weapon == NULL)//cannot pierce a tile where a weapon is standing
								{
									tile->m_is_pierced = true;
									if (tile->m_crew != NULL)
									{
										tile->m_crew->m_repair_timer = HULL_REPAIR_TIMER;
									}

									//pop feedback
									ostringstream ss_hull;
									ss_hull << "Pierced";
									SFTextPop::CreateSFTextPop(*(*CurrentGame).m_font[Font_Arial], SFTEXTPOP_SIZE_2, sf::Text::Bold, sf::Color::Blue, ss_hull.str(), 30.f, 20.f, 1.f, tile, SFTEXTPOP_OFFSET_2_BIS);
								}

								//shrapnel damage
								if ((*it)->m_shrapnel_damage > 0)
								{
									for (int j = 0; j < 2; j++)
									{
										for (vector<CrewMember*>::iterator it2 = (*it)->m_target_ship->m_crew[j].begin(); it2 != (*it)->m_target_ship->m_crew[j].end(); it2++)
										{
											if (GameEntity::IsColliding(tile, (*it2)) == true)
											{
												if ((*it2)->m_health > 0)
												{
													int damage_crew = Min((*it)->m_shrapnel_damage, (*it2)->m_health);
													(*it2)->m_health -= damage_crew;

													//pop feedback
													ostringstream ss_crew;
													ss_crew << damage_crew;
													SFTextPop::CreateSFTextPop(*(*CurrentGame).m_font[Font_Arial], SFTEXTPOP_SIZE_1, sf::Text::Bold, sf::Color::Red, ss_crew.str(), 30.f, 20.f, 1.f, tile, SFTEXTPOP_SIZE_1);
												}
											}
										}
									}
								}
							}
						}
					}

					//FX of the explosion
					int nb_hits = tiles_hit.size();
					//shuffle possible fx hits for a "random" carpet bombing
					if (nb_hits > 1)
					{
						vector<int> hits_order;
						for (int h = 0; h < nb_hits; h++)
						{
							hits_order.push_back(h);
						}
						for (int h = 0; h < nb_hits; h++)
						{
							int k = hits_order[h];
							int r = RandomizeIntBetweenValues(0, nb_hits - 1);
							hits_order[h] = hits_order[r];
							hits_order[r] = k;
						}

						for (int h = 0; h < nb_hits; h++)
						{
							RoomTile* k = tiles_hit[h];
							tiles_hit[h] = tiles_hit[hits_order[h]];
							tiles_hit[hits_order[h]] = k;
						}
					}

					//set FX positions
					for (int h = 0; h < nb_hits; h++)
					{
						FX* FX_hit = (*it)->m_FX_hit->Clone();
						FX_hit->m_position = tiles_hit[h]->m_position;
						FX_hit->UpdatePosition();
						FX_hit->m_delay_timer = h * SHRAPNEL_DELAY;
						(*CurrentGame).m_FX.push_back(FX_hit);
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

		//SFTextPop + clean old
		vector<SFTextPop*> old_texts_pop;
		for (vector<SFTextPop*>::iterator it = (*CurrentGame).m_texts_pop.begin(); it != (*CurrentGame).m_texts_pop.end(); it++)
		{
			old_texts_pop.push_back(*it);
		}
		(*CurrentGame).m_texts_pop.clear();
		for (vector<SFTextPop*>::iterator it = old_texts_pop.begin(); it != old_texts_pop.end(); it++)
		{
			if ((*it)->m_alpha > 0)
			{
				(*CurrentGame).m_texts_pop.push_back(*it);
				(*it)->Update(deltaTime);
			}
			else
			{
				delete *it;
			}
		}
	}

	//HUD resources
	m_resources_interface.Update();

	//MENUS
	//entering a seaport?
	if (warship_is_in_port == false && m_warship->m_seaport != NULL)
	{
		//unboarding?
		m_menu = Menu_CrewUnboard;
		m_warship->m_crew_unboard_interface.Init(m_warship, m_warship->m_seaport->m_island);
	}

	//Crew overboard menu
	if (m_menu == Menu_CrewOverboard)
	{
		if (m_warship->m_crew_overboard_interface.Update(deltaTime) == true)
		{
			m_menu = Menu_None;
		}
	}
	else if (m_menu == Menu_PrisonersChoice)
	{
		m_warship->m_prisoners_choice_interface.Update(deltaTime);

		if (m_warship->m_prisoners_choice_interface.m_crew.empty() == true)
		{
			m_warship->m_prisoners_choice_interface.Destroy();
			m_menu = Menu_None;
		}
	}
	else if (m_menu == Menu_CrewUnboard)
	{
		CrewMember* crew = NULL;
		if (hovered != NULL && (*CurrentGame).m_mouse_click == Mouse_RightClick && (*CurrentGame).m_window_has_focus == true)
		{
			//add crew to interface
			if (hovered->m_UI_type == UI_CrewMember)
			{
				CrewMember* crew = (CrewMember*)hovered;
				if (m_warship->m_crew_unboard_interface.AddCrewToInterface(crew) == true)
				{
					//remove from ship (since it has been added to the interface)
					vector<CrewMember*> old_crew;
					for (vector<CrewMember*>::iterator it = m_warship->m_crew[0].begin(); it != m_warship->m_crew[0].end(); it++)
					{
						old_crew.push_back(*it);
					}
					m_warship->m_crew[0].clear();
					for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
					{
						if (*it != crew)
						{
							m_warship->m_crew[0].push_back(*it);
						}
						else
						{
							crew->m_tile->m_crew = NULL;
						}
					}
				}
			}
			//remove crew from interface
			else if (hovered->m_UI_type == UI_CrewMemberUnboarding)
			{
				CrewMember* crew = (CrewMember*)hovered;
				m_warship->m_crew_unboard_interface.RemoveCrewFromInterface(crew);
				m_warship->AddCrewMember(crew);
			}
		}

		Reward* reward = m_warship->m_crew_unboard_interface.Update(deltaTime);
		if (reward != NULL)
		{
			m_warship->m_crew_unboard_interface.Destroy();

			m_warship->m_reward_interface.Init(m_warship, reward);
			m_menu = Menu_Reward;
		}
		
		
		//	bool found = false;
		//	for (vector<CrewMember*>::iterator it = m_warship->m_crew_unboard_interface.m_unboarded.begin(); it != m_warship->m_crew_unboard_interface.m_unboarded.end(); it++)
		//	{
		//		if (*it == hovered)
		//		{
		//			found = true;
		//			break;
		//		}
		//	}
		//
		//	if (found == false && m_warship->m_crew_unboard_interface.m_unboarded.size() < m_warship->m_crew_unboard_interface.m_slots_avaible)
		//	{
		//		//add unboarded crew to unboard interface
		//		crew = (CrewMember*)hovered;
		//		if (crew->m_is_prisoner == false)
		//		{
		//			m_warship->m_crew_unboard_interface.m_unboarded.push_back(crew);
		//			crew->m_position = m_warship->m_crew_unboard_interface.m_crew_slots[m_warship->m_crew_unboard_interface.m_unboarded.size() - 1]->m_position;
		//			crew->Update(deltaTime);//update position + reset hovering
		//
		//			//remove unboarded crew from ship from ship
		//			vector<CrewMember*> old_crew;
		//			for (vector<CrewMember*>::iterator it = m_warship->m_crew[0].begin(); it != m_warship->m_crew[0].end(); it++)
		//			{
		//				old_crew.push_back(*it);
		//			}
		//			m_warship->m_crew[0].clear();
		//			for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
		//			{
		//				if (*it != crew)
		//				{
		//					m_warship->m_crew[0].push_back(*it);
		//				}
		//			}
		//		}
		//	}
		//	else if(found == true)
		//	{
		//		//remove from unboarded crew interface
		//		m_warship->m_crew_unboard_interface.Update(deltaTime, crew);
		//	}
		//
		//	//update
		//	m_warship->m_crew_unboard_interface.Update(deltaTime, crew);
		//}
		//else
		//{
		//	m_warship->m_crew_unboard_interface.Update(deltaTime, NULL);
		//}
	}

	//Music
	(*CurrentGame).ManageMusicTransitions(deltaTime);
}

void Gameloop::Draw()
{
	(*CurrentGame).m_mainScreen.clear();

	//background
	m_background->Draw((*CurrentGame).m_mainScreen);

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
	}

	//ships
	int shipsVectorSize = m_ships.size();
	for (int i = 0; i < shipsVectorSize; i++)
	{
		Ship* ship = m_ships[i];
		if (m_scale == Scale_Strategic && ship->m_can_be_seen == true)
		{
			ship->GameEntity::Draw((*CurrentGame).m_mainScreen);

			//compass
			//m_warship->m_compass.Draw((*CurrentGame).m_mainScreen, m_warship->m_angle);
		}
		
		if (ship == m_warship || ship == m_tactical_ship)
		{
			if (ship->m_sinking_timer < SHIP_SINKING_TIME)
			{
				//room tiles
				if (i == 0)
				{
					//player room tiles: draw focused tile in last, so its outline appears above others
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
				}
				else
				{
					for (vector<RoomTile*>::iterator it = (*CurrentGame).m_enemy_tiles.begin(); it != (*CurrentGame).m_enemy_tiles.end(); it++)
					{
						(*it)->Draw((*CurrentGame).m_mainScreen);
					}
				}

				//rooms
				for (vector<Room*>::iterator it = ship->m_rooms.begin(); it != ship->m_rooms.end(); it++)
				{
					(*it)->Draw((*CurrentGame).m_mainScreen);
				}

				//doors
				for (vector<RoomConnexion*>::iterator it = ship->m_connexions.begin(); it != ship->m_connexions.end(); it++)
				{
					(*it)->Draw((*CurrentGame).m_mainScreen);
				}

				//weapons
				for (vector<Weapon*>::iterator it = ship->m_weapons.begin(); it != ship->m_weapons.end(); it++)
				{
					(*it)->Draw((*CurrentGame).m_mainScreen);

					if (m_scale == Scale_Tactical)
					{
						if (ship->m_is_fleeing == false && ship->m_sinking_timer <= 0)
						{
							(*it)->m_rofbar->Draw((*CurrentGame).m_mainScreen);

							if ((*it)->m_health < (*it)->m_health_max)
							{
								(*it)->m_lifebar->Draw((*CurrentGame).m_mainScreen);
							}
						}
					}
				}

				//engines
				for (vector<Engine*>::iterator it = ship->m_engines.begin(); it != ship->m_engines.end(); it++)
				{
					(*it)->Draw((*CurrentGame).m_mainScreen);

					if (ship->m_flee_count > 0)
					{
						(*it)->m_systembar->Draw((*CurrentGame).m_mainScreen);
					}
				}

				//rudder
				if (ship->m_rudder != NULL)
				{
					ship->m_rudder->Draw((*CurrentGame).m_mainScreen);
				}
			}

			//crew (can be displayed even when the ship is sunk, i.e. crew overboard)
			for (int j = 0; j < 2; j++)
			{
				for (vector<CrewMember*>::iterator it = ship->m_crew[j].begin(); it != ship->m_crew[j].end(); it++)
				{
					(*it)->Draw((*CurrentGame).m_mainScreen);

					if (ship->m_is_fleeing == false && ship->m_sinking_timer <= 0 && (*it)->m_health < (*it)->m_health_max)
					{
						(*it)->m_lifebar->Draw((*CurrentGame).m_mainScreen);
					}
				}
			}

			//combat interface
			if (ship->m_is_fleeing == false && ship->m_sinking_timer < SHIP_SINKING_TIME)
			{
				ship->m_combat_interface.Draw((*CurrentGame).m_mainScreen);
			}
		}

		if (i == 0 && ship->m_is_fleeing == false)
		{
			m_warship->m_crew_interface.Draw((*CurrentGame).m_mainScreen);
		}
	}

	
	if (m_scale == Scale_Tactical)
	{
		//Bullets
		for (vector<Ammo*>::iterator it = (*CurrentGame).m_bullets.begin(); it != (*CurrentGame).m_bullets.end(); it++)
		{
			(*it)->Draw((*CurrentGame).m_mainScreen);
		}
	}

	//FX
	for (vector<FX*>::iterator it = (*CurrentGame).m_FX.begin(); it != (*CurrentGame).m_FX.end(); it++)
	{
		if ((*it)->m_delay_timer <= 0)
		{
			(*it)->FX::Draw((*CurrentGame).m_mainScreen);
		}
	}

	//SFTextPop
	for (vector<SFTextPop*>::iterator it = (*CurrentGame).m_texts_pop.begin(); it != (*CurrentGame).m_texts_pop.end(); it++)
	{
		(*CurrentGame).m_mainScreen.draw(*(*it));
	}

	//Menus
	if (m_menu == Menu_PrisonersChoice)
	{
		m_warship->m_prisoners_choice_interface.Draw((*CurrentGame).m_mainScreen);
	}
	else if (m_menu == Menu_CrewOverboard)
	{
		m_warship->m_crew_overboard_interface.Draw((*CurrentGame).m_mainScreen);
	}
	else if (m_menu == Menu_CrewUnboard)
	{
		m_warship->m_crew_unboard_interface.Draw((*CurrentGame).m_mainScreen);
	}
	else if (m_menu == Menu_Reward)
	{
		m_warship->m_reward_interface.Draw((*CurrentGame).m_mainScreen);
	}

	//HUD - resources interface
	m_resources_interface.Draw((*CurrentGame).m_mainScreen);

	//HUD - contextual order
	if (m_contextual_order->m_type != Order_None && m_warship->m_is_fleeing == false && m_warship->m_sinking_timer <= 0)
	{
		m_contextual_order->Draw((*CurrentGame).m_mainScreen);
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
		//win-lose conditions: low health, high flood, too many crew dead, fleeing out of screen
		bool win_crew = m_tactical_ship == NULL || m_tactical_ship->m_nb_crew < m_tactical_ship->m_nb_crew_max * 0.3;
		bool win_health = m_tactical_ship == NULL || m_tactical_ship->m_sinking_timer >= SHIP_SINKING_TIME && m_tactical_ship->m_health == 0; //m_tactical_ship->m_health < m_tactical_ship->m_health_max * 0.2f;
		bool win_flood = m_tactical_ship == NULL || m_tactical_ship->m_sinking_timer >= SHIP_SINKING_TIME && win_health == false;
		bool win_flee = m_tactical_ship == NULL || m_tactical_ship->m_ship_offset.y < -m_tactical_ship->m_rooms_size.y * ROOMTILE_SIZE * 0.5f - ROOMTILE_OFFSET_Y;

		bool lose_crew = m_warship->m_nb_crew == 0;
		bool lose_health = m_warship->m_sinking_timer >= SHIP_SINKING_TIME;
		bool lose_flood = m_warship->m_sinking_timer >= SHIP_SINKING_TIME && lose_health == false;
		bool lose_flee = m_warship->m_ship_offset.y < -m_warship->m_rooms_size.y * ROOMTILE_SIZE * 0.5f - ROOMTILE_OFFSET_Y;

		bool win = m_tactical_ship == NULL || win_health == true || win_flood == true || win_crew == true || win_flee == true;
		bool lose = lose_health == true || lose_flood == true || lose_flee == true;
		if (win == true || lose == true)
		{
			//make prisoners
			if (m_menu == Menu_None && m_tactical_ship != NULL)
			{
				//surviving enemy crews? => open prisoners choice interface
				//TODO: win_flood and win_health first offers a game of choosing among the survivors whom to pull out from a certain death
				if (win == true)
				{
					if (win_crew == true)
					{
						m_menu = Menu_PrisonersChoice;
						m_warship->m_prisoners_choice_interface.Init(m_warship, m_tactical_ship);
					}
					else if (m_warship->m_crew_overboard_interface.m_rescued.empty() == false)
					{
						//transfer rescued crew to prisoners choice interface
						for (vector<CrewMember*>::iterator it = m_warship->m_crew_overboard_interface.m_rescued.begin(); it != m_warship->m_crew_overboard_interface.m_rescued.end(); it++)
						{
							m_warship->m_prisoners_choice_interface.m_crew.push_back(*it);
						}
						m_warship->m_crew_overboard_interface.m_rescued.clear();

						m_menu = Menu_PrisonersChoice;
						m_warship->m_prisoners_choice_interface.Init(m_warship, m_tactical_ship);
					}
					else if (m_tactical_ship->m_crew[0].empty() == false || m_tactical_ship->m_crew[1].empty() == false)
					{
						m_menu = Menu_CrewOverboard;
						m_warship->m_crew_overboard_interface.Init(m_warship, m_tactical_ship);
					}
				}
			}
			
			//switch back to strategic scale, after a few cleaning
			if (m_menu == Menu_None)
			{
				//Get reward
				if (win == true)
				{
					m_warship->AddResource(Resource_Gold, 5);
					SavePlayerData(m_warship);
				}
				//else
				//{
				//	LoadPlayerData(m_warship);
				//	m_warship->RestoreHealth();
				//	m_warship->InitCombat();
				//	m_tactical_ship = NULL;
				//	m_scale = Scale_Strategic;
				//}

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

				m_warship->RestoreHealth();
				m_warship->InitCombat();
				m_scale = Scale_Strategic;
			}
		}

		return true;
	}

	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		if ((*it) == m_warship || (*it)->m_can_be_seen == false)
		{
			continue;
		}

		m_tactical_ship = IsDMSInCombatRange(m_warship->m_DMS, (*it)->m_DMS);

		if (m_tactical_ship != NULL)
		{
			m_scale = Scale_Tactical;
			break;
		}
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
		m_tactical_ship->m_combat_interface.Init(m_tactical_ship, m_tactical_ship->m_alliance, m_tactical_ship->m_display_name, m_tactical_ship->m_type);//init enemy interface

		//fill enemy ship room tiles for pathfind
		(*CurrentGame).m_enemy_tiles.clear();
		for (vector<Room*>::iterator it = m_tactical_ship->m_rooms.begin(); it != m_tactical_ship->m_rooms.end(); it++)
		{
			for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
			{
				(*CurrentGame).m_enemy_tiles.push_back(*it2);
			}
		}

		(*CurrentGame).PlayMusic(Music_Combat);
	}

	return (m_scale == Scale_Tactical);
}

void Gameloop::UpdateRoomTileFeedback(RoomTile* tile, sf::Time deltaTime, Ship* ship)
{
	tile->m_shape_container.setFillColor(sf::Color::Black);

	if (tile->m_is_pierced == true)
	{
		tile->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Pierced]);
	}
	else if (tile->m_flood > 0)
	{
		tile->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Blue_Flood]);
	}
	else if (tile->m_is_prison == true)
	{
		tile->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Red_Prison]);
	}

	if (tile->m_system_tile != NULL)
	{
		if (tile->m_crew != NULL && tile->m_crew->m_tile == tile)
		{
			if (tile->m_system_tile->m_weapon == NULL || tile->m_system_tile->m_weapon->m_health > 0)
			{
				tile->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Green_System]);
			}
		}
		else
		{
			switch (tile->m_system_tile->m_system)
			{
				case System_Weapon:
				{
					if (tile->m_system_tile->m_weapon != NULL && tile->m_system_tile->m_weapon->m_health > 0)
					{
						tile->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Orange_System]);
					}
					break;
				}
				case System_Navigation:
				case System_Engine:
				{
					tile->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Cyan_System]);
					break;
				}
			}
		}
	}

	if ((*CurrentGame).m_pause == false)
	{
		//rudder animations
		if (tile->m_rudder != NULL)
		{
			Rudder* rudder = (Rudder*)tile->m_rudder;
			if (rudder->m_rotation_timer > 0)
			{
				rudder->m_rotation_timer -= deltaTime.asSeconds();
			}

			if (Ship::IsSystemOperational(System_Navigation, tile) == true)
			{
				if (rudder->m_rotation_timer <= 0.f)
				{
					rudder->m_rotation_timer = RandomizeFloatBetweenValues(RUDDER_ROTATION_TIMER_MIN, RUDDER_ROTATION_TIMER_MAX);
					int sign = rudder->m_rotation_speed > 0 ? 1 : -1;
					rudder->m_rotation_speed = Lerp(rudder->m_rotation_timer, RUDDER_ROTATION_TIMER_MIN, RUDDER_ROTATION_TIMER_MAX, RUDDER_ROTATION_SPEED_MAX, RUDDER_ROTATION_SPEED_MIN) * (-sign);
				}
			}
			else
			{
				rudder->m_rotation_speed = 0.f;
				rudder->m_rotation_timer = 0.f;
			}

			rudder->rotate(rudder->m_rotation_speed);
		}

		//engine animations
		if (tile->m_engine != NULL)
		{
			Engine* engine = (Engine*)tile->m_engine;

			if (ship->m_is_fleeing == false)
			{
				if (Ship::IsSystemOperational(System_Engine, tile) == true)
				{
					if (engine->m_rotation_speed < ENGINE_ROTATION_SPEED)
					{
						engine->m_rotation_speed++;
					}
					else if (engine->m_rotation_speed > ENGINE_ROTATION_SPEED)
					{
						engine->m_rotation_speed = ENGINE_ROTATION_SPEED;
					}
				}
				else if (engine->m_rotation_speed > 0.f)
				{
					engine->m_rotation_speed--;
				}
			}
			else
			{
				if (engine->m_rotation_speed < ENGINE_FLEE_ROTATION_SPEED)
				{
					engine->m_rotation_speed++;
				}
			}

			engine->rotate(engine->m_rotation_speed);
		}
	}
}

Ship* Gameloop::IsDMSInCombatRange(DMS_Coord DMS_a, DMS_Coord DMS_b)
{
	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		if ((*it) == m_warship || (*it)->m_can_be_seen == false)
		{
			continue;
		}

		//in range for combat?
		float posxA = DMS_a.m_minute_x * 60.f + DMS_a.m_second_x;
		float posxB = DMS_b.m_minute_x * 60.f + DMS_b.m_second_x;
		if (abs(posxA - posxB) > 1.f * NB_WATERTILE_SUBDIVISION)
		{
			continue;
		}

		float posyA = DMS_a.m_minute_y * 60.f + DMS_a.m_second_y;
		float posyB = DMS_b.m_minute_y * 60.f + DMS_b.m_second_y;
		if (abs(posyA - posyB) > 1.f * NB_WATERTILE_SUBDIVISION)
		{
			continue;
		}

		return *it;
	}

	return NULL;
}


//SAVING PLAYER DATA
int Gameloop::SavePlayerData(Warship* warship)
{
	printf("Saving player data.\n");

	ofstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		data << "DMS " << warship->m_DMS.m_degree_x << " " << warship->m_DMS.m_minute_x << " " << warship->m_DMS.m_degree_y << " " << warship->m_DMS.m_minute_y << endl;
		data << "Gold " << warship->m_resources[Resource_Gold] << endl;
		data << "Fish " << warship->m_resources[Resource_Fish] << endl;
		data << "Mech " << warship->m_resources[Resource_Mech] << endl;
		data << "Fidelity " << warship->m_resources[Resource_Fidelity] << endl;
		data << "Days " << warship->m_resources[Resource_Days] << endl;

		for (int j = 0; j < 2; j++)
		{
			for (vector<CrewMember*>::iterator it = warship->m_crew[j].begin(); it != warship->m_crew[j].end(); it++)
			{
				data << "Crew " << (*it)->m_display_name << " " << (*it)->m_type << " " << (*it)->m_race << " " << (*it)->m_health << " " << (*it)->m_health_max;
				for (int i = 0; i < NB_CREW_SKILLS; i++)
				{
					data << " " << (*it)->m_skills[i];
				}

				int prisoner = (*it)->m_is_prisoner == true ? 1 : 0;
				data << " " << prisoner;
				data << " " << (*it)->m_tile->m_coord_x << " " << (*it)->m_tile->m_coord_y;
				data << endl;
			}
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "No save file found. A new file is going to be created.\n" << endl;
	}

	return 0;
}

int Gameloop::LoadPlayerData(Warship* warship)
{
	//unselect current selection before loading
	if ((*CurrentGame).m_selected_ui != NULL)
	{
		(*CurrentGame).m_selected_ui->m_selected = false;
		(*CurrentGame).m_selected_ui->m_shape_container.setOutlineColor((*CurrentGame).m_selected_ui->m_default_color);
		(*CurrentGame).m_selected_ui = NULL;
	}

	printf("Loading player data.\n");

	std::ifstream  data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		std::getline(data, line);
		string s;

		int DMS_degree_x, DMS_minute_x, DMS_degree_y, DMS_minute_y;
		std::istringstream(line) >> s >> DMS_degree_x >> DMS_minute_x >> DMS_degree_y >> DMS_minute_y;
		warship->SetDMSCoord(DMS_Coord(DMS_degree_x, DMS_minute_x, 0.f, DMS_degree_y, DMS_minute_y, 0.f));

		int i = 0;
		for (int i = 0; i < NB_RESOURCES_TYPES; i++)
		{
			string s;
			std::getline(data, line);
			std::istringstream(line) >> s >> warship->m_resources[i];
		}

		warship->Reset();
		while (std::getline(data, line))
		{
			string t;
			std::istringstream(line) >> t;

			if (t.compare("Crew") == 0)
			{
				string name;
				int type, race;
				
				int skills[NB_CREW_SKILLS];
				bool prisoner;
				int health, health_max, coord_x, coord_y;
				std::istringstream(line) >> t >> name >> type >> race >> health >> health_max >> skills[Skill_Gunner] >> skills[Skill_Fishing] >> skills[Skill_Melee] >> skills[Skill_Navigation] >> skills[Skill_Engine] >> (bool)prisoner >> coord_x >> coord_y;
				
				CrewMember* crew = new CrewMember((CrewMemberType)type, Alliance_Player, (CrewMemberRace)race, prisoner);
				
				crew->m_display_name = name;
				crew->m_health_max = health_max;
				crew->m_health = health;
				for (int i = 0; i < NB_CREW_SKILLS; i++)
				{
					crew->m_skills[i] = skills[i];
				}
				crew->m_tile = warship->m_tiles[coord_x][coord_y];
				warship->m_tiles[coord_x][coord_y]->m_crew = crew;
				crew->m_position = crew->m_tile->m_position;//sf::Vector2f(coord_x, coord_y);

				warship->m_crew[(int)crew->m_is_prisoner].push_back(crew);
			}

			warship->m_nb_crew = warship->m_crew[0].size();
		}

		data.close();  // on ferme le fichier
		return 1;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "No save file found. A new file is going to be created.\n" << endl;
		return 0;
	}
}

void Gameloop::UpdateContextualOrderFeedback(CrewMember* crew, RoomTile* tile)
{
	//if we're not already on the tile hovered, a feedback of the possible order is displayed
	if (crew->m_tile != tile)
	{
		if (tile->m_is_pierced == true)
		{
			m_contextual_order->SetContextualOrder(Order_RepairHull, tile->m_shape_container.getPosition(), tile->m_crew == NULL || tile->m_crew == crew);
		}
		else if (tile->m_system_tile != NULL)
		{
			if (tile->m_system_tile->m_weapon != NULL)
			{
				m_contextual_order->SetContextualOrder(Order_Weapon, tile->m_shape_container.getPosition(), (tile->m_crew == NULL || tile->m_crew == crew) && tile->m_system_tile->m_weapon->m_health > 0);
			}
			else if (tile->m_system_tile->m_system == System_Engine)
			{
				m_contextual_order->SetContextualOrder(Order_Engine, tile->m_shape_container.getPosition(), tile->m_crew == NULL || tile->m_crew == crew);
			}
			else if (tile->m_system_tile->m_system == System_Navigation)
			{
				m_contextual_order->SetContextualOrder(Order_Rudder, tile->m_shape_container.getPosition(), tile->m_crew == NULL || tile->m_crew == crew);
			}
		}
		else if (tile->m_operator_tile == NULL)
		{
			if (tile->m_room->m_type == Room_Crewquarter)
			{
				m_contextual_order->SetContextualOrder(Order_Heal, tile->m_shape_container.getPosition(), tile->m_crew == NULL || tile->m_crew == crew);
			}
			else if (tile->m_flood > 0)
			{
				m_contextual_order->SetContextualOrder(Order_Swim, tile->m_shape_container.getPosition(), tile->m_crew == NULL || tile->m_crew == crew);
			}
			else
			{
				m_contextual_order->SetContextualOrder(Order_Move, tile->m_shape_container.getPosition(), tile->m_crew == NULL || tile->m_crew == crew);
			}
		}
	}
}

void Gameloop::UpdateTargetRoom(Weapon* weapon, Room* room_hovered)
{
	// 1. colorier en jaune la room survoleé (hovered) si c'est une cible potentielle
	// 2. colorier en rouge si la cible actuelle et que ce n'est pas la cible survolée
	// 3. attribuer target_room sur la room survolée si c'est une cible potentielle

	if (weapon->m_ship->m_alliance == Alliance_Player)
	{
		//color hovered room if it's a legit target
		if (room_hovered != NULL)
		{
			//can be targeted?
			bool room_can_be_targed = weapon->m_type != Weapon_Torpedo;
			if (weapon->m_type == Weapon_Torpedo)
			{
				for (vector<RoomTile*>::iterator it2 = room_hovered->m_tiles.begin(); it2 != room_hovered->m_tiles.end(); it2++)
				{
					if ((*it2)->m_hull == Hull_Left)
					{
						room_can_be_targed = true;
						break;
					}
				}
			}

			if (room_can_be_targed == true)
			{
				//color the room
				float hull_pos_feedback_x = 0;
				float hull_pos_feedback_ymin = 0;
				float hull_pos_feedback_ymax = 0;

				for (vector<RoomTile*>::iterator it2 = room_hovered->m_tiles.begin(); it2 != room_hovered->m_tiles.end(); it2++)
				{
					if (weapon->m_type != Weapon_Torpedo || (*it2)->m_hull == Hull_Left)
					{
						(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Yellow_Target_Hovered]);

						hull_pos_feedback_x = (*it2)->m_shape_container.getPosition().x;
						hull_pos_feedback_ymin = hull_pos_feedback_ymin == 0 || hull_pos_feedback_ymin > (*it2)->m_shape_container.getPosition().y ? (*it2)->m_shape_container.getPosition().y : hull_pos_feedback_ymin;
						hull_pos_feedback_ymax = hull_pos_feedback_ymax == 0 || hull_pos_feedback_ymax < (*it2)->m_shape_container.getPosition().y ? (*it2)->m_shape_container.getPosition().y : hull_pos_feedback_ymax;
					}
				}

				//get order: target room
				if (weapon->m_type != Weapon_Torpedo)
				{
					m_contextual_order->SetContextualOrder(Order_TargetRoom, room_hovered->m_shape_container.getPosition(), weapon->m_health > 0);
				}
				else
				{
					m_contextual_order->SetContextualOrder(Order_TargetHull, sf::Vector2f(hull_pos_feedback_x, (hull_pos_feedback_ymin + hull_pos_feedback_ymax) * 0.5), weapon->m_health > 0);
				}

				if ((*CurrentGame).m_mouse_click == Mouse_RightClick)
				{
					Weapon* weapon = (Weapon*)(*CurrentGame).m_selected_ui;
					weapon->m_target_room = room_hovered;
				}
			}
		}

		//color previous target
		if (weapon->m_target_room != NULL && weapon->m_target_room != room_hovered)
		{
			for (vector<RoomTile*>::iterator it2 = weapon->m_target_room->m_tiles.begin(); it2 != weapon->m_target_room->m_tiles.end(); it2++)
			{
				if (weapon->m_type != Weapon_Torpedo || (*it2)->m_hull == Hull_Left)
				{
					(*it2)->m_shape_container.setFillColor((*CurrentGame).m_dico_colors[Color_Red_Target_Locked]);
				}
			}
		}
	}
	
}

void Gameloop::UpdateAICrew(CrewMember* crew)
{
	if (crew->m_destination == NULL)
	{
		//first look into current room if there is any interesting task to do. Then we'll do the same search across all rooms
		RoomTile* destination = NULL;

		//not already busy reparing?
		if ((crew->m_tile != NULL && crew->m_tile->m_crew == crew && crew->m_tile->m_is_pierced == true) == false)
		{
			//Hull to repair?
			for (vector<RoomTile*>::iterator it2 = (*CurrentGame).m_enemy_tiles.begin(); it2 != (*CurrentGame).m_enemy_tiles.end(); it2++)
			{
				if ((*it2)->m_is_pierced == true && (*it2)->m_crew == NULL)
				{
					destination = (*it2);
					break;
				}
			}

			//Systems to use
			if (destination == NULL && (crew->m_tile->m_system_tile != NULL && crew->m_tile->m_crew == crew) == false)
			{
				for (vector<RoomTile*>::iterator it2 = (*CurrentGame).m_enemy_tiles.begin(); it2 != (*CurrentGame).m_enemy_tiles.end(); it2++)
				{
					if ((*it2)->m_operator_tile != NULL && (*it2)->m_operator_tile->m_crew == NULL)
					{
						if ((*it2)->m_weapon == NULL || (*it2)->m_weapon->m_health > 0)
						{
							destination = (*it2)->m_operator_tile;
							break;
						}
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
					destination->m_crew = crew;

					//assign destination for pathfind
					crew->m_destination = destination;
				}
			}
		}
	}
}

void Gameloop::UpdateAITargetRoom(Weapon* weapon)
{
	if (weapon->m_target_room == NULL)
	{
		vector<Room*> possible_rooms;
		for (vector<Room*>::iterator it2 = m_warship->m_rooms.begin(); it2 != m_warship->m_rooms.end(); it2++)
		{
			if ((*it2)->m_type == Room_PrisonCell)
			{
				continue;
			}

			if (weapon->m_type == Weapon_Torpedo)
			{
				for (vector<RoomTile*>::iterator it3 = (*it2)->m_tiles.begin(); it3 != (*it2)->m_tiles.end(); it3++)
				{
					if ((*it3)->m_hull == Hull_Right && (*it3)->m_is_pierced == false && (*it3)->m_weapon == NULL)
					{
						possible_rooms.push_back(*it2);
						break;
					}
				}
			}
			else
			{
				possible_rooms.push_back(*it2);
			}
		}

		if (possible_rooms.empty() == false)
		{
			int r = RandomizeIntBetweenValues(0, possible_rooms.size() - 1);
			weapon->m_target_room = possible_rooms[r];
		}
	}
}

bool Gameloop::UpdateUnboarding()
{
	if (m_warship->m_seaport != NULL && m_menu != Menu_CrewUnboard)
	{
		m_menu = Menu_CrewUnboard;
		
		m_warship->m_crew_unboard_interface.Init(m_warship, m_warship->m_seaport->m_island);
	}
	else if (m_warship->m_seaport == NULL && m_menu == Menu_CrewUnboard)
	{
		m_menu = Menu_None;

		m_warship->m_crew_unboard_interface.Destroy();
	}

	return true;
}