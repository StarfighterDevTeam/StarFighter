#include "Player.h"

extern Game* CurrentGame;

using namespace sf;

Player::Player(sf::Vector2i sector_index) : Ship()
{
	m_disable_inputs = false;
	m_controllerType = AllControlDevices;

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
	m_max_braking = 3000;
	m_idle_decelleration = 1000;
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

	sf::Vector2f inputs_direction = sf::Vector2f(0, 0);
	if ((*CurrentGame).m_window_has_focus)
	{
		inputs_direction = InputGuy::getDirections();

		if (InputGuy::isSpeeding() == true)
			inputs_direction.y = -1;
		if (InputGuy::isBraking() == true)
			inputs_direction.y = 1;
		else if (sf::Joystick::isConnected(0) == true && InputGuy::isSpeeding() == false)
			inputs_direction.y = 0;
	}

	ApplyFlightModel(deltaTime, inputs_direction);

	//weapons
	for (Weapon* weapon : m_weapons)
	{
		weapon->Update(deltaTime);

		if (InputGuy::isFiring() == true)
			if (weapon->IsReadyToFire() == true)
				weapon->Fire();
	}

	//markers
	for (SpatialObject* marked_object : m_marked_objects)
	{
		if (marked_object->m_removeMe == true)
			marked_object->Update(deltaTime);//need to update this ship "manually" because it's not in the m_sceneGameObjects anymore

		marked_object->m_marker->Update(deltaTime);
	}

	Ship::Update(deltaTime);

	//cycling mission
	if (m_inputs_states[Action_CyclingMission] == Input_Tap)
		CycleMission();

	UpdateMissions();
}

void Player::UpdateMissions()
{
	for (Mission* mission : m_missions)
		if (mission->m_status == MissionStatus_Accepted || mission->m_status == MissionStatus_Current)
			switch (mission->m_mission_type)
				case Mission_GoTo:
				{
					if (m_isOrbiting == mission->m_marked_objectives.front())
						CompleteMission(mission);
						
					break;
				}
}

bool Player::CycleMission()
{
	Mission* current_mission = NULL;
	Mission* new_current_mission = NULL;

	//browse to find current mission and try to cycle towards the next acceptable mission in the list
	for (Mission* mission : m_missions)
	{
		if (mission->m_status == MissionStatus_Current)
			current_mission = mission;

		if (current_mission != NULL && mission->m_status == MissionStatus_Accepted)
		{ 
			CancelMission(current_mission);
			SetCurrentMission(mission);
			return true;
		}
	}

	//no current mission = cycle to the first acceptable mission
	if (current_mission == NULL)
	{
		for (Mission* mission : m_missions)
		{
			if (mission->m_status == MissionStatus_Accepted)
			{
				SetCurrentMission(mission);
				return true;
			}
		}

		//no mission can be accepted, don't bother searching anymore
		return false;
	}
	//not found yet, cycle another time from the beginning this time
	else if (new_current_mission == NULL)
	{
		for (Mission* mission : m_missions)
		{
			if (mission->m_status == MissionStatus_Accepted)
			{
				CancelMission(current_mission);
				SetCurrentMission(mission);
				return true;
			}
		}
	}

	//can't cycle = we stay on the same current mission
	return false;
}

void Player::Draw(RenderTarget& screen)
{
	for (int i = 0; i < NB_MARKER_TYPES; i++)
		for (SpatialObject* marked_object : m_marked_objects)
			if ((int)marked_object->m_marker->m_marker_type == i)
				marked_object->m_marker->Draw(screen);

	GameObject::Draw(screen);
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
	if (data)  // si l'ouverture a réussi
	{
		data << "Save ";// << ship->m_speed.x << endl;

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
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

	if (data) // si ouverture du fichier réussie
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
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Player::SetControllerType(ControlerType contoller)
{
	m_controllerType = contoller;
}

void Player::MarkThis(SpatialObject* target)
{
	m_marked_objects.push_back(target);
}

bool Player::AcceptMission(Mission* mission)
{
	int i = 0;
	int j = 0;
	for (Mission* player_mission : m_missions)
	{
		if (player_mission->m_status == MissionStatus_Accepted || player_mission->m_status == MissionStatus_Current)
			i++;

		if (player_mission->m_status == MissionStatus_Current)
			j++;
	}
		
	if (i >= NB_MISSIONS_ACCEPTED_MAX)//can't accept because the mission backlog is full
		return false;
		
	mission->m_status = MissionStatus_Accepted;
	m_missions.push_back(mission);

	if (j == 0)
		SetCurrentMission(mission);

	return true;
}


void Player::SetCurrentMission(Mission* mission)
{
	mission->m_status = MissionStatus_Current;
	
	//mission markers
	for (SpatialObject* objective : mission->m_marked_objectives)
		AddMissionMarker(objective);
}

void Player::AddMissionMarker(SpatialObject* target)
{
	target->m_marker->m_isMission = true;
	target->m_marker->m_distance_text.setColor(sf::Color::Blue);
	target->m_marker->SetAnimationLine((int)Marker_Mission);
}

void Player::RemoveMissionMarker(SpatialObject* target)
{
	target->m_marker->SetMarkerType(target->m_marker->m_marker_type);
}

void Player::CancelMission(Mission* mission)
{
	if (mission->m_status == MissionStatus_Current)
	{
		for (SpatialObject* objective : mission->m_marked_objectives)
			RemoveMissionMarker(objective);

		mission->m_status = MissionStatus_Accepted;
	}
}

void Player::CompleteMission(Mission* mission)
{
	if (mission->m_status == MissionStatus_Current)
	{
		for (SpatialObject* objective : mission->m_marked_objectives)
			RemoveMissionMarker(objective);

		CycleMission();
	}

	mission->m_status = MissionStatus_Complete;
}

void Player::FailMission(Mission* mission)
{
	if (mission->m_status == MissionStatus_Current)
	{
		for (SpatialObject* objective : mission->m_marked_objectives)
			RemoveMissionMarker(objective);

		CycleMission();
	}

	mission->m_status = MissionStatus_Failed;
}

void Player::RemoveMission(Mission* mission)
{
	if (mission->m_status == MissionStatus_Current)
		CancelMission(mission);

	//Delete mission
	vector<Mission*> old_missions;
	for (Mission* it_mission : m_missions)
		old_missions.push_back(it_mission);
	m_missions.clear();
	for (Mission* it_mission : old_missions)
		if (it_mission != mission)
			m_missions.push_back(it_mission);

	delete mission;
}

SpatialObject* Player::GetTargetableEnemyShip(const GameObject* ref_object, const float dist_max, const float angle_delta_max)
{
	float shortest_distance = -1;
	SpatialObject* target = NULL;

	for (SpatialObject* marked_object : m_marked_objects)
	{
		if (marked_object->m_garbageMe == true || marked_object->m_collider != EnemyShipObject)
			continue;

		const float a = ref_object->m_position.x - marked_object->m_position.x;
		const float b = ref_object->m_position.y - marked_object->m_position.y;

		float distance_to_ref = (a * a) + (b * b);
		if (distance_to_ref < shortest_distance || shortest_distance < 0)
		{
			if (distance_to_ref < dist_max * dist_max)
			{
				float angle_delta = GetAngleDegToTargetPosition(ref_object->m_position, ref_object->m_heading, marked_object->m_position);

				if (abs(angle_delta) < angle_delta_max)
				{
					shortest_distance = distance_to_ref;
					target = marked_object;
				}
			}
		}
	}
	
	return target;
}