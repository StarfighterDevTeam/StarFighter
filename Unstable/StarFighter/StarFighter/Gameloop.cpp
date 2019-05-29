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
	m_ships.push_back(m_warship);
	m_resources_interface.Init(m_warship);

	if (LoadPlayerData(m_warship) == 0)
	{
		//No saved game loaded => create a new save from scratch
		m_warship->Init();

		GenerateRandomIslands(0, 0);
		GenerateRandomSecretLocations(0, 0);
		GenerateRandomShips(0, 0);

		//create a new save file
		SavePlayerData(m_warship);
	}

	//m_ships.push_back(new Ship(DMS_Coord{ 0, 13, 0, 0, 8, 0 }, Ship_FirstClass, Alliance_Enemy, "L'Esquif"));
	//m_ships.push_back(new Ship(DMS_Coord{ 0, 16, 0, 0, 11, 0 }, Ship_FirstClass, Alliance_Enemy, "Le Goelan"));
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
	
	for (vector<Seaport*>::iterator it = m_seaports.begin(); it != m_seaports.end(); it++)
	{
		delete (*it);
	}

	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		delete (*it);
	}

	delete m_contextual_order;

	m_resources_interface.Destroy();

	for (int i = 0; i < NB_SECRET_LOCATION_TYPES; i++)
	{
		for (vector<Location*>::iterator it = m_secret_locations[i].begin(); it != m_secret_locations[i].end(); it++)
		{
			delete (*it);
		}
	}
	
}

void Gameloop::InitWaterZones()
{
	//zones and water
	vector<WaterZone*> vec;
	WaterZone* zone = new WaterZone(0, 0);
	vec.push_back(zone);
	(*CurrentGame).m_waterzones.push_back(vec);
}

bool Gameloop::GetSaveOrLoadInputs()
{
	if ((*CurrentGame).m_input_actions[Action_Saving] == Input_Tap && (*CurrentGame).m_window_has_focus == true)
	{
		SavePlayerData(m_warship);
		return true;
	}
	else if (m_resources_interface.m_save_button->ButtonUpdate() == true && (*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		SavePlayerData(m_warship);
		return true;
	}
	else if (m_resources_interface.m_load_button->ButtonUpdate() == true && (*CurrentGame).m_mouse_click == Mouse_LeftClick)
	{
		//todo: clean current game
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
	
	//UPDATING GAME ENTITIES
	m_warship->UpdateUpkeepCosts();

	//change of scale?
	UpdateTacticalScale();

	//Water tiles
	if (m_scale == Scale_Strategic)
	{
		//reset previous list
		for (vector<WaterTile*>::iterator it = m_warship->m_tiles_can_be_seen.begin(); it != m_warship->m_tiles_can_be_seen.end(); it++)
		{
			(*it)->m_can_be_seen = false;
		}
		m_warship->m_tiles_can_be_seen.clear();

		//reduce the range of tiles that are likely to be the right candidates for being seen
		int min_x = m_warship->m_DMS.m_minute_x - (NB_WATERTILE_VIEW_RANGE + 1);
		int max_x = m_warship->m_DMS.m_minute_x + (NB_WATERTILE_VIEW_RANGE + 1);
		Bound(min_x, 0, NB_WATERTILE_SUBDIVISION - 1);
		Bound(max_x, 0, NB_WATERTILE_SUBDIVISION - 1);

		int min_y = m_warship->m_DMS.m_minute_y - (NB_WATERTILE_VIEW_RANGE + 1);
		int max_y = m_warship->m_DMS.m_minute_y + (NB_WATERTILE_VIEW_RANGE + 1);
		Bound(min_y, 0, NB_WATERTILE_SUBDIVISION - 1);
		Bound(max_y, 0, NB_WATERTILE_SUBDIVISION - 1);
		
		for (int i = min_x; i < max_x; i++)
		{
			for (int j = min_y; j < max_y; j++)
			{
				WaterTile* tile = m_warship->m_tile->m_zone->m_watertiles[i][j];

				//can be seen? no need to update other tiles because they won't be drawn anyway
				if (m_warship->CanViewWaterTile(tile))
				{
					tile->m_can_be_seen = true;
					m_warship->m_tiles_can_be_seen.push_back(tile);

					//position on "radar"
					tile->UpdatePosition(m_warship->m_DMS);
					if (tile->m_location != NULL)
					{
						tile->m_location->m_position = tile->m_position;
						tile->m_location->UpdatePosition();

						if (tile->m_location->m_type == Location_Seaport)
						{
							Seaport* seaport = (Seaport*)tile->m_location;
							seaport->m_text.setPosition(sf::Vector2f(seaport->m_text.getPosition().x, seaport->m_text.getPosition().y - seaport->m_text.getGlobalBounds().height * 0.5 - WATERTILE_SIZE * 0.5 - 10));
						}
					}

					//selection
					if (selection == m_warship && tile->m_type == Water_Empty)// && m_warship->m_destination == NULL
					{
						//display tile coords
						//ostringstream ss;
						//ss << tile->m_coord_x << ", " << tile->m_coord_y;
						//tile->m_text.setString(ss.str());
						tile->GameEntity::Update(deltaTime);
					}
					else
					{
						tile->GameEntity::UpdatePosition();

						if (tile->m_type == Water_Empty)
						{
							tile->m_text.setString("");

							//selection of water tiles is forbidden
							if (tile->m_selected == true)
							{
								tile->m_selected = false;
								tile->m_shape_container.setOutlineColor(tile->m_default_color);
								(*CurrentGame).m_selected_ui = NULL;
							}
						}
					}
				}
			}
		}
	}

	////Islands
	//for (vector<Seaport*>::iterator it = m_seaports.begin(); it != m_seaports.end(); it++)
	//{
	//	//position on "radar"
	//	(*it)->m_position = (*it)->m_tile->m_position;
	//	(*it)->UpdatePosition();
	//	(*it)->m_text.setPosition(sf::Vector2f((*it)->m_text.getPosition().x, (*it)->m_text.getPosition().y - (*it)->m_text.getGlobalBounds().height * 0.5 - WATERTILE_SIZE * 0.5 - 10));
	//}

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
	DMS_Coord warship_DMS = m_warship->m_DMS;
	int shipsVectorSize = m_ships.size();
	bool ship_moving = false;
	for (int i = 0; i < shipsVectorSize; i++)
	{
		if (m_ships[i]->m_speed != sf::Vector2f(0, 0))
		{
			ship_moving = true;
			break;
		}
	}

	if (m_scale == Scale_Strategic && ship_moving == false && (*CurrentGame).m_pause == false && m_menu == Menu_None)
	{
		for (int i = 0; i < shipsVectorSize; i++)
		{
			m_ships[i]->m_has_played = false;
		}
	}

	for (int i = 0; i < shipsVectorSize; i++)
	{
		Ship* ship = m_ships[i];

		//Strategical scale update
		if (m_scale == Scale_Strategic)
		{
			if (ship == m_warship)
			{
				//Orders
				if (ship->m_selected == true && hovered != NULL && hovered->m_UI_type == UI_WaterTile)
				{
					WaterTile* tile_hovered = (WaterTile*)hovered;

					//Hovering a water tile that is not our current tile?
					if (WaterTile::SameDMS(warship_DMS, tile_hovered->m_DMS) == false)
					{
						//Scan all enemy ships to see if we're in range for combat with anyone
						Ship* ship_in_combat_range = NULL;
						for (vector<Ship*>::iterator it3 = m_ships.begin(); it3 != m_ships.end(); it3++)
						{
							ship_in_combat_range = IsDMSInCombatRange(tile_hovered->m_DMS, true);
							if (ship_in_combat_range != NULL)
							{
								break;
							}
						}

						//Sail orders
						if (m_menu == Menu_None && ship_moving == false)// m_warship->m_speed == sf::Vector2f(0, 0))
						{
							int cost = m_warship->GetShortestPathLength(m_warship->m_tile, tile_hovered);
							
							//Contextual orders' feedback
							if (ship_in_combat_range == NULL)
							{
								if (tile_hovered->m_location == NULL || tile_hovered->m_location->m_type != Location_Seaport)
								{
									m_contextual_order->SetContextualOrder(Order_Sail, tile_hovered->m_position, cost <= m_warship->m_moves_max, cost);
								}
								else
								{
									m_contextual_order->SetContextualOrder(Order_Dock, tile_hovered->m_position, cost <= m_warship->m_moves_max, cost);
								}
							}
							else if (ship_in_combat_range->m_alliance == Alliance_Enemy)
							{
								m_contextual_order->SetContextualOrder(Order_Engage, tile_hovered->m_position, cost <= m_warship->m_moves_max, cost);
							}
							else if (ship_in_combat_range->m_alliance == Alliance_Ally)
							{
								m_contextual_order->SetContextualOrder(Order_Interact, tile_hovered->m_position, cost <= m_warship->m_moves_max, cost);
							}

							//Sail order
							if (mouse_click == Mouse_RightClick && tile_hovered->m_type == Water_Empty && cost <= m_warship->m_moves_max)
							{
								if (m_warship->SetSailsToWaterTile(tile_hovered, warship_DMS) == true)
								{
									SpendDays(cost, false);
									m_warship->m_can_open_new_menu = true;
								}
							}
						}
					}
				}

				ship->GameEntity::Update(deltaTime);

				//Rooms
				m_warship->UpdateRooms();
			}
			else//AI
			{
				//other ships update their position and visibility respect to player ship
				//TODO: finalize round up/down for visibility
				int x = ship->m_tile->m_DMS.m_minute_x;//+ (ship->m_speed.x < 0 && ship->m_DMS.m_second_x > 0 ? 0 : 0);
				int y = ship->m_tile->m_DMS.m_minute_y + (ship->m_speed.y > 0 && ship->m_DMS.m_second_y > 0 ? 1 : 0);
				ship->m_can_be_seen = ship->m_tile->m_zone->m_watertiles[x][y]->m_can_be_seen;
				ship->UpdatePosition(warship_DMS);

				//AI strategical movement
				if (m_warship->GetDistanceToWaterTile(ship->m_tile) < NB_WATERTILE_VIEW_RANGE + m_warship->m_moves_max)
				{
					SetAIStrategicalDestination(ship, warship_DMS);
				}

				ship->AnimatedSprite::update(deltaTime);
			}

			//apply orders (pathfind, speed...)
			if ((*CurrentGame).m_pause == false && m_menu == Menu_None)
			{
				ship->UpdateStrategical(deltaTime, warship_DMS);
			}

			//Player ship UI
			if (ship == m_warship)
			{
				ostringstream ss;
				ss << "\n\n\n";
				ss << ship->m_DMS.m_degree_y << "°" << ship->m_DMS.m_minute_y << "' " << (int)ship->m_DMS.m_second_y << "\"\N";
				ss << "\n";
				ss << ship->m_DMS.m_degree_x << "°" << ship->m_DMS.m_minute_x << "' " << (int)ship->m_DMS.m_second_x << "\"\E";
				ship->m_text.setString(ss.str());
			}
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
					if (ship == m_warship && m_warship->m_is_fleeing == false)
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
				if (ship == m_warship && m_warship->m_is_fleeing == false)
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
			if (ship == m_warship && mouse_click == Mouse_LeftClick && hovered != NULL && hovered->m_UI_type == UI_Connexion && ship->m_is_fleeing == false)
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

				int fidelity = 0;
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
						if (ship == m_warship)
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
						if (j == 0)
						{
							fidelity += (*it)->m_fidelity;
						}
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

						if (ship == m_warship)
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

				if (j == 0)
				{
					ship->m_resources[Resource_Fidelity] = ship->m_crew[j].empty() == true ? 50 : fidelity / ship->m_crew[j].size();
				}
			}

			//Move order for player's crew
			if (ship == m_warship && mouse_click == Mouse_RightClick && selection != NULL && selection->m_UI_type == UI_CrewMember && hovered != NULL && hovered->m_UI_type == UI_RoomTile && ship->m_is_fleeing == false)
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

						Ship* enemy_ship = ship == m_warship ? m_tactical_ship : m_warship;
						if (enemy_ship != NULL && enemy_ship->m_is_fleeing == false && enemy_ship->m_sinking_timer <= 0)
						{
							if ((*it)->m_target_room == NULL && ship != m_warship)
							{
								//AI choosing a target for each weapon
								UpdateAITargetRoom(*it);
							}

							if ((*it)->CanFire() == true && (*it)->m_target_room != NULL && (*CurrentGame).m_pause == false)
							{
								if (ship->FireWeapon(*it, deltaTime, enemy_ship) == true && ship != m_warship)
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
					if (ship == m_warship && (*it)->IsHoveredByMouse() == true && m_scale == Scale_Tactical)
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
					else if (ship != m_warship)
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
			int sonar = -1;
			if (ship == m_warship)
			{
				sonar = m_warship->m_sonar;
			}
			ship->m_combat_interface.Update(ship->m_health, ship->m_health_max, ship->m_flood, ship->m_flood_max, ship->m_nb_crew, ship->m_nb_crew_max, sonar);
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
	Ship* ship_in_combat_range = IsDMSInCombatRange(m_warship->m_DMS, true);
	//Open a new contextual menu?
	if (m_warship->m_can_open_new_menu == true && m_warship->m_has_played == false)
	{
		Location* location = m_warship->m_tile->m_location;
		//Arriving on a special location?
		if (location != NULL && location->m_type == Location_Seaport)
		{
			m_menu = Menu_CrewUnboard;
			m_warship->m_crew_unboard_interface.Init(m_warship, m_warship->m_tile->m_location, NULL);
			m_warship->m_can_open_new_menu = false;
		}
		else if (ship_in_combat_range != NULL)
		{
			m_menu = Menu_CrewUnboard;
			m_warship->m_crew_unboard_interface.Init(m_warship, NULL, ship_in_combat_range);
			m_warship->m_can_open_new_menu = false;
		}
		else if (location != NULL && location->m_visited_countdown == 0 && m_warship->m_sonar >= location->m_depth)
		{
			m_menu = Menu_CrewUnboard;
			m_warship->m_crew_unboard_interface.Init(m_warship, m_warship->m_tile->m_location, NULL);
			m_warship->m_can_open_new_menu = false;
		}
	}

	//Help interface
	if (hovered != NULL && hovered->m_UI_type == UI_Commodity)
	{
		Commodity* commodity = (Commodity*)hovered;
		m_help_interface.Init(commodity->m_display_name, commodity->m_description, sf::Vector2f(commodity->m_position.x - RESOURCES_ICON_SIZE * 0.5, commodity->m_position.y + RESOURCES_ICON_SIZE * 0.5));
	}
	else if (hovered != NULL && hovered->m_UI_type == UI_Resource)
	{
		Resource* resource = (Resource*)hovered;
		
		if (resource->m_type == Resource_Fidelity)
		{
			string description = "Crew fidelity is important, as low fidelity will sometimes occasion mutiny.";
			m_help_interface.Init(resource->m_display_name, description, sf::Vector2f(resource->m_position.x - resource->m_shape_container.getSize().x * 0.5, resource->m_position.y + resource->m_shape_container.getSize().y * 0.5 * 0.5 + 16));
		}
		else if (resource->m_type == Resource_Days)
		{
			string description = "Days are passing, and the Royal Navy might just be closing on you very soon.";
			m_help_interface.Init(resource->m_display_name, description, sf::Vector2f(resource->m_position.x - resource->m_shape_container.getSize().x * 0.5, resource->m_position.y + resource->m_shape_container.getSize().y * 0.5 * 0.5 + 16));
		}
		else
		{
			string description = "Your holds of resources.";
			m_help_interface.Init(resource->m_display_name, description, sf::Vector2f(resource->m_position.x - RESOURCES_ICON_SIZE * 0.5, resource->m_position.y + RESOURCES_ICON_SIZE * 0.5 + 6));
		}
	}
	else if (hovered != NULL && hovered->m_UI_type == UI_Rudder)
	{
		Rudder* rudder = (Rudder*)hovered;
		string display_name = (*CurrentGame).m_dico_ship_systems[rudder->m_tile->m_system];
		string activated = m_warship->IsSystemOperational(System_Navigation, rudder->m_tile) == true ? "operated" : "not operated";
		string description = "Status: " + activated + "\n+" + std::to_string((int)(100 * NAVIGATION_DODGE_CHANCE)) + "% dodge chance (if operated by a crew member).\nThe operating crew also adds his Nagivation skill % to dodge chances.";
		m_help_interface.Init(display_name, description, sf::Vector2f(rudder->m_position.x - ROOMTILE_SIZE * 0.5, rudder->m_position.y + ROOMTILE_SIZE * 0.5));
	}
	else if (hovered != NULL && hovered->m_UI_type == UI_Engine)
	{
		Engine* engine = (Engine*)hovered;
		string display_name = (*CurrentGame).m_dico_ship_systems[engine->m_tile->m_system];
		string activated = m_warship->IsSystemOperational(System_Engine, engine->m_tile) == true ? "operated" : "not operated";
		string description = "Status: " + activated + "\nCharges the Flee thrusters (if operated by a crew member).\nWhen fully charged, right-click on the Engine to flee an ungoing combat.\n\n+" + std::to_string((int)(100 * ENGINE_DODGE_CHANCE)) + "% dodge chance per engine (if operated by a crew member).\nThe operating crew also adds his Engine skill % to dodge chances.";
		m_help_interface.Init(display_name, description, sf::Vector2f(engine->m_position.x - ROOMTILE_SIZE * 0.5, engine->m_position.y + ROOMTILE_SIZE * 0.5));
	}
	else if (hovered != NULL && hovered->m_UI_type == UI_Weapon)
	{
		Weapon* weapon = (Weapon*)hovered;
		string display_name = (*CurrentGame).m_dico_ship_weapons[weapon->m_type];
		string activated = m_warship->IsSystemOperational(System_Weapon, weapon->m_tile) == true ? "operated" : "not operated";
		string description = "Status: " + activated + "\nLoads and fire on enemy ships during combat (as long as operated by a crew member).\nSelect it with left-click and right-click to assign a target during combat.";
		if (weapon->m_type == Weapon_Cannon)
		{
			description += "\n\nCannons aims at random into a target room, dealing great damage to the hull and crew members.";
		}
		else if (weapon->m_type == Weapon_Torpedo)
		{
			description += "\n\nTorpedo aims at random onto a target hull, piercing it and causing flooding into the enemy ship.";
		}
		else if (weapon->m_type == Weapon_Shrapnel)
		{
			description += "\n\nShrapnel aims at random into a target room, dealing spread damage to crew members, but not to the hull.";
		}

		m_help_interface.Init(display_name, description, sf::Vector2f(weapon->m_position.x - ROOMTILE_SIZE * 0.5, weapon->m_position.y + ROOMTILE_SIZE * 0.5));
	}
	else if (hovered != NULL && hovered->m_UI_type == UI_ResourceUpkeep)
	{
		Resource* resource = (Resource*)hovered;
		string description = "Crew upkeep costs resources every day.\nEach day passed with the upkeep paid increase crew fidelity.\nUpkeep costs unpaid decrease crew fidelity.";
		m_help_interface.Init(resource->m_display_name, description, sf::Vector2f(resource->m_position.x - RESOURCES_ICON_SIZE * 0.5, resource->m_position.y + RESOURCES_ICON_SIZE * 0.5 + 6));
	}
	else if (m_help_interface.m_panel != NULL)
	{
		m_help_interface.Destroy();
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
				m_warship->m_crew_unboarding.push_back(crew);
				m_warship->m_crew_unboard_interface.AddCrewToInterface(crew);
			}
			//remove crew from interface
			else if (hovered->m_UI_type == UI_CrewMemberUnboarding)
			{
				CrewMember* crew = (CrewMember*)hovered;
				m_warship->m_crew_unboard_interface.RemoveCrewFromInterface(crew);

				vector<CrewMember*> old_crew;
				for (vector<CrewMember*>::iterator it = m_warship->m_crew_unboarding.begin(); it != m_warship->m_crew_unboarding.end(); it++)
				{
					old_crew.push_back(*it);
				}
				m_warship->m_crew_unboarding.clear();
				for (vector<CrewMember*>::iterator it = old_crew.begin(); it != old_crew.end(); it++)
				{
					if ((*it) != crew)
					{
						m_warship->m_crew_unboarding.push_back(*it);
					}
				}
			}
		}

		Choice* choice = m_warship->m_crew_unboard_interface.Update(deltaTime);

		//choice made?
		if (choice != NULL)
		{
			//pay costs
			for (int j = 0; j < NB_RESOURCES_TYPES; j++)
			{
				if (j != Resource_Days && j != Resource_Fidelity)
				{
					m_warship->AddResource(ResourceType(j), - choice->m_cost[j]);
				}
				else if (j == Resource_Days)
				{
					SpendDays(choice->m_cost[j], true);
				}
			}

			if (choice->m_cost_commodity != Commodity_None)
			{
				RemoveCommodity(choice->m_cost_commodity);
			}

			//get reward
			string rewardID = choice->RandomizeRewardID();
			Reward* reward = GenerateReward(rewardID, m_warship->m_crew_unboard_interface.m_location, m_warship->m_crew_unboard_interface.m_other_ship, choice->m_gauge_value, choice->m_gauge_value_max);

			//reward is not empty? open the reward interface.
			if (reward != NULL && reward->m_string.compare("0") != 0)
			{
				//crew killed or recruited?
				int size = reward->m_resources.size();
				for (int i = 0; i < size; i++)
				{
					if (reward->m_resources[i].first == Resource_Crew)
					{
						//crew killed
						if (reward->m_resources[i].second < 0)
						{
							//cannot kill more than unboarded
							for (int k = 0; k < -reward->m_resources[i].second; k++)
							{
								if (m_warship->m_crew_unboarding.size() > k)
								{
									m_warship->m_reward_interface.m_crew_killed.push_back(m_warship->m_crew_unboarding[k]);
								}
							}
							reward->m_resources[i].second = -m_warship->m_reward_interface.m_crew_killed.size();
						}
						else//crew recruited
						{
							for (int k = 0; k < reward->m_resources[i].second; k++)
							{
								m_warship->m_reward_interface.m_crew_recruited.push_back(new CrewMember(Crew_Civilian, m_warship->m_alliance));
							}
						}
					}
				}

				m_warship->m_reward_interface.Init(m_warship, reward);
				m_menu = Menu_Reward;
			}
			else//no reward interface
			{
				//set next menu
				SetNextMenuFromReward(reward);
			}
		}
	}
	else if (m_menu == Menu_Reward)
	{
		if (m_warship->m_reward_interface.Update() == true)
		{
			//give reward
			for (vector<pair<ResourceType, int> >::iterator it = m_warship->m_reward_interface.m_reward->m_resources.begin(); it != m_warship->m_reward_interface.m_reward->m_resources.end(); it++)
			{
				m_warship->AddResource((*it).first, (*it).second);
			}

			//secret location revealed
			if (m_warship->m_reward_interface.m_reward->m_DMS_location != NULL)
			{
				DMS_Coord& dms = *m_warship->m_reward_interface.m_reward->m_DMS_location;
				(*CurrentGame).m_waterzones[dms.m_degree_x][dms.m_degree_y]->m_watertiles[dms.m_minute_x][dms.m_minute_y]->m_location->m_known = true;
				printf("");
			}

			//commodity
			if (m_warship->m_reward_interface.m_reward->m_commodity != Commodity_None)
			{
				AddCommodityToHolds(m_warship->m_reward_interface.m_reward->m_commodity);
			}

			//crew recruited
			for (vector<CrewMember*>::iterator it = m_warship->m_reward_interface.m_crew_recruited.begin(); it != m_warship->m_reward_interface.m_crew_recruited.end(); it++)
			{
				m_warship->AddCrewMember(*it, m_warship->m_rooms.front());
			}

			//crew killed
			for (vector<CrewMember*>::iterator it = m_warship->m_reward_interface.m_crew_killed.begin(); it != m_warship->m_reward_interface.m_crew_killed.end(); it++)
			{
				vector<CrewMember*> old_crew;
				for (vector<CrewMember*>::iterator it2 = m_warship->m_crew[0].begin(); it2 != m_warship->m_crew[0].end(); it2++)
				{
					old_crew.push_back(*it2);
				}
				m_warship->m_crew[0].clear();
				for (vector<CrewMember*>::iterator it2 = old_crew.begin(); it2 != old_crew.end(); it2++)
				{
					if (*it == *it2)
					{
						continue;
					}
					else
					{
						m_warship->m_crew[0].push_back(*it2);
					}
				}
			}

			//set next menu
			SetNextMenuFromReward(m_warship->m_reward_interface.m_reward);

			//kill reward interface
			m_warship->m_reward_interface.Destroy();
		}
	}
	else if (m_menu == Menu_Dockyard)
	{
		string buy_upgrade = m_warship->m_dockyard_interface.Update(deltaTime);
		if (buy_upgrade.empty() == false)
		{
			//apply upgrade
			m_warship->ApplyUpgrade(buy_upgrade);
		}

		if (m_warship->m_dockyard_interface.m_leave_button->ButtonUpdate() == true && (*CurrentGame).m_mouse_click == Mouse_LeftClick)
		{
			//close interface
			m_warship->m_dockyard_interface.Destroy();
			m_menu = Menu_CrewUnboard;
		}
	}

	//close crew unboard interface?
	if (m_menu == Menu_CrewUnboardLeave)
	{
		CloseCrewUnboardInterface();
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
		WaterTile* focused_water_tile = NULL;
		for (vector<WaterTile*>::iterator it = m_warship->m_tiles_can_be_seen.begin(); it != m_warship->m_tiles_can_be_seen.end(); it++)
		{
			if (((*CurrentGame).m_hovered_ui != NULL && (*CurrentGame).m_hovered_ui == *it) || ((*CurrentGame).m_selected_ui != NULL && (*CurrentGame).m_selected_ui == *it))
			{
				focused_water_tile = *it;
			}
			else
			{
				(*it)->Draw((*CurrentGame).m_mainScreen);
				//secret locations (if known)
				if ((*it)->m_location != NULL && (*it)->m_location->m_type != Location_Seaport && (*it)->m_location->m_known == true && (*it)->m_location->m_visited_countdown == 0)
				{
					(*it)->m_location->Draw((*CurrentGame).m_mainScreen);
				}
			}
		}
		if (focused_water_tile != NULL)
		{
			focused_water_tile->Draw((*CurrentGame).m_mainScreen);
			//secret locations (if known)
			if (focused_water_tile->m_location != NULL && focused_water_tile->m_location->m_type != Location_Seaport && focused_water_tile->m_location->m_known == true && focused_water_tile->m_location->m_visited_countdown == 0)
			{
				focused_water_tile->m_location->Draw((*CurrentGame).m_mainScreen);
			}
		}

		//islands and ports
		for (vector<Seaport*>::iterator it = m_seaports.begin(); it != m_seaports.end(); it++)
		{
			if ((*it)->m_tile->m_can_be_seen == true)
			{
				(*it)->Draw((*CurrentGame).m_mainScreen);
				//island name
				(*it)->m_island->Draw((*CurrentGame).m_mainScreen);
			}
		}
	}

	//ships
	int shipsVectorSize = m_ships.size();
	for (int i = shipsVectorSize - 1; i >= 0; i--)
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
				if (ship == m_warship)
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

		if (ship == m_warship && ship->m_is_fleeing == false)
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
	else if (m_menu == Menu_Dockyard)
	{
		m_warship->m_dockyard_interface.Draw((*CurrentGame).m_mainScreen);
	}

	//HUD - resources interface
	m_resources_interface.Draw((*CurrentGame).m_mainScreen);

	//HUD - contextual order
	if (m_contextual_order->m_type != Order_None && m_warship->m_is_fleeing == false && m_warship->m_sinking_timer <= 0)
	{
		m_contextual_order->Draw((*CurrentGame).m_mainScreen);
	}

	//HUD - help interface
	if (m_help_interface.m_panel != NULL)
	{
		m_help_interface.Draw((*CurrentGame).m_mainScreen);
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
				m_warship->m_can_open_new_menu = false;
				m_scale = Scale_Strategic;
			}
		}

		return true;
	}

	return (m_scale == Scale_Tactical);
}

void Gameloop::StartCombatWithShip(Ship* enemy_ship)
{
	m_tactical_ship = enemy_ship;

	//stop strategic movement
	m_warship->m_destination = NULL;
	m_warship->m_speed = sf::Vector2f(0, 0);
	m_warship->m_current_path.clear();

	m_warship->InitCombat();//init cooldowns
	enemy_ship->BuildShip();//generate enemy ship's rooms, crew and weapons
	enemy_ship->InitCombat();//init cooldowns
	enemy_ship->m_combat_interface.Init(enemy_ship, enemy_ship->m_alliance, enemy_ship->m_display_name, enemy_ship->m_type);//init enemy interface

	//fill enemy ship room tiles for pathfind
	(*CurrentGame).m_enemy_tiles.clear();
	for (vector<Room*>::iterator it = enemy_ship->m_rooms.begin(); it != enemy_ship->m_rooms.end(); it++)
	{
		for (vector<RoomTile*>::iterator it2 = (*it)->m_tiles.begin(); it2 != (*it)->m_tiles.end(); it2++)
		{
			(*CurrentGame).m_enemy_tiles.push_back(*it2);
		}
	}

	(*CurrentGame).PlayMusic(Music_Combat);
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

Ship* Gameloop::IsDMSInCombatRange(DMS_Coord DMS_hovered, bool ally_included)
{
	for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
	{
		if ((*it) == m_warship || (*it)->m_can_be_seen == false || ((*it)->m_alliance == Alliance_Ally && ally_included == false))
		{
			continue;
		}

		//in range for combat?
		float posxA = DMS_hovered.m_minute_x * 60.f + DMS_hovered.m_second_x;
		float posxB = (*it)->m_DMS.m_minute_x * 60.f + (*it)->m_DMS.m_second_x;
		if (abs(posxA - posxB) > 0.f * NB_WATERTILE_SUBDIVISION)
		//if (abs(posxA - posxB) > 1.f * NB_WATERTILE_SUBDIVISION)
		{
			continue;
		}

		float posyA = DMS_hovered.m_minute_y * 60.f + DMS_hovered.m_second_y;
		float posyB = (*it)->m_DMS.m_minute_y * 60.f + (*it)->m_DMS.m_second_y;
		if (abs(posyA - posyB) > 0.f * NB_WATERTILE_SUBDIVISION)
		//if (abs(posyA - posyB) > 1.f * NB_WATERTILE_SUBDIVISION)
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
		
		for (vector<Upgrade*>::iterator it = warship->m_upgrades.begin(); it != warship->m_upgrades.end(); it++)
		{
			data << "Upgrade " << (*it)->m_type << endl;
		}

		for (int j = 0; j < 2; j++)
		{
			for (vector<CrewMember*>::iterator it = warship->m_crew[j].begin(); it != warship->m_crew[j].end(); it++)
			{
				data << "Crew " << StringReplace((*it)->m_display_name, " ", "_") << " " << (*it)->m_type << " " << (*it)->m_skin << " " << (*it)->m_race << " " << (*it)->m_health << " " << (*it)->m_health_max;
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

		for (vector<Commodity*>::iterator it = warship->m_holds.begin(); it != warship->m_holds.end(); it++)
		{
			data << "Holds " << (*it)->m_type;
			data << endl;
		}

		for (vector<Seaport*>::iterator it = m_seaports.begin(); it != m_seaports.end(); it++)
		{
			ostringstream island_name;
			island_name << StringReplace((*it)->m_display_name, " ", "_");
			data << "Island " << (*it)->m_tile->m_DMS.m_degree_x << " " << (*it)->m_tile->m_DMS.m_degree_y << " " << (*it)->m_tile->m_DMS.m_minute_x << " " << (*it)->m_tile->m_DMS.m_minute_y << " " << (int)((*it)->m_seaport_type) << " " << (*it)->m_visited_countdown;
			data << " " << (*it)->m_island->m_upcorner_x << " " << (*it)->m_island->m_upcorner_y << " " << (*it)->m_island->m_width << " " << (*it)->m_island->m_height << " " << (*it)->m_island->m_skin << " " << island_name.str();
			data << endl;
		}

		for (vector<Ship*>::iterator it = m_ships.begin(); it != m_ships.end(); it++)
		{
			if ((*it) == m_warship)
			{
				continue;
			}

			ostringstream ship_name;
			ship_name << StringReplace((*it)->m_display_name, " ", "_");
			data << "Ship " << (*it)->m_DMS.m_minute_x << " " << (*it)->m_DMS.m_minute_y;
			data << " " << (int)((*it)->m_type) << " " << (int)((*it)->m_alliance) << " " << ship_name.str();
			data << endl;
		}

		for (int i = 0; i < NB_SECRET_LOCATION_TYPES; i++)
		{
			for (vector<Location*>::iterator it = m_secret_locations[i].begin(); it != m_secret_locations[i].end(); it++)
			{
				switch (i)
				{
					case Location_Wreck:
					{
						data << "Wreck ";
						break;
					}
					case Location_SeaMonster:
					{
						data << "SeaMonster ";
						break;
					}
					case Location_Fish:
					{
						data << "Fish ";
						break;
					}
				}

				DMS_Coord& dms = (*it)->m_tile->m_DMS;
				data << dms.m_degree_x << " " << dms.m_minute_x << " " << dms.m_second_x << " " << dms.m_degree_y << " " << dms.m_minute_y << " " << dms.m_second_y << " " << (*it)->m_depth << " " << (*it)->m_visited_countdown;
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
		warship->SetDMSCoord(DMS_Coord(DMS_degree_x, DMS_minute_x, 0, DMS_degree_y, DMS_minute_y, 0));

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

			if (t.compare("Upgrade") == 0)
			{
				string upgrade_type;
				std::istringstream(line) >> t >> upgrade_type;
				
				warship->ApplyUpgrade(upgrade_type);
			}
			else if (t.compare("Crew") == 0)
			{
				string name;
				int type, race, skin;
				
				int skills[NB_CREW_SKILLS];
				bool prisoner;
				int health, health_max, coord_x, coord_y;
				std::istringstream(line) >> t >> name >> type >> skin >> race >> health >> health_max >> skills[Skill_Gunner] >> skills[Skill_Fishing] >> skills[Skill_Combat] >> skills[Skill_Navigation] >> skills[Skill_Engine] >> skills[Skill_Diving] >> (bool)prisoner >> coord_x >> coord_y;
				
				CrewMember* crew = new CrewMember((CrewMemberType)type, Alliance_Player, (CrewMemberRace)race, prisoner);
				
				crew->m_skin = skin;
				crew->setAnimationLine(AnimationDirection_Down + (NB_ANIMATION_DIRECTIONS * crew->m_skin));
				crew->m_display_name = StringReplace(name, "_", " ");
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
			else if (t.compare("Holds") == 0)
			{
				int commodity_type;
				std::istringstream(line) >> t >> commodity_type;
				AddCommodityToHolds((CommodityType)commodity_type);
			}
			else if (t.compare("Island") == 0)
			{
				int seaport_coord_x, seaport_coord_y, seaport_type, seaport_visited_countdown, island_upcorner_x, island_upcorner_y, island_width, island_height, island_skin, zone_coord_x, zone_coord_y;
				string name;
				std::istringstream(line) >> t >> zone_coord_x >> zone_coord_y >> seaport_coord_x >> seaport_coord_y >> seaport_type >> seaport_visited_countdown >> island_upcorner_x >> island_upcorner_y >> island_width >> island_height >> island_skin >> name;
				name = StringReplace(name, "_", " ");

				Seaport* seaport = new Seaport(seaport_coord_x, seaport_coord_y, zone_coord_x, zone_coord_y, (SeaportType)seaport_type);
				seaport->m_display_name = name;
				seaport->m_text.setString(name);
				seaport->m_visited_countdown = seaport_visited_countdown;
				Island* island = seaport->AddIsland(island_upcorner_x, island_upcorner_y, island_width, island_height, island_skin);
				m_seaports.push_back(seaport);
			}
			else if (t.compare("Ship") == 0)
			{
				int minute_x, minute_y, type, alliance;
				string name;
				std::istringstream(line) >> t >> minute_x >> minute_y >> type >> alliance >> name;
				name = StringReplace(name, "_", " ");

				Ship* ship = new Ship(DMS_Coord{ 0, minute_x, 0, 0, minute_y, 0 }, (ShipType)type, (ShipAlliance)alliance);
				ship->m_display_name = name;
				m_ships.push_back(ship);
			}
			else if (t.compare("Wreck") == 0 || t.compare("SeaMonster") == 0 || t.compare("Fish") == 0)
			{
				int degree_x, minute_x, degree_y, minute_y, depth, visited_countdown;
				float second_x, second_y;
				std::istringstream(line) >> t >> degree_x >> minute_x >> second_x >> degree_y >> minute_y >> second_y >> depth >> visited_countdown;

				DMS_Coord* dms = new DMS_Coord(degree_x, minute_x, second_x, degree_y, minute_y, second_y);
				WaterTile* tile = (*CurrentGame).m_waterzones[degree_x][degree_y]->m_watertiles[minute_x][minute_y];
				Location* location;

				if (t.compare("Wreck") == 0)
				{
					location = new Location(Location_Wreck, tile);
					m_secret_locations[Location_Wreck].push_back(location);
				}
				else if (t.compare("SeaMonster") == 0)
				{
					location = new Location(Location_SeaMonster, tile);
					m_secret_locations[Location_SeaMonster].push_back(location);
				}
				else if (t.compare("Fish") == 0)
				{
					location = new Location(Location_Fish, tile);
					m_secret_locations[Location_Fish].push_back(location);
				}

				location->m_depth = depth;
				location->m_visited_countdown = visited_countdown;
				tile->m_location = location;
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

void Gameloop::SpendDays(int days, bool skip_time)
{
	if (days == 0)
	{
		return;
	}

	m_warship->PayUpkeepCost(days);

	//refresh seaports resources
	for (vector<Seaport*>::iterator it = m_seaports.begin(); it != m_seaports.end(); it++)
	{
		if ((*it)->m_visited_countdown > 0)
		{
			(*it)->m_visited_countdown += days;

			if ((*it)->m_visited_countdown >= RESOURCES_REFRESH_RATE_IN_DAYS)
			{
				(*it)->m_visited_countdown = 0;
			}
		}
	}
}

void Gameloop::GenerateRandomIslands(int zone_coord_x, int zone_coord_y)
{
	//init islands
	for (int t = 0; t < NB_SEAPORT_TYPES; t++)
	{
		int r = 0;
		switch (t)
		{
			case Seaport_Small://1x1
			{
				r = RandomizeIntBetweenValues(25, 30);
				break;
			}
			case Seaport_Medium://1x2, 2x1, 2x2
			{
				r = RandomizeIntBetweenValues(20, 25);
				break;
			}
			case Seaport_Large://2x3, 3x2, 3x3
			{
				r = RandomizeIntBetweenValues(15, 20);
				break;
			}
		}

		for (int i = 0; i < r; i++)
		{
			int x = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
			int y = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);

			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
			while (tile->m_type != Water_Empty || tile->m_location != NULL)
			{
				x = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
				y = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
				tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
			}

			//island size
			int t = RandomizeIntBetweenValues(0, NB_SEAPORT_TYPES - 1);
			int width, height;
			switch (t)
			{
			case Seaport_Small://1x1
			{
				width = 1;
				height = 1;
				break;
			}
			case Seaport_Medium://1x2, 2x1, 2x2
			{
				width = RandomizeIntBetweenValues(1, 2);
				height = width == 1 ? 2 : RandomizeIntBetweenValues(1, 2);
				break;
			}
			case Seaport_Large://2x3, 3x2, 3x3
			{
				width = RandomizeIntBetweenValues(2, ISLAND_SIZE_MAX);
				height = width == 2 ? ISLAND_SIZE_MAX : RandomizeIntBetweenValues(2, ISLAND_SIZE_MAX);
				break;
			}
			}

			//randomize the position of the island around the seaport
			vector<WaterTile*> possible_candidates;
			for (int j = 0; j < width; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					//check offgrid && not adjacent to island/port
					int y_ = k == 0 ? -1 : height;
					if (CanIslandBeCreatedInArea(x - j, y + y_, width, height, zone_coord_x, zone_coord_y) == true)
					{
						tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x - j][y + y_];
						possible_candidates.push_back(tile);
					}
				}
			}

			//check offgrid && not adjacent to island/port
			for (int j = 0; j < height; j++)
			{
				for (int k = 0; k < 2; k++)
				{
					int x_ = k == 0 ? 1 : -width;
					if (CanIslandBeCreatedInArea(x + x_, y + j, width, height, zone_coord_x, zone_coord_y) == true)
					{
						tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x + x_][y + j];
						possible_candidates.push_back(tile);
					}
				}
			}

			//create seaport and island
			int candidates_size = possible_candidates.size();
			if (candidates_size > 0)
			{
				WaterTile* candidate_tile = possible_candidates[RandomizeIntBetweenValues(0, candidates_size - 1)];
				Seaport* seaport = new Seaport(x, y, zone_coord_x, zone_coord_y, (SeaportType)t);
				(*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y]->m_location = seaport;
				m_seaports.push_back(seaport);
				seaport->AddIsland(candidate_tile->m_coord_x, candidate_tile->m_coord_y, width, height, RandomizeIntBetweenValues(0, NB_ISLAND_SKINS - 1));
			}
		}
	}
}

void Gameloop::GenerateRandomSecretLocations(int zone_coord_x, int zone_coord_y)
{
	for (int i = 0; i < NB_SECRET_LOCATION_TYPES; i++)
	{
		int r = 0;
		switch (i)
		{
			case Location_Wreck:
			case Location_SeaMonster:
			{
				r = 10;
				break;
			}
			case Location_Fish:
			{
				r = 250;
				break;
			}
		}

		for (int j = 0; j < r; j++)
		{
			int x = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
			int y = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);

			WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
			while (tile->m_type != Water_Empty || tile->m_location != NULL || tile == m_warship->m_tile)
			{
				x = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
				y = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
				tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
			}

			Location* location = new Location((LocationType)i, tile);
			tile->m_location = location;
			m_secret_locations[i].push_back(location);
		}
	}
}

void Gameloop::GenerateRandomShips(int zone_coord_x, int zone_coord_y)
{
	for (int a = Alliance_Enemy; a <= Alliance_Ally; a++)
	{
		for (int i = Ship_FirstClass; i < NB_SHIP_TYPES; i++)
		{
			int r = 0;
			switch (i)
			{
				case Ship_FirstClass:
				{
					r = a == Alliance_Enemy ? 5 : 0;
					break;
				}
				case Ship_SecondClass:
				{
					r = a == Alliance_Enemy ? 30 : 30;
					break;
				}
			}

			for (int j = 0; j < r; j++)
			{
				int x = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
				int y = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);

				WaterTile* tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
				while (tile->m_type != Water_Empty || tile == m_warship->m_tile)
				{
					x = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
					y = RandomizeIntBetweenValues(0, NB_WATERTILE_SUBDIVISION - 1);
					tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[x][y];
				}

				m_ships.push_back(new Ship(DMS_Coord{ 0, x, 0, 0, y, 0 }, (ShipType)i, (ShipAlliance)a));
			}
		}
	}
}

bool Gameloop::CanIslandBeCreatedInArea(int upcorner_x, int upcorner_y, int width, int height, int zone_coord_x, int zone_coord_y)
{
	//test 1: on grid
	bool is_on_grid = (upcorner_x >= 0 && upcorner_x + width - 1 <= NB_WATERTILE_SUBDIVISION - 1 && upcorner_y <= NB_WATERTILE_SUBDIVISION - 1 && upcorner_y >= height - 1);
	if (is_on_grid == false)
	{
		return false;
	}

	//test 2: area is free, with 2 tiles margins around the island
	bool is_free = true;
	for (int i = -2; i < width + 2; i++)
	{
		for (int j = -2; j < height + 2; j++)
		{
			WaterTile* tile = NULL;
			if (upcorner_x + i >= 0 && upcorner_x + i <= NB_WATERTILE_SUBDIVISION - 1 && upcorner_y - j >= 0 && upcorner_y - j <= NB_WATERTILE_SUBDIVISION - 1)
			{
				tile = (*CurrentGame).m_waterzones[zone_coord_x][zone_coord_y]->m_watertiles[upcorner_x + i][upcorner_y - j];
			}
			
			if (tile != NULL && (tile->m_type != Water_Empty || tile->m_location != NULL))
			{
				is_free = false;
			}
		}
	}

	return is_on_grid && is_free;
}

Reward* Gameloop::GenerateReward(string rewardID, Location* location, Ship* other_ship, int gauge, int gauge_max)
{
	if (rewardID.empty() == false)
	{
		Reward* reward = new Reward();

		//chance of failing?
		if (location != NULL && location->m_type == Location_SeaMonster)
		{
			int monster_strenght = (int)Lerp(location->m_depth, SEAMONSTER_DEPTH_MIN, SEAMONSTER_DEPTH_MAX, 0, gauge_max);
			if (monster_strenght > gauge || RandomizeFloatBetweenValues(0, 1) < SEAMONSTER_LOSE_PROBABILITY)
			{
				//lose combat against monster
				int delta = monster_strenght / gauge;
				Bound(delta, 0, SEAMONTER_CREW_KILLS_MAX);

				pair<ResourceType, int> resource;
				resource.first = Resource_Crew;
				resource.second = - delta;
				reward->m_resources.push_back(resource);
				reward->m_string = "The massive ojbect turns out to be a dangerous\nsea monster!\nAfter a fierce fight, your divers get torn apart.";

				return reward;
			}
		}

		int line = 0;
		for (vector< vector<string> >::iterator it = (*CurrentGame).m_rewards_config.begin(); it != (*CurrentGame).m_rewards_config.end(); it++)
		{
			if ((*it).front().compare(rewardID) == 0)
			{
				//get rewards
				for (int j = 0; j < NB_RESOURCES_TYPES; j++)
				{
					int value = stoi((*CurrentGame).m_rewards_config[line][Reward_Gold + j]);

					if (value == 0)
					{
						continue;
					}

					if (j == Resource_Gold || j == Resource_Fish || j == Resource_Mech)
					{
						//add random + pro rata of skills invested (if any)
						float cooldown = location->m_visited_countdown == 0 ? 1.f : (1.f * location->m_visited_countdown / RESOURCES_REFRESH_RATE_IN_DAYS);

						if (gauge_max == 0)
						{
							value = (int)(1.f * value * RandomizeFloatBetweenValues(0.8, 1.2) * cooldown);
						}
						else
						{
							value = (int)(1.f * value * (1.f * gauge / gauge_max) * RandomizeFloatBetweenValues(0.8, 1.2) * cooldown);
						}
					}

					pair<ResourceType, int> resource;
					resource.first = (ResourceType)j;
					resource.second = value;
					reward->m_resources.push_back(resource);
				}

				//secret wreck location
				int secret = stoi((*CurrentGame).m_rewards_config[line][Reward_SecretWreck]);
				if (secret == 1)
				{
					if (m_secret_locations[Location_Wreck].size() > 0)
					{
						//get a secret wreck location from the water zone map
						int r = RandomizeIntBetweenValues(0, m_secret_locations[Location_Wreck].size() - 1);
						reward->m_DMS_location = new DMS_Coord(m_secret_locations[Location_Wreck][r]->m_tile->m_DMS);
					}
				}

				//commodity
				reward->m_commodity = (CommodityType)stoi((*CurrentGame).m_rewards_config[line][Reward_Commodity]);

				//combat
				int combat = stoi((*CurrentGame).m_rewards_config[line][Reward_Combat]);
				if (combat == 1)
				{
					reward->m_combat_ship = other_ship;
				}

				//dockyard
				int dockyard = stoi((*CurrentGame).m_rewards_config[line][Reward_Dockyard]);
				if (dockyard == 1)
				{
					reward->m_dockyard = location;
				}

				//reward text
				reward->m_string = (*CurrentGame).m_rewards_config[line][Reward_Text];
				reward->m_string = StringReplace(reward->m_string, "_", " ");
				reward->m_string = StringCut(reward->m_string, 48);

				return reward;
			}
			else
			{
				line++;
			}
		}	
	}
	else
	{
		return NULL;
	}
}

void Gameloop::SetAIStrategicalDestination(Ship* ship, DMS_Coord warship_DMS)
{
	//already has a path?
	if (ship->m_current_path.empty() == false)
	{
		return;
	}

	//set a final destination (generally, a medium or large seaport)
	if (ship->m_destination_long == NULL)
	{
		//we want to select the 3 closest seaports, and then we'll randomize one destination among the 3.
		vector<Seaport*> possible_candidates;
		for (int i = 0; i < 3; i++)
		{
			int distance_min = -1;
			Seaport* candidate = NULL;
			for (vector<Seaport*>::iterator it = m_seaports.begin(); it != m_seaports.end(); it++)
			{
				//AI ships only navigate to medium or large seaports
				if ((*it)->m_seaport_type == Seaport_Small || ship->m_seaport == (*it))
				{
					continue;
				}

				bool found = false;
				for (vector<Seaport*>::iterator it2 = possible_candidates.begin(); it2 != possible_candidates.end(); it2++)
				{
					if (*it == *it2)
					{
						found = true;
						break;
					}
				}
				if (found == true)
				{
					continue;
				}

				int dist = ship->GetDistanceToWaterTile((*it)->m_tile);
				if (distance_min < 0 || dist < distance_min)
				{
					distance_min = dist;
					candidate = *it;
				}
			}

			if (candidate != NULL)
			{
				possible_candidates.push_back(candidate);
			}
		}

		//randomize destination
		if (possible_candidates.empty() == true)
		{
			return;
		}
		int r = RandomizeIntBetweenValues(0, possible_candidates.size() - 1);
		Seaport* selected = possible_candidates[r];
		ship->m_destination_long = selected->m_tile;
	}

	//enemy aggro?
	bool aggro = false;
	if (ship->m_alliance == Alliance_Enemy && ship->m_tile->m_can_be_seen == true && m_warship->m_has_played == true && ship->m_has_played == false)
	{
		float dist = ship->GetDistanceFloatToWaterTile(m_warship->m_tile);
		float proba_aggro = (1.f * ship->m_moves_max - dist + 1) / ship->m_moves_max;
		Boundf(proba_aggro, 0.f, 0.95f);
		aggro = proba_aggro >= RandomizeFloatBetweenValues(0, 1);
	}
	
	if (aggro == true)
	{
		ship->m_destination = m_warship->m_destination != NULL ? m_warship->m_destination : m_warship->m_tile;
	}
	//get sub-destination from long destination
	else if (ship->m_destination == NULL && ship->m_destination_long != NULL && m_warship->m_has_played == false)
	{
		ship->m_destination = ship->m_destination_long;
	}

	//give order to sail to destination (when the player starts to move too)
	if (ship->m_destination != NULL && m_warship->m_has_played == true)
	{
		ship->UpdateAITilesCanBeSeen();
		ship->SetSailsToWaterTile(ship->m_destination, warship_DMS);
	}
}

bool Gameloop::AddCommodityToHolds(CommodityType commodity_type)
{
	if (m_warship->m_holds.size() == m_warship->m_holds_capacity)
	{
		return false;
	}

	Texture* texture = TextureLoader::getInstance()->loadTexture((*CurrentGame).m_commodities_config[commodity_type - 1][Commodity_Texture], RESOURCES_ICON_SIZE, RESOURCES_ICON_SIZE);
	Commodity* commodity = new Commodity(commodity_type);
	commodity->setAnimation(texture, 1, 1);
	m_warship->m_holds.push_back(commodity);

	return true;
}

void Gameloop::RemoveCommodity(CommodityType commodity_type)
{
	vector<Commodity*> old_holds;
	for (vector<Commodity*>::iterator it = m_warship->m_holds.begin(); it != m_warship->m_holds.end(); it++)
	{
		old_holds.push_back(*it);
	}
	m_warship->m_holds.clear();

	bool removed = false;
	for (vector<Commodity*>::iterator it = old_holds.begin(); it != old_holds.end(); it++)
	{
		if (removed == true || (*it)->m_type != commodity_type)
		{
			m_warship->m_holds.push_back(*it);
		}
		else
		{
			removed = true;
			delete *it;
		}
	}
}

void Gameloop::CloseCrewUnboardInterface()
{
	m_menu = Menu_None;

	//Refresh location's cooldown
	if (m_warship->m_crew_unboard_interface.m_location != NULL)
	{
		m_warship->m_crew_unboard_interface.m_location->m_visited_countdown = 1;
	}
	
	//Clean unboarding interface and crew
	m_warship->m_crew_unboard_interface.Destroy();
	m_warship->m_crew_unboarding.clear();
}

void Gameloop::SetNextMenuFromReward(Reward* reward)
{
	//init combat?
	if (reward !=NULL && reward->m_combat_ship != NULL)
	{
		m_scale = Scale_Tactical;
		StartCombatWithShip(reward->m_combat_ship);

		m_menu = Menu_CrewUnboardLeave;
	}
	else if (reward != NULL && reward->m_dockyard != NULL)//open dockyard menu?
	{
		m_menu = Menu_Dockyard;
		m_warship->m_dockyard_interface.Init(m_warship, reward->m_dockyard);
	}
	else
	{
		m_menu = Menu_CrewUnboardLeave;
	}
}