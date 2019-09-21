#include "Player.h"

extern Game* CurrentGame;

using namespace sf;

Player::Player(sf::Vector2i sector_index) : Ship()
{
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;
	m_money = 0;

	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_actions_states[i] = false;
	}

	//Star Hunter
	(*CurrentGame).m_playerShip = this;

	Init(sf::Vector2f(0, 0), sf::Vector2f(0, 0), "2D/V_Alpha2.png", sf::Vector2f(68, 84), 3, 1);

	(*CurrentGame).SetStarSectorIndex(this, sector_index);

	setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X * 0.5, REF_WINDOW_RESOLUTION_Y * 0.5));

	m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserGreen, PlayerFire, PlayerFireLayer, sf::Vector2f(6, m_size.y * 0.5), 0));
	m_weapons.push_back(new Weapon(this, Weapon_Laser, Ammo_LaserGreen, PlayerFire, PlayerFireLayer, sf::Vector2f(-6, m_size.y * 0.5), 0));
	m_weapons.push_back(new Weapon(this, Weapon_Missile, Ammo_Missile, PlayerFire, PlayerFireLayer, sf::Vector2f(m_size.x * 0.5 + 8, 0), 0));
	m_weapons.push_back(new Weapon(this, Weapon_Missile, Ammo_Missile, PlayerFire, PlayerFireLayer, sf::Vector2f(-m_size.x * 0.5 - 8, 0), 0));

	//Flight model
	m_speed_max = 800;
	m_acceleration_max = 2000;
	m_turn_speed = 160;
	m_braking_max = 3000;
	m_idle_decelleration = 1000;

	//combat
	m_health_max = 50;
	m_shield_max = 20;
	m_shield_range = 100;
	m_shield_regen = 1.5;
	m_isReflectingShots = false;
	m_energy_max = 100;
	m_energy_regen = 10;

	InitShip();
}

Player::~Player()
{
	for (Mission* mission : m_missions)
		delete mission;
}

void Player::Update(sf::Time deltaTime)
{
	//controls
	UpdateInputStates();

	m_inputs_direction = sf::Vector2f(0, 0);
	if ((*CurrentGame).m_window_has_focus)
	{
		m_inputs_direction = InputGuy::getDirections();

		if (InputGuy::isSpeeding() == true)
			m_inputs_direction.y = -1;
		if (InputGuy::isBraking() == true)
			m_inputs_direction.y = 1;
		else if (sf::Joystick::isConnected(0) == true && InputGuy::isSpeeding() == false)
			m_inputs_direction.y = 0;
	}

	ApplyFlightModel(deltaTime, m_inputs_direction);

	//weapons
	for (Weapon* weapon : m_weapons)
	{
		weapon->Update(deltaTime);

		if (InputGuy::isFiring() == true)
			if (weapon->IsReadyToFire() == true)
				if (m_energy >= weapon->m_energy_cost)
				{
					weapon->Fire();
					m_energy -= weapon->m_energy_cost;
				}
	}

	//Gravity circle to be drawn
	if (m_gravitation_range > 0)
		(*CurrentGame).m_gravity_circles.push_back(m_gravitation_circle);

	Ship::Update(deltaTime);

	UpdateEnergyRegen(deltaTime);
	m_energy_rect.setSize(sf::Vector2f(m_energy_container_rect.getSize().x * m_energy / m_energy_max, m_energy_container_rect.getSize().y));

	//cycling mission
	if (m_inputs_states[Action_CyclingMission] == Input_Tap)
		CycleMission();

	//updating mission objectives and status
	UpdateMissions();
}

void Player::UpdateMissions()
{
	//Automatic cycling
	bool found = false;
	for (Mission* mission : m_missions)
		if (mission->m_status == MissionStatus_Current)
		{
			found = true;
			break;
		}

	if (found == false)
		CycleMission();

	//Update missions
	vector<Mission*> missions_to_delete;
	
	for (Mission* mission : m_missions)
		if (mission->m_status == MissionStatus_Current || mission->m_status == MissionStatus_Accepted)
		{
			vector<SpatialObject*> tmp_marked_objectives;
			for (SpatialObject* object : mission->m_marked_objectives)
				switch (object->m_collider)
				{
					case BeaconObject:
					{	
						if (mission->m_status == MissionStatus_Current && GetDistanceSquaredBetweenPositions(m_position, object->m_position) < 200 * 200)
						{
							UnmarkThis(object, true);

							//Beacon activation (spawning ships...)
							Beacon* beacon = (Beacon*)object;
							for (AIShip* ship : beacon->m_ships_to_create)
							{
								(*CurrentGame).addToScene(ship, AIShipLayer, ship->m_hostility == Hostility_Ally ? AllyShipObject : EnemyShipObject, true);
								ship->SetROE(ROE_FireAtWill);
								(*CurrentGame).m_playerShip->MarkThis(ship, true);

								tmp_marked_objectives.push_back(ship);
							}
							beacon->m_ships_to_create.clear();
							delete beacon;
						}
						else
							tmp_marked_objectives.push_back(object);

						mission->m_body_text = "Go to beacon";
						break;
					}
					case PlanetObject:
					{
						Planet* planet = (Planet*)object;
						if (mission->m_status == MissionStatus_Current && m_isOrbiting == object)
							UnmarkThis(object, true);
						else
							tmp_marked_objectives.push_back(object);

						//update text
						if (mission->m_mission_type == Mission_GoTo)
							mission->m_body_text = "Go to planet " + to_string(planet->m_planet_id);
						else
							mission->m_body_text = "Go back to planet " + to_string(planet->m_planet_id);
						break;
					}
					case EnemyShipObject:
					{
						AIShip* ship = (AIShip*)object;
						if (mission->m_mission_type == Mission_EliminateSquad || mission->m_mission_type == Mission_Bounty)
						{
							//when reaching the target ship, mark and display the real number of enemy ships
							if (mission->m_status == MissionStatus_Current && object->m_roe == ROE_FireAtWill && mission->m_marked_objectives.size() == 1)
								for (SpatialObject* ally : ship->m_scripted_allied_ships)
								{
									(*CurrentGame).m_playerShip->MarkThis(ally, true);
									tmp_marked_objectives.push_back(ally);
								}

							mission->m_body_text = "Destroy target enemies. " + (mission->m_status == MissionStatus_Current && ship->m_roe == ROE_FireAtWill ? to_string(mission->m_marked_objectives.size()) + " left." : "");
						}
						else if (mission->m_mission_type == Mission_EliminateBoss)
							mission->m_body_text = "Destroy target enemy.";

						if (ship->m_garbageMe == true)
						{
							UnmarkThis(object, true);

							//return to mission's owner planet
							if (object == mission->m_marked_objectives.back() && tmp_marked_objectives.empty() == true)
							{
								(*CurrentGame).m_playerShip->MarkThis(mission->m_owner, true);
								tmp_marked_objectives.push_back(mission->m_owner);
								mission->m_body_text = "Go back to planet " + to_string(mission->m_owner->m_planet_id);
							}
						}
						else
							tmp_marked_objectives.push_back(object);
						break;
					}
					case AllyShipObject:
					{
						AIShip* ship = (AIShip*)object;

						if (mission->m_status == MissionStatus_Current)
						{	
							//escort is figthing?
							bool escort_is_fighting = false;
							for (GameObject* escort_object : (*CurrentGame).m_sceneGameObjectsTyped[AllyShipObject])
							{
								SpatialObject* escort_ally = (SpatialObject*)escort_object;
								if (escort_ally->GetEscortedShip() != NULL && escort_ally->GetEscortedShip() == ship && escort_ally->m_roe == ROE_FireAtWill)
								{
									escort_is_fighting = true;
									break;
								}
							}

							//escort is fighting => stop the convoy
							if (escort_is_fighting == true)
								ship->SetROE(ROE_Freeze);
							//player near convoy, convoy freezed, escort is not fighting and no enemy nearby? => set convoy on the move
							else if (ship->m_roe == ROE_Freeze && GetDistanceSquaredBetweenPositions(m_position, object->m_position) < 300 * 300)
							{
								//no enemy on nearby?
								if (ship->GetTargetableEnemyShip(REF_WINDOW_RESOLUTION_X * 0.5, 360) == NULL)
									ship->SetROE(ship->m_native_ROE);
							}
								
							//convoy on the move?
							if (ship->m_roe != ROE_Freeze)
							{
								//arrived at destination?
								if (ship->m_scripted_destination != NULL && GetDistanceSquaredBetweenPositions(ship->m_position, *ship->m_scripted_destination) < (PLANET_ORBIT_RANGE + PLANET_RADIUS - ship->m_size.y * 0.5) * (PLANET_ORBIT_RANGE + PLANET_RADIUS - ship->m_size.y * 0.5))
								{
									UnmarkThis(ship, true);
									ship->m_garbageMe = true;

									//delete escorting ships
									for (GameObject* escort_ship : ship->m_scripted_allied_ships)
									{
										UnmarkThis((SpatialObject*)escort_ship, false);
										escort_ship->m_garbageMe = true;
									}
										
								}
								//too far from convoy = freeze convoy movement
								else if (GetDistanceSquaredBetweenPositions(m_position, object->m_position) > REF_WINDOW_RESOLUTION_X * 0.5 * REF_WINDOW_RESOLUTION_X * 0.5)
								{
									ship->SetROE(ROE_Freeze);
									tmp_marked_objectives.push_back(ship);
								}
								else
									tmp_marked_objectives.push_back(ship);
							}
							else
								tmp_marked_objectives.push_back(ship);
						}
						else
						{
							if (ship->m_roe != ROE_Freeze)
								ship->SetROE(ROE_Freeze);
							tmp_marked_objectives.push_back(object);
						}

						//update text
						if (ship->m_roe == ROE_Freeze)
							mission->m_body_text = "Rejoin the convoy";
						else
							mission->m_body_text = "Escort the convoy";
						break;
					}
				}

			//Update mission objectives remaining
			mission->m_marked_objectives.clear();
			for (SpatialObject* object : tmp_marked_objectives)
				mission->m_marked_objectives.push_back(object);

			//Mission complete?
			if (mission->m_marked_objectives.empty() == true)
			{
				EndMission(mission, MissionStatus_Complete);
				//mission->m_owner->m_nb_missions_to_create++;
				missions_to_delete.push_back(mission);
			}
		}

	for (Mission* mission : missions_to_delete)
		RemoveMission(mission);
}

bool Player::CycleMission()
{
	vector<Mission*> missions_before;
	vector<Mission*> missions_after;
	
	//browse to find current mission and try to cycle towards the next acceptable mission in the list
	Mission* current_mission = NULL;
	Mission* first_mission = NULL;

	for (Mission* mission : m_missions)
		if (mission->m_status == MissionStatus_Accepted)
		{
			if (current_mission == NULL && first_mission == NULL)
				first_mission = mission;
			else if (current_mission != NULL)
			{
				missions_after.push_back(mission);
				break;
			}
		}
		else if (mission->m_status == MissionStatus_Current)
			current_mission = mission;

	//set next mission: mission after the current one in the list > first mission in the list > current mission by default
	Mission* next_mission = missions_after.empty() == false ? missions_after.back() : first_mission != NULL ? first_mission : current_mission;

	if (next_mission == current_mission)
		return false;
	
	if (current_mission != NULL)
		EndMission(current_mission, MissionStatus_Accepted);

	SetCurrentMission(next_mission);
	return true;
}

void Player::Draw(RenderTarget& screen)
{
	for (SpatialObject* marked_object : m_marked_objects)
	{
		if (marked_object->m_marker_target != NULL)
			marked_object->m_marker_target->Draw(screen);

		if (marked_object->m_marker_mission != NULL)
			marked_object->m_marker_mission->Draw(screen);
	}
			
	Ship::Draw(screen);

	if (m_visible == true)
	{
		screen.draw(m_energy_container_rect);
		screen.draw(m_energy_rect);
	}

	DebugDrawMissions();

	DebugDrawMoney();
}

void Player::UpdateInputStates()
{
	if ((*CurrentGame).m_window_has_focus)
	{
		GetInputState(InputGuy::isSpeeding(), Action_Speeding);
		GetInputState(InputGuy::isBraking(), Action_Braking);
		GetInputState(InputGuy::isFiring(), Action_Firing);
		GetInputState(InputGuy::isCyclingMission(), Action_CyclingMission);
	}
	else
	{
		GetInputState(false, Action_Speeding);
		GetInputState(false, Action_Braking);
		GetInputState(false, Action_Firing);
		GetInputState(false, Action_CyclingMission);
	}
}

bool Player::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
{
	if (state_required == Input_Tap && condition && m_inputs_states[action] == Input_Tap)
	{
		m_actions_states[action] = !m_actions_states[action];
		return true;
	}
	else if (state_required == Input_Hold && condition)
	{
		m_actions_states[action] = m_inputs_states[action];
		return true;
	}
	else if (!condition)
	{
		m_actions_states[action] = false;
	}
	return false;
}

void Player::GetInputState(bool input_guy_boolean, PlayerActions action)
{
	if (input_guy_boolean)
	{
		m_inputs_states[action] = m_inputs_states[action] == Input_Release ? Input_Tap : Input_Hold;
	}
	else
	{
		m_inputs_states[action] = Input_Release;
	}
}

//SAVE AND LOAD LOCAL FILE
int Player::SaveShip(Player* ship)
{
	printf("Saving game in local file.\n");
	assert(ship != NULL);

	ofstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in | ios::trunc);
	if (data)  // si l'ouverture a r�ussi
	{
		data << "Save ";// << ship->m_speed.x << endl;

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No existing save file founded. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Player::LoadShip(Player* ship)
{
	printf("Loading ship from local file.\n");
	assert(ship != NULL);

	std::ifstream data(string(getSavesPath()) + PLAYER_SAVE_FILE, ios::in);

	if (data) // si ouverture du fichier r�ussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			std::istringstream ss(line);

			string saved_content;
			ss >> saved_content;
			//ship->content = saved_content;
		}

		data.close();  // on ferme le fichier
		return true;
	}
	else  // si l'ouverture a �chou�
	{
		cerr << "DEBUG: No save file found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Player::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Player::MarkThis(SpatialObject* target, bool isMission)
{
	//change flag and add marker if necessary
	if (isMission == true && target->m_marker_mission == NULL)
			target->m_marker_mission = new Marker(Marker_Mission, target);
	else if (isMission == false && target->m_marker_target == NULL)
		target->m_marker_target = new Marker(target->m_hostility == Hostility_Ally ? Marker_Ally : Marker_Enemy, target);
		
	//already marked?
	for (SpatialObject* object : m_marked_objects)
		if (object == target)
			return;

	m_marked_objects.push_back(target);

	//add to game objects updated if not present
	if (target->m_removeMe == false)
		return;

	(*CurrentGame).addToScene(target, target->m_layer, target->m_collider, true);

	//remove from storage
	int id = (*CurrentGame).GetSectorId(target->m_sector_index);
	vector<GameObject*> tmp_sceneGameObjectsStored;
	for (GameObject* object : (*CurrentGame).m_sceneGameObjectsStored[id])
		if (object != target)
			tmp_sceneGameObjectsStored.push_back(object);

	(*CurrentGame).m_sceneGameObjectsStored[id].clear();
	if (tmp_sceneGameObjectsStored.empty() == true)
		(*CurrentGame).m_sceneGameObjectsStored.erase(id);
	else
		for (GameObject* object : tmp_sceneGameObjectsStored)
			(*CurrentGame).m_sceneGameObjectsStored[id].push_back(object);
}


void Player::UnmarkThis(SpatialObject* target, bool isMission)
{
	if (isMission == true)
	{
		delete target->m_marker_mission;
		target->m_marker_mission = NULL;
	}
	else
	{
		delete target->m_marker_target;
		target->m_marker_target = NULL;
	}

	//remove marked object if no marker is left on it
	vector<SpatialObject*> tmp_marked_objects;
	for (SpatialObject* object : m_marked_objects)
		if (object == target)
		{
			if (target->m_marker_mission != NULL || target->m_marker_target != NULL)
				return;
			else
				(*CurrentGame).StoreObjectIfNecessary(target);
		}
		else
			tmp_marked_objects.push_back(object);

	m_marked_objects.clear();
	for (SpatialObject* object : tmp_marked_objects)
		m_marked_objects.push_back(object);
}

void Player::UpdateMarkers(sf::Time deltaTime)
{
	for (SpatialObject* marked_object : m_marked_objects)
	{
		if (marked_object->m_marker_target != NULL)
			marked_object->m_marker_target->Update(deltaTime);

		if (marked_object->m_marker_mission != NULL)
			marked_object->m_marker_mission->Update(deltaTime);
	}
}

int Player::GetMarkedObjectsCount()
{
	return m_marked_objects.size();
}

bool Player::AcceptMission(Mission* mission)
{
	if (m_missions.size() >= NB_MISSIONS_MAX)
		return false;

	mission->m_status = MissionStatus_Accepted;
	m_missions.push_back(mission);

	for (Mission* player_mission : m_missions)
		if (player_mission->m_status == MissionStatus_Current)
			return true;

	//if we don't have a current mission, set this new accepted mission as current mission
	SetCurrentMission(mission);
	return true;
}


void Player::SetCurrentMission(Mission* mission)
{
	mission->m_status = MissionStatus_Current;
	
	//mission markers
	for (SpatialObject* objective : mission->m_marked_objectives)
		(*CurrentGame).m_playerShip->MarkThis(objective, true);
}

void Player::EndMission(Mission* mission, MissionStatus status)
{
	if (mission->m_status == MissionStatus_Current)
		for (SpatialObject* objective : mission->m_marked_objectives)
			(*CurrentGame).m_playerShip->UnmarkThis(objective, true);

	mission->m_status = status;

	if (status == MissionStatus_Complete)
		m_money += mission->m_money_prize;
}

void Player::RemoveMission(Mission* mission)
{
	//Delete mission
	vector<Mission*> old_missions;
	for (Mission* it_mission : m_missions)
		old_missions.push_back(it_mission);
	m_missions.clear();
	for (Mission* it_mission : old_missions)
		if (it_mission != mission)
			m_missions.push_back(it_mission);

	if (mission->m_owner != NULL)
	{
		vector<Mission*> old_planet_missions;
		for (Mission* it_mission : mission->m_owner->m_missions)
			old_planet_missions.push_back(it_mission);
		mission->m_owner->m_missions.clear();
		for (Mission* it_mission : old_planet_missions)
			if (it_mission != mission)
				mission->m_owner->m_missions.push_back(it_mission);
	}

	delete mission;
}

void Player::DebugDrawMissions()
{
	float offset_y = 50;
	for (Mission* mission : m_missions)
	{
		sf::Text body_text;
		body_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
		body_text.setCharacterSize(20);
		body_text.setStyle(sf::Text::Bold);
		
		if (mission->m_status == MissionStatus_Current)
			body_text.setColor(sf::Color::White);
		else if (mission->m_status == MissionStatus_Accepted)
			body_text.setColor(sf::Color::Blue);
		else if (mission->m_status == MissionStatus_Complete)
			body_text.setColor(sf::Color::Green);
		else if (mission->m_status == MissionStatus_Failed)
			body_text.setColor(sf::Color::Red);

		body_text.setString(mission->m_body_text);
		body_text.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - 350, offset_y));

		offset_y += 40;

		(*CurrentGame).m_mainScreen.draw(body_text);
	}
}

void Player::DebugDrawMoney()
{
	sf::Text money_text;
	money_text.setFont(*(*CurrentGame).m_font[Font_Arial]);
	money_text.setCharacterSize(20);
	money_text.setStyle(sf::Text::Bold);
	money_text.setColor(sf::Color::White);

	money_text.setString(to_string(m_money) + " �");
	money_text.setPosition(sf::Vector2f(REF_WINDOW_RESOLUTION_X - 120, REF_WINDOW_RESOLUTION_Y - 50));

	(*CurrentGame).m_mainScreen.draw(money_text);
}

void Player::Death()
{
	//Debug respawn
	m_health = m_health_max;
	m_shield = m_shield_max;
	m_energy = m_energy_max;

	Ship::Death();
}

void Player::GetHitByGravitation(GameObject* ship)
{
	AIShip* attractor = (AIShip*)ship;
	if (attractor->m_roe == ROE_Ambush)
		attractor->SetROE(ROE_FireAtWill);

	Ship::GetHitByGravitation(ship);
}