#include "Ship.h"

extern Game* CurrentGame;

using namespace sf;

Ship::Ship(ShipModel* ship_model) : GameObject(Vector2f(0, 0), Vector2f(0, 0), ship_model->m_textureName, ship_model->m_size, Vector2f((ship_model->m_size.x / 2), (ship_model->m_size.y / 2)), ship_model->m_frameNumber)
{
	if (!ship_model)
	{
		return;
	}

	m_ship_model = ship_model;

	m_automatic_fire = false;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		m_equipment[i] = NULL;
	}
	m_weapon = NULL;
	m_collider_type = PlayerShip;
	m_moving = false;
	m_movingX = m_movingY = false;
	m_visible = true;
	m_damage = 0;
	m_hyperspeed = 1.0f;
	m_armor = 1;
	m_shield = 0;
	m_shield_regen = 0;
	m_disable_inputs = false;
	m_disable_fire = false;
	m_disabledHyperspeed = false;
	m_graze_count = 0;
	m_graze_level = 0;
	m_last_hazard_level_played = 0;
	m_disable_bots = true;
	m_is_asking_scene_transition = false;
	m_currentScene_hazard = 0;

	m_level = 1;
	m_level_max = FIRST_LEVEL_MAX;
	m_xp = 0;
	m_xp_max = XP_MAX_FIRST_LEVEL;

	m_fake_ship = NULL;
	if (!ship_model->m_fake_textureName.empty())
	{
		m_fake_ship = new FakeShip(this, ship_model->m_fake_textureName, ship_model->m_fake_size, ship_model->m_fake_frameNumber, NB_ShipAnimations);
		(*CurrentGame).addToScene(m_fake_ship, FakeShipLayer, FakePlayerShip);
	}

	m_combo_aura = new Aura(this, "Assets/2D/FX/Aura_Graze.png", sf::Vector2f(50, 50), 3, NB_GRAZE_LEVELS);
	(*CurrentGame).addToScene(m_combo_aura, AuraLayer, Neutral);

	m_trail = new Aura(this, "Assets/2D/FX/Aura_HyperspeedTrail.png", sf::Vector2f(70, 34), 3, 1);
	sf::Vector2f real_size = m_fake_ship ? m_fake_ship->m_size : m_size;
	m_trail->m_offset = sf::Vector2f(0, (real_size.y / 2) + (m_trail->m_size.y / 2));
	(*CurrentGame).addToScene(m_trail, FakeShipLayer, Neutral);

	m_targetPortal = NULL;
	m_targetShop = NULL;
	m_equipment_loot = NULL;
	m_weapon_loot = NULL;
	m_isFocusedOnHud = false;
	m_previously_focused_item = NULL;

	m_SFPanel = NULL;
	m_is_asking_SFPanel = SFPanel_None;
	m_HUD_SFPanel = NULL;

	m_previouslyCollidingWithInteractiveObject = No_Interaction;
	m_isCollidingWithInteractiveObject = No_Interaction;
	m_HUD_state = HUD_Idle;

	for (size_t i = 0; i < NBVAL_PlayerActions; i++)
	{
		m_inputs_states[i] = Input_Release;
		m_actions_states[i] = false;
	}

	Init();
}

Ship::~Ship()
{
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			delete m_equipment[i];
		}
	}
	if (m_ship_model)
	{
		delete m_ship_model;
	}
	if (m_weapon)
	{
		delete m_weapon;
	}

	//game objects
	if (m_combo_aura)
	{
		m_combo_aura->m_GarbageMe = true;
		m_combo_aura->m_visible = false;
	}
	if (m_fake_ship)
	{
		m_fake_ship->m_GarbageMe = true;
		m_fake_ship->m_visible = false;
	}
	if (m_trail)
	{
		m_trail->m_GarbageMe = true;
		m_trail->m_visible = false;
	}
}

void Ship::Init()
{
	UpdateShipLevel();

	m_max_speed = getFighterFloatStatValue(Fighter_MaxSpeed);
	m_deceleration = getFighterFloatStatValue(Fighter_Deceleration);
	m_acceleration = getFighterFloatStatValue(Fighter_Acceleration);
	m_hyperspeed = getFighterFloatStatValue(Fighter_Hyperspeed);
	m_hyperspeed_fuel_max = getFighterIntStatValue(Fighter_HyperspeedFuel);
	if (m_hyperspeed_fuel > m_hyperspeed_fuel_max)
	{
		m_hyperspeed_fuel = m_hyperspeed_fuel_max;
	}
	m_armor_max = getFighterIntStatValue(Fighter_ArmorMax);
	if (m_armor > m_armor_max)
	{
		m_armor = m_armor_max;
	}
	m_shield_max = getFighterIntStatValue(Fighter_ShieldMax);
	if (m_shield > m_shield_max)
	{
		m_shield = m_shield_max;
	}
	m_shield_regen = getFighterIntStatValue(Fighter_ShieldRegen);
	m_shield_recovery_time = getFighterFloatStatValue(Fighter_ShieldRecovery);
	m_damage = getFighterIntStatValue(Fighter_ContactDamage);
	m_size = !m_ship_model->m_fake_textureName.empty() ? m_ship_model->m_fake_size : m_ship_model->m_size;
	m_textureName = m_ship_model->m_textureName;
	m_frameNumber = m_ship_model->m_frameNumber;

	//fake texture
	m_transparent = !m_ship_model->m_fake_textureName.empty();

	//Loading bots
	m_bot_list.clear();
	if (m_ship_model->m_bot)
	{
		m_bot_list.push_back(m_ship_model->m_bot);
	}
	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i] != NULL)
		{
			if (m_equipment[i]->m_bot)
			{
				m_bot_list.push_back(m_equipment[i]->m_bot);
			}
		}
	}
}

void Ship::CleanGarbagedEquipments()
{
	size_t shipEquipmentsSize = m_garbageEquipments.size();
	for (size_t i = 0; i < shipEquipmentsSize; i++)
	{
		if (m_garbageEquipments[i] == NULL)
			continue;

		delete m_garbageEquipments[i];
	}
	m_garbageEquipments.clear();

	size_t shipWeaponsSize = m_garbageWeapons.size();
	for (size_t i = 0; i < shipWeaponsSize; i++)
	{
		if (m_garbageWeapons[i] == NULL)
			continue;

		delete m_garbageWeapons[i];
	}
	m_garbageWeapons.clear();
}

bool Ship::setShipEquipment(Equipment* equipment, bool overwrite_existing, bool no_save)
{
	assert(equipment != NULL);

	if (!overwrite_existing && m_equipment[equipment->m_equipmentType])
	{
		return false;
	}

	if (overwrite_existing && m_equipment[equipment->m_equipmentType] && m_equipment[equipment->m_equipmentType]->m_bot)
	{
		DestroyBots();
	}

	if (overwrite_existing && m_equipment[equipment->m_equipmentType])
	{
		m_garbageEquipments.push_back(m_equipment[equipment->m_equipmentType]);
	}

	m_equipment[equipment->m_equipmentType] = equipment;
	
	Init();

	if (equipment->m_bot)
	{
		GenerateBots(this);
	}
	
	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE, this);

	return true;
}

bool Ship::setShipWeapon(Weapon* weapon, bool overwrite_existing, bool no_save)
{
	assert(weapon != NULL);

	if (!overwrite_existing && m_weapon)
	{
		return false;
	}

	if (overwrite_existing && m_weapon)
	{
		m_garbageWeapons.push_back(m_weapon);
	}
		
	m_weapon = weapon;
		
	Init();

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE, this);
	
	return true;
}

void Ship::cleanEquipment(int equipment_type, bool no_save)
{
	if (m_equipment[equipment_type])
	{
		if (m_equipment[equipment_type]->m_bot)
		{
			DestroyBots();
		}
		delete m_equipment[equipment_type];
		m_equipment[equipment_type] = NULL;

		this->Init();
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE, this);
}

void Ship::cleanWeapon(bool no_save)
{
	if (m_weapon)
	{
		delete m_weapon;
		m_weapon = NULL;

		Init();
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE, this);
}

void Ship::setShipModel(ShipModel* ship_model, bool no_save)
{
	assert(ship_model != NULL);

	if (ship_model->m_bot)
	{
		DestroyBots();
	}

	m_ship_model = ship_model;
	Init();

	if (ship_model->m_bot)
	{
		GenerateBots(this);
	}

	if (!no_save)
		Ship::SaveItems(ITEMS_SAVE_FILE, this);
}

void Ship::GetInputState(bool input_guy_boolean, PlayerActions action)
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

//Inputs
void Ship::UpdateInputStates()
{
	GetInputState(InputGuy::isFiring(), Action_Firing);
	GetInputState(InputGuy::isBraking(), Action_Braking);
	GetInputState(InputGuy::isHyperspeeding(), Action_Hyperspeeding);
	GetInputState(InputGuy::isSlowMotion(), Action_Slowmotion);
	GetInputState(InputGuy::isOpeningHud(), Action_OpeningHud);
	GetInputState(InputGuy::isChangingResolution(), Action_ChangingResolution);
	GetInputState(InputGuy::setAutomaticFire(), Action_AutomaticFire);
	GetInputState(InputGuy::isUsingDebugCommand(), Action_DebugCommand);
}

bool Ship::UpdateAction(PlayerActions action, PlayerInputStates state_required, bool condition)
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

void Ship::update(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//clean the dust
	CleanGarbagedEquipments();

	//ManageVisibility();
	//Resetting flags
	if (m_isCollidingWithInteractiveObject != PortalInteraction && !m_is_asking_scene_transition)
	{
		m_targetPortal = NULL;
	}
	if (m_isCollidingWithInteractiveObject != ShopInteraction)
	{
		m_targetShop = NULL;
	}
	m_movingX = false;
	m_movingY = false;
	m_moving = false;
	m_is_asking_SFPanel = SFPanel_None;
	m_is_asking_scene_transition = false;

	//Update
	ManageImmunity();
	ManageShieldRegen(deltaTime, hyperspeedMultiplier);

	sf::Vector2f directions = InputGuy::getDirections();

	ManageInputs(deltaTime, hyperspeedMultiplier, directions);

	m_trail->m_visible = (hyperspeedMultiplier > 1.0f);

	GameObject::update(deltaTime, hyperspeedMultiplier);

	ScreenBorderContraints();
	SettingTurnAnimations();
	ManageFeedbackExpiration(deltaTime);

	//Update HUD
	if (m_SFPanel)
	{
		m_SFPanel->Update(deltaTime, directions);
	}
	if (m_HUD_SFPanel)
	{
		m_HUD_SFPanel->Update(deltaTime, directions);
	}

	//member objects follow
	if (m_combo_aura)
	{
		m_combo_aura->update(deltaTime, hyperspeedMultiplier);
	}
	if (m_trail)
	{
		m_trail->update(deltaTime, hyperspeedMultiplier);
	}
	if (m_fake_ship)
	{
		m_fake_ship->update(deltaTime, hyperspeedMultiplier);
	}
}

bool Ship::ManageVisibility()
{
	//no update on invisible ship (=dead)
	if (m_fake_ship)
	{
		return m_fake_ship->m_visible;
	}
	else
	{
		return m_visible;
	}
}

void Ship::ManageShieldRegen(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//sheld regen if not maximum
	if (m_shield > 0 || m_shield_recovery_clock.getElapsedTime().asSeconds() > m_shield_recovery_time)
	{
		static double shield_regen_buffer = 0;
		if (m_shield < m_shield_max)
		{
			if (hyperspeedMultiplier < 1.0f)
			{
				shield_regen_buffer += m_shield_regen * deltaTime.asSeconds() * hyperspeedMultiplier;
			}
			else
			{
				shield_regen_buffer += m_shield_regen * deltaTime.asSeconds();
			}

			if (shield_regen_buffer > 1)
			{
				double intpart;
				shield_regen_buffer = modf(shield_regen_buffer, &intpart);
				m_shield += intpart;
			}

			//canceling over-regen
			if (m_shield > m_shield_max)
			{
				m_shield = m_shield_max;
			}
		}
	}
}

void Ship::ManageFiring(sf::Time deltaTime, float hyperspeedMultiplier)
{
	//Fire function
	if (m_weapon)
	{
		if (m_weapon->isFiringReady(deltaTime, hyperspeedMultiplier))
		{
			if (!m_disable_fire && (*CurrentGame).m_end_dialog_clock.getElapsedTime().asSeconds() > END_OF_DIALOGS_DELAY && !m_actions_states[Action_Hyperspeeding])
			{
				if (m_actions_states[Action_Firing] || m_actions_states[Action_AutomaticFire])
				{
					//calculating the angle we want to face, if any
					float target_angle = getRotation();
					if (m_weapon->m_target_seaking != NO_SEAKING || (m_weapon->m_target_seaking == SEMI_SEAKING && m_weapon->m_rafale_index == 0))
					{
						target_angle = fmod(GameObject::getRotation_for_Direction((*CurrentGame).m_direction) - (*CurrentGame).GetAngleToNearestGameObject(EnemyObject, getPosition()), 360);
					}

					float current_angle = getRotation();
					float delta = current_angle - target_angle;
					if (delta > 180)
						delta -= 360;
					else if (delta < -180)
						delta += 360;

					//float theta = (this->getRotation() - delta) / 180 * M_PI;
					float theta = getRotation() / 180 * M_PI;
					if (m_weapon->m_target_seaking != NO_SEAKING)
					{
						theta -= delta / 180 * M_PI;
					}

					if (m_weapon->m_target_seaking == SEMI_SEAKING && m_weapon->m_rafale_index > 0 && m_weapon->m_rafale_index < m_weapon->m_rafale)
					{
						//semi-seaking and rafale not ended = no update of target or weapon position
					}
					else
					{
						m_weapon->m_weapon_current_offset.x = m_weapon->m_weaponOffset.x + m_size.x / 2 * sin(theta);
						m_weapon->m_weapon_current_offset.y = m_weapon->m_weaponOffset.y - m_size.y / 2 * cos(theta);

						//transmitting the angle to the weapon, which will pass it to the bullets
						m_weapon->m_shot_angle = theta;
					}

					m_weapon->setPosition(getPosition().x + m_weapon->m_weapon_current_offset.x, getPosition().y + m_weapon->m_weapon_current_offset.y);
					m_weapon->Fire(FriendlyFire, deltaTime);

					(*CurrentGame).PlaySFX(SFX_Fire);
				}
			}
		}
	}
}

void Ship::UpdateHUDStates()
{
	//test
	//if (m_targetDialogs.empty())
	//{
	//	m_targetDialogs.push_back(Enemy::LoadDialog("Dial_V3_01"));
	//	m_targetDialogs.push_back(Enemy::LoadDialog("Dial_V3_02"));
	//	(*CurrentGame).m_waiting_for_dialog_validation = true;
	//}
	
	if (!m_targetDialogs.empty())
	{
		m_is_asking_SFPanel = SFPanel_Dialog;
		m_HUD_state = HUD_Dialog;
	}
	else if (m_HUD_state != HUD_OpeningEquipment)
	{
		if (m_targetPortal && m_targetPortal->m_state == PortalOpen && !m_actions_states[Action_Firing])
		{
			m_HUD_state = HUD_PortalInteraction;
		}
		else if (m_targetShop)
		{
			if (m_HUD_state != HUD_ShopBuyMenu && m_HUD_state != HUD_ShopSellMenu && m_HUD_state != HUD_ShopStellarMap)
			{
				m_HUD_state = HUD_ShopMainMenu;
			}
		}
		else
		{
			m_HUD_state = HUD_Idle;
		}
		//else if (m_HUD_state == HUD_PortalInteraction || m_HUD_state == HUD_ShopMainMenu)
		//{
		//	m_HUD_state = HUD_Idle;
		//}
	}
}

void Ship::ManageInputs(sf::Time deltaTime, float hyperspeedMultiplier, sf::Vector2f inputs_direction)
{
	if (!m_disable_inputs)
	{
		//Registering inputs
		UpdateInputStates();

		//Debug command
		if (m_inputs_states[Action_DebugCommand] == Input_Tap || m_inputs_states[Action_DebugCommand] == Input_Hold)
		{
			(*CurrentGame).killGameObjectLayer(EnemyObject);
		}

		//Update HUD state
		UpdateHUDStates();

		//DIALOG
		if (m_HUD_state == HUD_Dialog && m_HUD_SFPanel)
		{
			//Continue
			if (m_inputs_states[Action_Firing] == Input_Tap)
			{
				ContinueDialog();
			}
		}
		//EQUIPMENT HUD
		else if (m_HUD_state == HUD_OpeningEquipment && m_HUD_SFPanel)
		{
			//Cursor movement
			m_HUD_SFPanel->GetCursor()->m_visible = true;
			MoveCursor(m_HUD_SFPanel->GetCursor(), inputs_direction, deltaTime, m_HUD_SFPanel);

			//Swapping items
			if (m_inputs_states[Action_Firing] == Input_Tap && m_HUD_SFPanel->GetFocusedItem() && m_HUD_SFPanel->GetFocusedGrid() == 2)
			{
				EquipItem();
			}
			else if (m_inputs_states[Action_Firing] == Input_Tap && m_HUD_SFPanel->GetFocusedItem() && m_HUD_SFPanel->GetFocusedGrid() == 1)
			{
				DesequipItem();
			}
			//Garbaging item
			//else if ((m_inputs_states[Action_Braking] == Input_Tap || m_inputs_states[Action_Braking] == Input_Hold) && m_HUD_SFPanel->GetFocusedItem())
			//{
			//	GarbagingItem();
			//}
			else
			{
				m_HUD_SFPanel->SetPrioritaryFeedback(false);
			}
			m_previously_focused_item = m_HUD_SFPanel->GetFocusedItem();

			//Weapon firing
			ManageFiring(deltaTime, hyperspeedMultiplier);
			//Bots firing
			for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
			{
				(*it)->Fire(deltaTime, (*CurrentGame).m_hyperspeedMultiplier, m_actions_states[Action_Firing], m_actions_states[Action_Hyperspeeding]);
			}
			if (m_actions_states[Action_Firing] || m_actions_states[Action_AutomaticFire])
			{
				m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
				m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
			}

			//Closing hud
			if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			{
				m_HUD_state = HUD_Idle;
				if (m_HUD_SFPanel)
				{
					m_HUD_SFPanel->GetCursor()->m_visible = false;
				}
			}
		}
		//SHOP BUY
		else if (m_HUD_state == HUD_ShopBuyMenu && m_SFPanel)
		{
			//Cursor movement
			MoveCursor(m_SFPanel->GetCursor(), inputs_direction, deltaTime, m_SFPanel);
			//Force HUD cursor on equivalent object
			m_HUD_SFPanel->GetCursor()->m_visible = m_SFPanel->GetFocusedItem();
			if (m_HUD_SFPanel->GetCursor()->m_visible)
			{
				m_HUD_SFPanel->ForceCursorOnEquivalentObjectInGrid(m_SFPanel->GetFocusedItem(), m_HUD_SFPanel->GetGrid(true));
			}

			//Switch to sell menu
			if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			{
				m_HUD_state = HUD_ShopSellMenu;
				m_HUD_SFPanel->GetCursor()->m_visible = true;
				m_SFPanel->GetCursor()->m_visible = false;
				m_HUD_SFPanel->ClearHighlight();
				m_SFPanel->ClearHighlight();

				if (m_HUD_SFPanel->GetItemStatsPanel())
				{
					m_HUD_SFPanel->SetFocusedItem(NULL);
				}
			}

			//interaction: buy item
			if (m_inputs_states[Action_Firing] == Input_Tap && m_SFPanel->GetFocusedItem())
			{
				BuyingItem();    
			}

			//exit
			if (m_inputs_states[Action_Slowmotion] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_HUD_SFPanel->GetCursor()->m_visible = false;
			}
		}
		//SHOP SELL
		else if (m_HUD_state == HUD_ShopSellMenu)
		{
			//Cursor movement
			MoveCursor(m_HUD_SFPanel->GetCursor(), inputs_direction, deltaTime, m_HUD_SFPanel);

			//Swapping items
			//if (m_inputs_states[Action_Firing] == Input_Tap && m_HUD_SFPanel->GetFocusedItem() && m_HUD_SFPanel->GetFocusedGrid() == 2)
			//{
			//	SwappingItems();
			//}
			//Selling item
			//else if (m_inputs_states[Action_Braking] == Input_Tap && m_HUD_SFPanel->GetFocusedItem())
			if (m_inputs_states[Action_Firing] == Input_Tap && m_HUD_SFPanel->GetFocusedItem())
			{
				SellingItem();
			}
			
			//Switch to buy menu
			if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			{
				m_HUD_state = HUD_ShopBuyMenu;
				m_SFPanel->GetCursor()->m_visible = true;
				m_HUD_SFPanel->ClearHighlight();
				m_SFPanel->ClearHighlight();

				if (m_HUD_SFPanel->GetItemStatsPanel())
				{
					m_HUD_SFPanel->SetFocusedItem(NULL);
				}
			}

			//exit
			if (m_inputs_states[Action_Slowmotion] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_HUD_SFPanel->GetCursor()->m_visible = false;
			}
		}
		//STELLAR MAP
		else if (m_HUD_state == HUD_ShopStellarMap)
		{
			//Cursor movement
			MoveCursor(m_SFPanel->GetCursor(), inputs_direction, deltaTime, m_SFPanel);

			//Teleportation
			if (m_SFPanel->GetTeleportationCost() > 0 && m_money >= m_SFPanel->GetTeleportationCost())
			{
				if (m_inputs_states[Action_Firing] == Input_Tap)
				{
					Teleport(m_SFPanel->GetTeleportationDestination());
					m_money -= m_SFPanel->GetTeleportationCost();
					SavePlayerMoney(MONEY_SAVE_FILE, this);
					m_HUD_state = HUD_Idle;
				}
			}

			//Center view
			if (m_inputs_states[Action_Braking] == Input_Tap)
			{
				CenterMapView();
			}

			//exit
			if (m_inputs_states[Action_Slowmotion] == Input_Tap)
			{
				m_HUD_state = HUD_ShopMainMenu;
				m_SFPanel->GetCursor()->m_visible = true;
			}
		}
		else
		{
			//Opening hud
			if (UpdateAction(Action_OpeningHud, Input_Tap, true))
			{
				m_HUD_state = HUD_OpeningEquipment;
				m_HUD_SFPanel->GetCursor()->m_visible = true;
				
				if (!m_disabledHyperspeed)
				{
					(*CurrentGame).m_hyperspeedMultiplier = 1.0f / m_hyperspeed;
				}
			}

			//Moving
			m_moving = inputs_direction.x != 0 || inputs_direction.y != 0;
			m_movingX = inputs_direction.x != 0;
			m_movingY = inputs_direction.y != 0;
			ManageAcceleration(inputs_direction);

			//IDLE (COMBAT)
			if (m_HUD_state == HUD_Idle)
			{
				//Slow_motion and hyperspeed
				UpdateAction(Action_Slowmotion, Input_Tap, !m_disabledHyperspeed);
				UpdateAction(Action_Hyperspeeding, Input_Hold, !m_disabledHyperspeed);

				if (m_actions_states[Action_Hyperspeeding] && m_hyperspeed_fuel > 0)
				{
					(*CurrentGame).m_hyperspeedMultiplier = m_hyperspeed;
					m_hyperspeed_fuel -= m_hyperspeed * HYPERSPEED_CONSUMPTION_FOR_CRUISING * deltaTime.asSeconds();
					if (m_hyperspeed_fuel < 0)
					{
						m_hyperspeed_fuel = 0;
					}
				}
				else if (m_actions_states[Action_Slowmotion] && !m_disabledHyperspeed && m_hyperspeed_fuel > 0)
				{
					(*CurrentGame).m_hyperspeedMultiplier = 1.0f / m_hyperspeed;
					m_hyperspeed_fuel -= m_hyperspeed * HYPERSPEED_CONSUMPTION_FOR_SLOWMOTION * deltaTime.asSeconds();
					if (m_hyperspeed_fuel < 0)
					{
						m_hyperspeed_fuel = 0;
					}
				}
				else
				{
					(*CurrentGame).m_hyperspeedMultiplier = 1.0f;
				}

				//Auto fire option (F key)
				if (UpdateAction(Action_AutomaticFire, Input_Tap, !m_disable_fire))
				{
					//Bots automatic fire option
					m_automatic_fire = m_actions_states[Action_AutomaticFire];
					for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
					{
						(*it)->m_automatic_fire = m_actions_states[Action_AutomaticFire];
					}
				}

				//Firing button
				UpdateAction(Action_Firing, Input_Hold, !m_disable_fire);

				//Weapon firing
				ManageFiring(deltaTime, hyperspeedMultiplier);
				//Bots firing
				for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
				{
					(*it)->Fire(deltaTime, (*CurrentGame).m_hyperspeedMultiplier, m_actions_states[Action_Firing], m_actions_states[Action_Hyperspeeding]);
				}

				//Braking and speed malus on firing
				UpdateAction(Action_Braking, Input_Hold, true);
				if (m_actions_states[Action_Braking] || m_actions_states[Action_Firing] || m_actions_states[Action_AutomaticFire])
				{
					m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
					m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
				}
			}
			//PORTAL
			else if (m_HUD_state == HUD_PortalInteraction)
			{
				//Up and down in options, IF LEVEL CHOOSER IS AVAILABLE
				if (m_SFPanel)
				{
					if (m_inputs_states[Action_Braking] == Input_Tap && m_SFPanel->GetSelectedOptionIndex() < m_targetPortal->m_max_unlocked_hazard_level)
					{
						m_SFPanel->SetSelectedOptionIndex(m_SFPanel->GetSelectedOptionIndex() + 1);
					}
					else if (m_inputs_states[Action_Hyperspeeding] == Input_Tap && m_SFPanel->GetSelectedOptionIndex() > 0)
					{
						m_SFPanel->SetSelectedOptionIndex(m_SFPanel->GetSelectedOptionIndex() - 1);
					}
				}

				//Weapon firing
				ManageFiring(deltaTime, hyperspeedMultiplier);
				//Bots firing
				for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
				{
					(*it)->Fire(deltaTime, (*CurrentGame).m_hyperspeedMultiplier, m_actions_states[Action_Firing], m_actions_states[Action_Hyperspeeding]);
				}
				if (m_actions_states[Action_Firing] || m_actions_states[Action_AutomaticFire])
				{
					m_speed.x *= SHIP_BRAKING_MALUS_SPEED;
					m_speed.y *= SHIP_BRAKING_MALUS_SPEED;
				}

				//Entering portal
				if (m_inputs_states[Action_Firing] == Input_Tap)
				{
					m_is_asking_scene_transition = true;//this triggers transition in InGameState update
				}
			}
			//SHOP MAIN
			else if (m_HUD_state == HUD_ShopMainMenu && m_SFPanel)
			{
				//Up and down in options
				if (m_inputs_states[Action_Braking] == Input_Tap && m_SFPanel->GetSelectedOptionIndex() < NBVAL_ShopOptions - 1)
				{
					m_SFPanel->SetSelectedOptionIndex(m_SFPanel->GetSelectedOptionIndex() + 1);
				}
				else if (m_inputs_states[Action_Hyperspeeding] == Input_Tap && m_SFPanel->GetSelectedOptionIndex() > 0)
				{
					m_SFPanel->SetSelectedOptionIndex(m_SFPanel->GetSelectedOptionIndex() - 1);
				}

				//Select
				if (m_inputs_states[Action_Firing] == Input_Tap)
				{
					switch (m_SFPanel->GetSelectedOptionIndex())
					{
						case ShopHeal:
						{
							ResplenishHealth();

							//(*CurrentGame).PlaySFX(SFX_Heal);

							break;
						}
						case ShopBuy:
						{
							m_HUD_state = HUD_ShopBuyMenu;
							m_HUD_SFPanel->GetCursor()->m_visible = true;
							break;
						}
						case ShopStellarMap:
						{
							m_HUD_state = HUD_ShopStellarMap;
							break;
						}
					}
				}
			}
		}
		
		IdleDecelleration(deltaTime);
	}
	else
	{
		m_isFocusedOnHud = false;
	}
}

void Ship::ManageAcceleration(sf::Vector2f inputs_direction)
{
	m_speed.x += inputs_direction.x*getFighterFloatStatValue(Fighter_Acceleration);
	m_speed.y += inputs_direction.y*getFighterFloatStatValue(Fighter_Acceleration);

	//max speed constraints
	GameObject::NormalizeSpeed(&m_speed, getFighterFloatStatValue(Fighter_MaxSpeed));
}

void Ship::ManageImmunity()
{
	//immunity frames after death
	if (m_immune)
	{
		if (m_immunityTimer.getElapsedTime() > sf::seconds(2))
		{
			m_immune = false;
		}
	}

	//CHEAT
	//this->immune = true;
}

void Ship::ManageFeedbackExpiration(sf::Time deltaTime)
{
	//damage feedback expires?
	if (!m_ship_model->m_fake_textureName.empty())
	{
		assert(m_fake_ship != NULL);

		if (m_fake_ship->m_color_timer > sf::seconds(0))
		{
			m_fake_ship->m_color_timer -= deltaTime;
			m_fake_ship->setColor(m_fake_ship->m_color);
			if (m_fake_ship->m_color_timer < sf::seconds(0))
			{
				m_fake_ship->setColor(Color(255, 255, 255, 255));
			}
		}
	}
	if (m_color_timer > sf::seconds(0))
	{
		m_color_timer -= deltaTime;
		setColor(m_color);
		if (m_color_timer < sf::seconds(0))
		{
			setColor(Color(255, 255, 255, 255));
		}
	}
}

void Ship::MoveCursor(GameObject* cursor, sf::Vector2f inputs_directions, sf::Time deltaTime, SFPanel* container)
{
	if (!cursor)
		return;
	
	cursor->m_speed.x = inputs_directions.x * HUD_CURSOR_SPEED;
	cursor->m_speed.y = inputs_directions.y * HUD_CURSOR_SPEED;

	cursor->update(deltaTime);
	
	if (container)
	{
		//panel constraints
		sf::Vector2f panel_size = container->getSize();
		sf::Vector2f panel_pos = container->getOrigin() == sf::Vector2f(0, 0) ? sf::Vector2f(container->getPosition().x + panel_size.x / 2, container->getPosition().y + panel_size.y / 2) : container->getPosition();
		sf::Vector2f cursor_pos = cursor->getPosition();
		if (cursor_pos.x < panel_pos.x - (panel_size.x / 2))
		{
			cursor->setPosition(panel_pos.x - (panel_size.x / 2), cursor_pos.y);
			cursor->m_speed.x = 0;
		}
		if (cursor_pos.x > panel_pos.x + (panel_size.x / 2))
		{
			cursor->setPosition(panel_pos.x + (panel_size.x / 2), cursor_pos.y);
			cursor->m_speed.x = 0;
		}
		if (cursor_pos.y < panel_pos.y - (panel_size.y / 2))
		{
			cursor->setPosition(cursor_pos.x, panel_pos.y - (panel_size.y / 2));
			cursor->m_speed.y = 0;
		}
		if (cursor_pos.y > panel_pos.y + (panel_size.y / 2))
		{
			cursor->setPosition(cursor_pos.x, panel_pos.y + (panel_size.y / 2));
			cursor->m_speed.y = 0;
		}
	}
}


void Ship::BuyingItem()
{
	//case of weapon hovered
	if (m_SFPanel && m_SFPanel->GetFocusedItem()->m_weapon_loot)
	{
		if (m_money >= m_SFPanel->GetFocusedItem()->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS)
		{
			//if weapon already possessed, we try to put the item in stash
			if (m_weapon)
			{
				if (m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*m_SFPanel->GetFocusedItem()))
				{
					m_money -= m_SFPanel->GetFocusedItem()->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
					m_SFPanel->GetGrid()->setCellPointerForIntIndex(m_SFPanel->GetGrid()->GetIntIndex(m_SFPanel->GetFocusedIndex()), NULL);
					Ship::SavePlayerMoney(MONEY_SAVE_FILE, this);
					Ship::SaveItems(ITEMS_SAVE_FILE, this);
				}
			}
			//else we equip if directly
			else
			{
				setShipWeapon(m_SFPanel->GetFocusedItem()->m_weapon_loot->Clone());
				m_money -= m_SFPanel->GetFocusedItem()->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				m_HUD_SFPanel->GetGrid()->setCellPointerForIntIndex(NBVAL_Equipment, m_SFPanel->GetFocusedItem());
				m_SFPanel->GetGrid()->setCellPointerForIntIndex(m_SFPanel->GetGrid()->GetIntIndex(m_SFPanel->GetFocusedIndex()), NULL);
				Ship::SavePlayerMoney(MONEY_SAVE_FILE, this);

				(*CurrentGame).PlaySFX(SFX_Equip);
			}

			(*CurrentGame).PlaySFX(SFX_BuyOrSell);
		}
	}
	//case of equipment hovered
	else if (m_SFPanel && m_SFPanel->GetFocusedItem()->m_equipment_loot)
	{
		if (m_money >= m_SFPanel->GetFocusedItem()->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS)
		{
			//if equipment already possessed, we try to put the item in stash
			if (m_equipment[m_SFPanel->GetFocusedItem()->m_equipment_loot->m_equipmentType])
			{
				if (m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*m_SFPanel->GetFocusedItem()))
				{
					m_money -= m_SFPanel->GetFocusedItem()->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
					m_SFPanel->GetGrid()->setCellPointerForIntIndex(m_SFPanel->GetGrid()->GetIntIndex(m_SFPanel->GetFocusedIndex()), NULL);
					Ship::SavePlayerMoney(MONEY_SAVE_FILE, this);
					Ship::SaveItems(ITEMS_SAVE_FILE, this);

					
				}
			}
			//else we equip if directly
			else
			{
				setShipEquipment(m_SFPanel->GetFocusedItem()->m_equipment_loot->Clone());
				m_money -= m_SFPanel->GetFocusedItem()->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
				m_HUD_SFPanel->GetGrid()->setCellPointerForIntIndex(m_SFPanel->GetFocusedItem()->m_equipment_loot->m_equipmentType, m_SFPanel->GetFocusedItem());
				m_SFPanel->GetGrid()->setCellPointerForIntIndex(m_SFPanel->GetGrid()->GetIntIndex(m_SFPanel->GetFocusedIndex()), NULL);
				Ship::SavePlayerMoney(MONEY_SAVE_FILE, this);

				(*CurrentGame).PlaySFX(SFX_Equip);
			}

			(*CurrentGame).PlaySFX(SFX_BuyOrSell);
		}
	}
}

void Ship::SellingItem()
{
	//interaction
	if (m_HUD_SFPanel && m_SFPanel && m_HUD_SFPanel->GetFocusedItem())
	{
		//move item
		int focused_grid = m_HUD_SFPanel->GetFocusedGrid();
		int focused_index = m_HUD_SFPanel->GetGrid(false, focused_grid)->GetIntIndex(m_HUD_SFPanel->GetFocusedIndex());
		m_SFPanel->GetGrid()->insertObject(*m_HUD_SFPanel->GetFocusedItem(), -1, false);
		m_HUD_SFPanel->GetGrid(false, focused_grid)->setCellPointerForIntIndex(focused_index, NULL);

		//get the money
		int equip_type = m_HUD_SFPanel->GetFocusedItem()->m_weapon_loot ? NBVAL_Equipment : m_HUD_SFPanel->GetFocusedItem()->m_equipment_loot->m_equipmentType;
		if (equip_type == NBVAL_Equipment)
		{
			m_money += m_HUD_SFPanel->GetFocusedItem()->m_weapon_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
		}
		else
		{
			m_money += m_HUD_SFPanel->GetFocusedItem()->m_equipment_loot->m_credits * MONEY_COST_OF_LOOT_CREDITS;
		}

		//desequip if swapped from equipped items
		if (m_HUD_SFPanel->GetFocusedGrid() == 1)
		{
			if (equip_type == NBVAL_Equipment)
			{
				cleanWeapon(true);
			}
			else
			{
				cleanEquipment(equip_type, true);
			}

			(*CurrentGame).PlaySFX(SFX_Equip);
		}

		(*CurrentGame).PlaySFX(SFX_BuyOrSell);

		Ship::SaveItems(ITEMS_SAVE_FILE, this);
		
		//(*CurrentGame).m_hud.focused_item = NULL;
	}

	SavePlayerMoney(MONEY_SAVE_FILE, this);

	return;	
}

void Ship::EquipItem()
{
	//Equip
	if (m_HUD_SFPanel->GetFocusedItem() && m_HUD_SFPanel->GetFocusedGrid() == 2)
	{
		GameObject* tmp_ptr = m_HUD_SFPanel->GetFocusedItem();
		int equip_index_ = m_HUD_SFPanel->GetGrid(false, 2)->GetIntIndex(m_HUD_SFPanel->GetFocusedIndex());

		if (tmp_ptr->m_equipment_loot)
		{
			int ship_index_ = tmp_ptr->m_equipment_loot->m_equipmentType;
			ObjectGrid::SwapObjectsBetweenGrids(*m_HUD_SFPanel->GetGrid(false, 1), *m_HUD_SFPanel->GetGrid(false, 2), ship_index_, equip_index_);

			Equipment* new_equipment = m_HUD_SFPanel->GetGrid(false, 1)->getCellPointerFromIntIndex(ship_index_)->m_equipment_loot->Clone();
			this->setShipEquipment(new_equipment, true);
			new_equipment = NULL;
		}
		else if (tmp_ptr->m_weapon_loot)
		{
			int ship_index_ = NBVAL_Equipment;
			ObjectGrid::SwapObjectsBetweenGrids(*m_HUD_SFPanel->GetGrid(false, 1), *m_HUD_SFPanel->GetGrid(false, 2), ship_index_, equip_index_);

			Weapon* new_weapon = m_HUD_SFPanel->GetGrid(false, 1)->getCellPointerFromIntIndex(ship_index_)->m_weapon_loot->Clone();
			this->setShipWeapon(new_weapon, true);
			new_weapon = NULL;
		}
		else
		{
			LOGGER_WRITE(Logger::DEBUG, "<!> Error: trying to swap an item that has no equipment or weapon.\n");
		}

		(*CurrentGame).PlaySFX(SFX_Equip);

		tmp_ptr = NULL;
	}
}

void Ship::DesequipItem()
{
	//Desequip
	if (m_HUD_SFPanel->GetFocusedItem() && m_HUD_SFPanel->GetFocusedGrid() == 1)
	{
		GameObject* tmp_ptr = m_HUD_SFPanel->GetFocusedItem();
		int ship_index_ = m_HUD_SFPanel->GetGrid(false, 1)->GetIntIndex(m_HUD_SFPanel->GetFocusedIndex());

		if (tmp_ptr->m_equipment_loot)
		{
			if (m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*tmp_ptr))
			{
				cleanEquipment(ship_index_);
				m_HUD_SFPanel->GetGrid(false, 1)->setCellPointerForIntIndex(ship_index_, NULL);
			}
		}
		else if (tmp_ptr->m_weapon_loot)
		{
			if (m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*tmp_ptr))
			{
				cleanWeapon(ship_index_);
				m_HUD_SFPanel->GetGrid(false, 1)->setCellPointerForIntIndex(ship_index_, NULL);
			}
		}
		else
		{
			LOGGER_WRITE(Logger::DEBUG, "<!> Error: trying to swap an item that has no equipment or weapon.\n");
		}

		(*CurrentGame).PlaySFX(SFX_Equip);

		tmp_ptr = NULL;
	}
}

void Ship::GarbagingItem()
{
	if (m_inputs_states[Action_Braking] == Input_Tap && m_HUD_SFPanel->GetFocusedItem())
	{
		m_brakingHoldingClock.restart();
		m_HUD_SFPanel->GetCursor()->setAnimationLine(Cursor_Focus1_8, false);
		m_HUD_SFPanel->SetPrioritaryFeedback(true);
	}
	else if (m_HUD_SFPanel->GetPrioritaryFeedback())
	{
		if (m_HUD_SFPanel->GetFocusedItem() != m_previously_focused_item || m_inputs_states[Action_Braking] != Input_Hold)
		{
			m_brakingHoldingClock.restart();
			m_HUD_SFPanel->SetPrioritaryFeedback(false);
		}
		else// if (m_HUD_SFPanel->GetFocusedItem())
		{
			if (m_brakingHoldingClock.getElapsedTime() > sf::seconds(HUD_HOLD_TIME_BEFORE_REMOVE_ITEM))
			{
				m_HUD_SFPanel->GetCursor()->setAnimationLine(Cursor_Focus1_8);
				if (m_brakingHoldingClock.getElapsedTime().asSeconds() < HUD_HOLD_TIME_BEFORE_REMOVE_ITEM / 8)
					m_HUD_SFPanel->GetCursor()->setAnimationLine(Cursor_Focus1_8);

				int equip_type = NBVAL_Equipment;
				if (m_HUD_SFPanel->GetFocusedItem()->m_equipment_loot)
				{
					equip_type = m_HUD_SFPanel->GetFocusedItem()->m_equipment_loot->m_equipmentType;
				}
				//garbage in hud
				int grid_id_ = m_HUD_SFPanel->GetFocusedGrid();
				int index_ = m_HUD_SFPanel->GetGrid(false, grid_id_)->GetIntIndex(m_HUD_SFPanel->GetFocusedIndex());
				delete m_HUD_SFPanel->GetGrid(false, grid_id_)->grid[m_HUD_SFPanel->GetFocusedIndex().x][m_HUD_SFPanel->GetFocusedIndex().y];
				m_HUD_SFPanel->GetGrid(false, grid_id_)->setCellPointerForIntIndex(index_, NULL);

				//garbage for real
				if (m_HUD_SFPanel->GetFocusedGrid() == 1)
				{
					if (equip_type == NBVAL_Equipment)
					{
						this->cleanWeapon(true);
					}
					else
					{
						this->cleanEquipment(equip_type, true);
					}
				}
				//Save items
				SaveItems(ITEMS_SAVE_FILE, this);

				m_brakingHoldingClock.restart();
			}
			//holding feedback
			else
			{
				for (int k = 0; k < HUD_CURSOR_HOLDING_FRACTIONS; k++)
				{
					if (m_brakingHoldingClock.getElapsedTime().asSeconds() < (1.0f * HUD_HOLD_TIME_BEFORE_REMOVE_ITEM / HUD_CURSOR_HOLDING_FRACTIONS) * (k + 1))
					{
						m_HUD_SFPanel->GetCursor()->setAnimationLine((CursorFeedbackStates)(Cursor_Focus1_8 + k));
						break;
					}
				}
			}
		}
	}
}

void Ship::SettingTurnAnimations()
{
	//setting animation
	const sf::Vector2f f = (sf::Vector2f)GameObject::getDirectionMultiplier((*CurrentGame).m_direction);
	const float x = GameObject::getSize_for_Direction((*CurrentGame).m_direction, sf::Vector2f(m_speed.x * f.x, m_speed.y * f.y)).x;

	if (!m_ship_model->m_fake_textureName.empty())
	{
		if (x > 0 && m_currentAnimationIndex != ShipTurningRight && !m_disable_inputs)
		{
			m_currentAnimationIndex = ShipTurningRight;
		}

		else if (x < 0 && m_currentAnimationIndex != ShipTurningLeft && !m_disable_inputs)
		{
			m_currentAnimationIndex = ShipTurningLeft;
		}

		else if ((x == 0 && m_currentAnimationIndex != ShipIdle) || m_disable_inputs)
		{
			m_currentAnimationIndex = ShipIdle;
		}
	}
}

void Ship::RotateShip(float angle)
{
	setRotation(angle);
	if (m_fake_ship)
	{
		m_fake_ship->setRotation(angle);
	}
	if (m_combo_aura)
	{
		m_combo_aura->setRotation(angle);
	}
	if (m_trail)
	{
		m_trail->setRotation(angle);
	}
	for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
	{
		(*it)->setRotation(angle);
	}
}

void Ship::ScreenBorderContraints()
{
	//screen borders contraints	correction
	if (getPosition().x < m_size.x / 2)
	{
		setPosition(m_size.x / 2, getPosition().y);
		m_speed.x = 0;
	}

	if (getPosition().x > SCENE_SIZE_X - (m_size.x / 2))
	{
		setPosition(SCENE_SIZE_X - (m_size.x / 2), getPosition().y);
		m_speed.x = 0;
	}

	if (getPosition().y < m_size.y / 2)
	{
		setPosition(getPosition().x, m_size.y / 2);
		m_speed.y = 0;
	}

	if (getPosition().y > SCENE_SIZE_Y - (m_size.y / 2))
	{
		setPosition(getPosition().x, SCENE_SIZE_Y - (m_size.y / 2));
		m_speed.y = 0;
	}
	
}

void Ship::IdleDecelleration(sf::Time deltaTime)
{
	//idle deceleration
	if (!m_movingX || m_isFocusedOnHud == true)
	{
		m_speed.x -= (m_speed.x) * deltaTime.asSeconds()*(getFighterFloatStatValue(Fighter_Deceleration) / 100);

		if (abs(m_speed.x) < SHIP_MIN_SPEED_X)
			m_speed.x = 0;
	}

	if (!m_movingY || m_isFocusedOnHud == true)
	{
		m_speed.y -= (m_speed.y)*deltaTime.asSeconds()*(getFighterFloatStatValue(Fighter_Deceleration) / 100);

		if (abs(m_speed.y) < SHIP_MIN_SPEED_Y)
			m_speed.y = 0;
	}
}

void Ship::FillShopWithRandomObjets(size_t num_spawned_objects, Shop* shop, EnemyClass loot_class)
{
	assert(shop != NULL);
	//Generate random loots in shop
	for (size_t i = 0; i < num_spawned_objects; i++)
	{
		//double random_number = (double)rand() / (RAND_MAX);
		float random_beast_scale = RandomizeFloatBetweenValues(LootTable_BeastScale_Base[loot_class]);
		int loot_credits_ = ceil(random_beast_scale / BEAST_SCALE_TO_BE_ON_PAR_WITH_ENEMIES * (*CurrentGame).GetBonusStatsMultiplierToBeOnParForLevel(shop->m_level + 1));
		int equipment_type_roll = rand() % ((int)NBVAL_Equipment + 1);//+1 is for the weapon type

		if (Enemy::AssignRandomEquipment((EquipmentType)equipment_type_roll, loot_credits_, shop->m_level, shop, random_beast_scale))
		{
			GameObject* capsule = NULL;
			if (equipment_type_roll < NBVAL_Equipment)
			{
				capsule = Ship::CloneEquipmentIntoGameObject(shop->m_equipment_loot);
				shop->m_equipment_loot = NULL;
			}
			else
			{
				capsule = Ship::CloneWeaponIntoGameObject(shop->m_weapon_loot);
				shop->m_weapon_loot = NULL;
			}

			shop->m_items.push_back(capsule);
		}
		else
		{
			printf("<!> Error: could not generate equipment in Shop, with Enemy::AssignRandomEquipment(), on i=%d\n", i);
		}
	}
}

bool Ship::ResplenishHealth()
{
	bool hasHealthToResplenish = false;
	if (m_armor < m_armor_max)
	{
		m_armor = m_armor_max;
		hasHealthToResplenish = true;
	}
	if (m_shield < m_shield_max)
	{
		m_shield = m_shield_max;
		hasHealthToResplenish = true;
	}
	if (m_hyperspeed_fuel < m_hyperspeed_fuel_max)
	{
		m_hyperspeed_fuel = m_hyperspeed_fuel_max;
		hasHealthToResplenish = true;
	}
	return hasHealthToResplenish;
}

void Ship::Respawn()
{
	Init();
	//GenerateBots(this);
	ResplenishHealth();
	SetVisibility(true);
	SetBotsVisibility((*CurrentGame).m_direction != NO_DIRECTION);
	
	m_immune = true;
	m_immunityTimer.restart();
}

void Ship::SetVisibility(bool visible)
{
	m_visible = visible;
	if (m_fake_ship)
	{
		m_fake_ship->m_visible = visible;
	}

	SetBotsVisibility(visible);
}

void Ship::Death()
{
	FX* myFX = m_FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX, ExplosionLayer, Neutral);

	SetVisibility(false);
	m_graze_count = 0;
	m_graze_level = 0;

	(*CurrentGame).PlaySFX(SFX_BigKill);

	//DestroyBots();

	//(*CurrentGame).garbageLayer(AuraLayer);

	//losing xp
	//int death_xp_penalty_ = floor(this->xp_max * XP_DEATH_MALUS_PERCENTAGE);
	//if (this->xp < death_xp_penalty_)
	//{
	//	this->xp = 0;
	//}
	//else
	//{
	//	this->xp -= death_xp_penalty_;
	//}
}

GameObject* Ship::CloneEquipmentIntoGameObject(Equipment* new_equipment)
{
	assert(new_equipment != NULL);

	GameObject* capsule = new GameObject(sf::Vector2f(0, 0), sf::Vector2f(0, 0), new_equipment->m_textureName, new_equipment->m_size, sf::Vector2f(new_equipment->m_size.x / 2, new_equipment->m_size.y / 2), new_equipment->m_frameNumber);
	capsule->setEquipmentLoot(new_equipment->Clone());

	return capsule;
}

GameObject* Ship::CloneWeaponIntoGameObject(Weapon* new_weapon)
{
	assert(new_weapon != NULL);

	GameObject* capsule = new GameObject(new_weapon->getPosition(), sf::Vector2f(0, 0), new_weapon->m_textureName, new_weapon->m_size, sf::Vector2f(new_weapon->m_size.x / 2, new_weapon->m_size.y / 2), new_weapon->m_frameNumber);
	capsule->setWeaponLoot(new_weapon->Clone());

	return capsule;
}

bool Ship::GetLoot(GameObject& object)
{
	//EQUIPMENT
	if (object.m_equipment_loot)
	{
		bool success = false;
		GameObject* capsule = CloneEquipmentIntoGameObject(object.m_equipment_loot);
		//stash it
		if (m_equipment[object.m_equipment_loot->m_equipmentType])
		{
			success = m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*capsule, -1);
		}
		//equip it
		else
		{
			setShipEquipment(object.m_equipment_loot->Clone());
			//and update HUD
			success = m_HUD_SFPanel->GetGrid(false, 1)->insertObject(*capsule, object.m_equipment_loot->m_equipmentType, true);
		}
		
		if (success)
		{
			delete object.m_equipment_loot;
			object.m_equipment_loot = NULL;
		}
		
		if (success)
		{
			(*CurrentGame).PlaySFX(SFX_Equip);
		}

		return success;
	}
	//WEAPON
	else if (object.m_weapon_loot)
	{
		bool success = false;
		GameObject* capsule = CloneWeaponIntoGameObject(object.m_weapon_loot);
		//stash it
		if (m_weapon)
		{
			success = m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*capsule, -1);
		}
		//equip it
		else
		{
			setShipWeapon(object.m_weapon_loot->Clone());
			//and update HUD
			success = m_HUD_SFPanel->GetGrid(false, 1)->insertObject(*capsule, NBVAL_Equipment, true);
		}

		if (success)
		{
			delete object.m_weapon_loot;
			object.m_weapon_loot = NULL;
		}

		if (success)
		{
			(*CurrentGame).PlaySFX(SFX_Equip);
		}

		return success;
	}
	//MONEY
	else if (object.m_money > 0)
	{
		//feedback
		sf::Color _yellow = sf::Color::Color(255, 209, 53, 255);//yellow
		SFText* text_feedback = new SFText((*CurrentGame).m_font[Font_Terminator], 18, _yellow, getPosition());
		ostringstream ss;
		ss << "$ " << object.m_money;
		text_feedback->setString(ss.str());
		sf::Vector2f size = m_fake_ship ? m_fake_ship->m_size : m_size;
		text_feedback->setPosition(getPosition());
		SFTextPop* pop_feedback = new SFTextPop(text_feedback, 0, MONEY_LOOT_DISPLAY_NOT_FADED_TIME, MONEY_LOOT_DISPLAY_NOT_FADED_TIME, NULL, MONEY_LOOT_DISPLAY_SPEED_Y, sf::Vector2f(0, -size.y / 2 - TEXT_POP_OFFSET_Y));
		pop_feedback->setPosition(sf::Vector2f(pop_feedback->getPosition().x - pop_feedback->getGlobalBounds().width / 2, pop_feedback->getPosition().y));
		delete text_feedback;
		(*CurrentGame).addToFeedbacks(pop_feedback);

		//transfer money
		get_money_from(object);

		(*CurrentGame).PlaySFX(SFX_MoneyLoot);

		Ship::SavePlayerMoney(MONEY_SAVE_FILE, this);
		return true;
	}

	return false;
}

void Ship::GetPortal(GameObject* object)
{
	m_targetPortal = (Portal*)(object);
	m_isCollidingWithInteractiveObject = PortalInteraction;

	if ((*CurrentGame).m_direction == NO_DIRECTION && !(*CurrentGame).m_waiting_for_dialog_validation && m_targetPortal->m_state == PortalOpen)
	{
		m_is_asking_SFPanel = SFPanel_Portal;
	}
	else if ((*CurrentGame).m_direction != NO_DIRECTION && !(*CurrentGame).m_waiting_for_dialog_validation && m_targetPortal->m_state == PortalOpen)
	{
		m_is_asking_SFPanel = SFPanel_Action;
	}
}

void Ship::GetShop(GameObject* object)
{
	m_targetShop = (Shop*)(object);
	m_isCollidingWithInteractiveObject = ShopInteraction;

	if (!(*CurrentGame).m_waiting_for_dialog_validation)
	{
		if (m_HUD_state == HUD_ShopBuyMenu || m_HUD_state == HUD_ShopSellMenu)
		{
			m_is_asking_SFPanel = SFPanel_Inventory;
		}
		else if (m_HUD_state == HUD_ShopStellarMap)
		{
			m_is_asking_SFPanel = SFPanel_Map;
		}
		else
		{
			m_is_asking_SFPanel = SFPanel_Shop;
		}
	}
}

static int GrazeLevelsThresholds[NB_GRAZE_LEVELS] = { 0, 100, 500, 1500 };
static float GrazeLevelsBeastBonus[NB_GRAZE_LEVELS] = { 0.0f, 0.2f, 0.4f, 0.6f };

void Ship::GetGrazing()
{
	m_graze_count++;

	if (m_graze_level < NB_GRAZE_LEVELS - 1)
	{
		if (m_graze_count >= GrazeLevelsThresholds[m_graze_level + 1])
		{
			//Graze level up
			m_graze_level++;
			if (m_combo_aura && m_graze_level > 0 && m_graze_level < NB_GRAZE_LEVELS)
			{
				//update aura
				if (m_graze_level > GRAZE_LEVEL_NONE + 1)
				{
					m_combo_aura->setAnimationLine(m_graze_level, true);
				}
				//"new" aura
				else
				{
					m_combo_aura->setAnimationLine(m_graze_level, false);
				}
				
			}
		}
	}
}

int Ship::getGrazeCount()
{
	return m_graze_count;
}

float Ship::getShipBeastScore()
{
	float bonus = 0;
	if (m_graze_level < NB_GRAZE_LEVELS && m_graze_level >= 0)
	{
		bonus = GrazeLevelsBeastBonus[m_graze_level];
	}
	else
	{
		LOGGER_WRITE(Logger::DEBUG, "<!> Error, The ship has a 'graze_level' (%d) beyond existing values\n", this->graze_level);
		m_graze_count = 0;
		m_graze_level = 0;
	}

	return bonus;
}

void Ship::damage_from(GameObject& object)
{
	if (!m_immune)
	{
		if (!m_ship_model->m_fake_textureName.empty())
		{
			assert(m_fake_ship != NULL);
			m_fake_ship->setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));
		}
		this->setColor(Color(255, 0, 0, 255), sf::seconds(DAMAGE_FEEDBACK_TIME));

		if (object.m_damage > m_shield)
		{
			m_armor -= (object.m_damage - m_shield);
			if (m_shield > 0)
			{
				m_shield_recovery_clock.restart();
			}
			m_shield = 0;
		}
		else
		{
			m_shield -= object.m_damage;
		}
	}
	m_graze_count = 0;
	m_graze_level = GRAZE_LEVEL_NONE;
	if (m_combo_aura)
	{
		m_combo_aura->setAnimationLine(GRAZE_LEVEL_NONE);
	}
}


int Ship::GetFocusedPortalMaxUnlockedHazardLevel()
{
	if (m_targetPortal == NULL)
	{
		return -1;
	}
	else
	{
		return m_targetPortal->m_max_unlocked_hazard_level;
	}
}

//void Ship::gain_xp(int xp_earned_)
//{
//	this->xp += xp_earned_;
//
//	//no level down
//	if (this->xp < 0)
//	{
//		this->xp = 0;
//	}
//
//	while (this->xp >= this->xp_max && this->level < this->level_max)
//	{
//		this->LevelUp();
//	}
//
//	//max level reached?
//	if (this->xp > this->xp_max && this->level >= this->level_max)
//	{
//		this->xp = this->xp_max;
//	}
//}

//void Ship::LevelUp()
//{
//	this->level++;
//	this->xp -= this->xp_max;
//	this->xp_max = floor(this->xp_max * (1 + XP_MAX_INCREASE_PER_LEVEL));
//}

int Ship::UpdateShipLevel()
{
	int level_ = 1;
	int credits_ = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			credits_ += m_equipment[i]->m_credits;
		}
	}
	if (m_weapon)
	{
		credits_ += m_weapon->m_credits;
	}

	while (credits_ >= ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1))
	{
		credits_ -= ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1);
		level_++;
	}

	m_level = level_;
	m_xp = credits_;
	m_xp_max = ((*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_ + 1) - (*CurrentGame).GetEnemiesStatsMultiplierForLevel(level_)) * (NBVAL_Equipment + 1);

	return level_;
}

//SAVING AND LOADING ITEMS AND MONEY
int Ship::SavePlayerMoney(string file, Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving money in profile.\n");
	assert(ship != NULL);

	ofstream data(file.c_str(), ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		data << "Money " << ship->m_money << endl;

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for known scenes. A new file is going to be created.\n" << endl;
	}

	return 0;
}

bool Ship::LoadPlayerMoney(string file, Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading items from profile.\n");
	assert(ship != NULL);

	std::ifstream  data(file, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		while (std::getline(data, line))
		{
			string equipment_type;

			//Loading money

			std::istringstream(line) >> equipment_type >> ship->m_money;
		}
		
		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No MONEY SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Ship::SaveEquipmentData(ofstream& data, Equipment* equipment, bool skip_type)
{
	if (equipment)
	{
		if (!skip_type)
		{
			switch (equipment->m_equipmentType)
			{
				case Engine:
				{
					data << "Engine ";
					break;
				}
				case Armor:
				{
					data << "Armor ";
					break;
				}
				case Shield:
				{
					data << "Shield ";
					break;
				}
				case Module:
				{
					data << "Module ";
					break;
				}
				default:
				{
					data << "Unknown ";
					break;
				}
			}
		}

		data << equipment->m_display_name << " ";
		data << equipment->m_level << " ";
		data << equipment->m_credits << " ";
		data << equipment->m_quality << " ";
		data << equipment->m_textureName << " ";
		data << equipment->m_size.x << " ";
		data << equipment->m_size.y << " ";
		data << equipment->m_frameNumber << " ";
		data << equipment->m_max_speed << " ";
		data << equipment->m_deceleration << " ";
		data << equipment->m_acceleration << " ";
		data.precision(3);
		data << equipment->m_hyperspeed << " ";
		data.precision(0);
		data << equipment->m_hyperspeed_fuel << " ";
		data << equipment->m_armor << " ";
		data << equipment->m_shield << " ";
		data << equipment->m_shield_regen << " ";
		data.precision(3);
		data << equipment->m_shield_recovery_time << " ";
		data.precision(0);
		data << equipment->m_damage << " ";

		if (equipment->m_bot)
		{
			data << equipment->m_bot->m_display_name << " ";
			data << equipment->m_bot->m_textureName << " ";
			data << equipment->m_bot->m_size.x << " ";
			data << equipment->m_bot->m_size.y << " ";
			data << equipment->m_bot->m_frameNumber << " ";
			data << equipment->m_bot->m_spread.x << " ";
			data << equipment->m_bot->m_spread.y << " ";
			data << equipment->m_bot->m_rotation_speed << " ";
			data << equipment->m_bot->m_Pattern.currentPattern << " ";
			switch (equipment->m_bot->m_Pattern.currentPattern)
			{
				case NoMovePattern:
				{
					break;
				}
				case Line_:
				{
					data << equipment->m_bot->m_Pattern.patternSpeed << " ";
					data << &equipment->m_bot->m_Pattern.patternParams[1] << " ";
					break;
				}
				default:
				{
					data << equipment->m_bot->m_Pattern.patternSpeed << " ";
					data << &equipment->m_bot->m_Pattern.patternParams[0] << " ";
					data << &equipment->m_bot->m_Pattern.patternParams[1] << " ";
					break;
				}
			}

			if (equipment->m_bot->m_weapon)
			{
				Ship::SaveWeaponData(data, equipment->m_bot->m_weapon, true, true);
				return;
			}
			else
			{
				data << "0";
			}
		}
		else
		{
			data << "0";
		}
	}
	else
	{
		data << "0";
	}

	data << endl;
}

void Ship::SaveWeaponData(ofstream& data, Weapon* weapon, bool skip_type, bool skip_level)
{
	if (!skip_type)
	{
		data << "Weapon ";
	}

	if (weapon)
	{
		data << weapon->m_display_name << " ";
		if (!skip_level)
		{
			data << weapon->m_level << " ";
			data << weapon->m_credits << " ";
			data << weapon->m_quality << " ";
		}
		data << weapon->m_textureName << " ";
		data << weapon->m_size.x << " ";
		data << weapon->m_size.y << " ";
		data << weapon->m_frameNumber << " ";
		data.precision(3);
		data << weapon->m_rate_of_fire << " ";
		data.precision(0);
		data << weapon->m_rafale << " ";
		data.precision(3);
		data << weapon->m_rafale_cooldown << " ";
		data.precision(0);

		data << weapon->m_multishot << " ";
		data << weapon->m_xspread << " ";
		data << weapon->m_dispersion << " ";
		data << (int)weapon->m_shot_mode << " ";
		data << (int)weapon->m_target_seaking << " ";

		data << weapon->m_ammunition->m_display_name << " ";
		data << weapon->m_ammunition->m_damage << " ";
		data << weapon->m_ammunition->m_speed.y << " ";
		data << weapon->m_ammunition->m_range << " ";
		data << weapon->m_ammunition->m_textureName << " ";
		data << weapon->m_ammunition->m_size.x << " ";
		data << weapon->m_ammunition->m_size.y << " ";
		data << weapon->m_ammunition->m_frameNumber << " ";
		data << weapon->m_ammunition->m_explosion->m_display_name << " ";
		data << (int)weapon->m_ammunition->m_Pattern.currentPattern;
		if (weapon->m_ammunition->m_Pattern.currentPattern == Line_)
		{
			data << " " << weapon->m_ammunition->m_Pattern.patternSpeed << " ";
			data << &weapon->m_ammunition->m_Pattern.patternParams[1];
		}
		else if (weapon->m_ammunition->m_Pattern.currentPattern > NoMovePattern)
		{
			data << " " << weapon->m_ammunition->m_Pattern.patternSpeed << " ";
			data << &weapon->m_ammunition->m_Pattern.patternParams[0] << " ";
			data << &weapon->m_ammunition->m_Pattern.patternParams[1];
		}
	}
	else
	{
		data << "0";
	}

	data << endl;
}

int Ship::SaveItems(string file, Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Saving items in profile.\n");
	assert(ship != NULL);

	ofstream data(file.c_str(), ios::in | ios::trunc);
	if (data)  // si l'ouverture a réussi
	{
		// instructions
		for (int i = 0; i < NBVAL_Equipment; i++)
		{
			switch (i)
			{
				case Engine:
				{
					data << "Engine ";
					break;
				}
				case Armor:
				{
					data << "Armor ";
					break;
				}
				case Shield:
				{
					data << "Shield ";
					break;
				}
				case Module:
				{
					data << "Module ";
					break;
				}
				default:
				{
					data << "Unknown ";
					break;
				}
			}

			Ship::SaveEquipmentData(data, ship->m_equipment[i], true);
		}

		data << "Weapon ";
		Ship::SaveWeaponData(data, ship->m_weapon, true);

		for (size_t i = 0; i < ship->m_HUD_SFPanel->GetGrid(false, 2)->squares.x; i++)
		{
			for (size_t j = 0; j < ship->m_HUD_SFPanel->GetGrid(false, 2)->squares.y; j++)
			{
				if (ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[i][j])
				{
					if (ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[i][j]->m_equipment_loot)
					{
						Ship::SaveEquipmentData(data, ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[i][j]->m_equipment_loot, false);
					}
					else if (ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[i][j]->m_weapon_loot)
					{
						Ship::SaveWeaponData(data, ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[i][j]->m_weapon_loot, false);
					}
					else
					{
						data << "0";
						data << endl;
					}
				}
				else
				{
					data << "0";
					data << endl;
				}
			}
		}

		data.close();  // on ferme le fichier
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No save file found for known scenes. A new file is going to be created.\n" << endl;
	}

	return 0;
}

Equipment* Ship::LoadEquipmentFromLine(string line)
{
	string equipment_type;
	string display_name;

	std::istringstream ss(line);
	ss >> equipment_type >> display_name;
	//ss.str(line.substr(ss.tellg()));

	Equipment* equipment = new Equipment();

	string texture_name;
	int level;
	int credits;
	float quality;
	int width;
	int height;
	int frames;
	float max_speed;
	float deceleration;
	float acceleration;
	float hyperspeed;
	int hyperspeed_fuel;
	int armor;
	int shield;
	int shield_regen;
	float shield_recovery;
	int damage;

	string bot_name;
	string bot_texture_name;
	int bot_width;
	int bot_height;
	int bot_frames;
	float bot_spread_x;
	float bot_spread_y;
	float bot_rotation_speed;
	float bot_pattern_speed;
	int bot_pattern_type;
	float bot_pattern_arg1;
	float bot_pattern_arg2;

	string bot_weapon_name;
	string bot_weapon_texture_name;
	int bot_weapon_width;
	int bot_weapon_height;
	int bot_weapon_frames;
	float bot_weapon_rate_of_fire;
	int bot_weapon_rafale;
	float bot_rafale_cooldown;
	int bot_weapon_multishot;
	float bot_weapon_xspread;
	float bot_weapon_dispersion;
	int bot_weapon_shot_mode;
	int bot_weapon_target_seaking;

	string bot_ammo_name;
	int bot_ammo_damage;
	float bot_ammo_speed;
	float bot_ammo_range;
	string bot_ammo_texture_name;
	int bot_ammo_width;
	int bot_ammo_height;
	int bot_ammo_frames;
	int bot_ammo_pattern_type;
	float bot_ammo_pattern_speed;
	float bot_ammo_pattern_arg1;
	float bot_ammo_pattern_arg2;
	string bot_ammo_explosion_name;

	if (display_name.compare("0") == 0)
	{
		//do nothing
	}
	else
	{
		ss >> level >> credits >> quality >> texture_name >> width >> height >> frames >> max_speed >> deceleration >> acceleration >> hyperspeed >> hyperspeed_fuel >> armor >> shield >> shield_regen >> shield_recovery >> damage >> bot_name;

		if (bot_name.compare("0") == 0)
		{
			//do nothing
		}
		else
		{
			ss >> bot_texture_name >> bot_width >> bot_height >> bot_frames >> bot_spread_x >> bot_spread_y >> bot_rotation_speed >> bot_pattern_type;
			if (bot_pattern_type == Line_)
			{
				ss >> bot_pattern_speed >> bot_pattern_arg2;
			}
			else if (bot_pattern_type > NoMovePattern)
			{
				ss >> bot_pattern_speed >> bot_pattern_arg1 >> bot_pattern_arg2;
			}

			ss >> bot_weapon_name;
			if (bot_weapon_name.compare("0") == 0)
			{
				//do nothing
			}
			else
			{
				ss >> bot_weapon_texture_name >> bot_weapon_width >> bot_weapon_height >> bot_weapon_frames >> bot_weapon_rate_of_fire >>
					bot_weapon_rafale >> bot_rafale_cooldown >> bot_weapon_multishot >> bot_weapon_xspread >> bot_weapon_dispersion >> bot_weapon_shot_mode
					>> bot_weapon_target_seaking;

				ss >> bot_ammo_name >> bot_ammo_damage >> bot_ammo_speed >> bot_ammo_range >> bot_ammo_texture_name >> bot_ammo_width >> bot_ammo_height >>
					bot_ammo_frames >> bot_ammo_explosion_name >> bot_ammo_pattern_type;

				if (bot_ammo_pattern_type == Line_)
				{
					ss >> bot_ammo_pattern_speed >> bot_ammo_pattern_arg2;
				}
				else if (bot_ammo_pattern_type >> NoMovePattern)
				{
					ss >> bot_ammo_pattern_speed >> bot_ammo_pattern_arg1 >> bot_ammo_pattern_arg2;
				}
			}
		}
	}

	EquipmentType type = NBVAL_Equipment;
	if (equipment_type.compare("Shield") == 0)
	{
		type = Shield;
	}
	else if (equipment_type.compare("Armor") == 0)
	{
		type = Armor;
	}
	else if (equipment_type.compare("Engine") == 0)
	{
		type = Engine;
	}
	else if (equipment_type.compare("Module") == 0)
	{
		type = Module;
	}

	equipment->Init(type, max_speed, acceleration, deceleration, hyperspeed, hyperspeed_fuel, armor, shield, shield_regen, shield_recovery, damage, texture_name, sf::Vector2f(width, height), frames, display_name);
	equipment->m_level = level;
	equipment->m_credits = credits;
	equipment->m_quality = quality;
	if (bot_name.compare("0") != 0)
	{
		Bot* bot = new Bot(Vector2f(0, 0), Vector2f(0, 0), bot_texture_name, sf::Vector2f(bot_width, bot_height));
		bot->m_display_name = bot_name;
		bot->m_spread = sf::Vector2f(bot_spread_x, bot_spread_y);
		bot->m_rotation_speed = bot_rotation_speed;
		bot->m_Pattern.currentPattern = (PatternType)bot_pattern_type;
		if (bot->m_Pattern.currentPattern == Line_)
		{
			bot->m_Pattern.patternSpeed = bot_pattern_speed;
			bot->m_Pattern.patternParams->push_back(bot_pattern_arg2);
		}
		else if (bot->m_Pattern.currentPattern > NoMovePattern)
		{
			bot->m_Pattern.patternSpeed = bot_pattern_speed;
			bot->m_Pattern.patternParams->push_back(bot_pattern_arg1);
			bot->m_Pattern.patternParams->push_back(bot_pattern_arg2);
		}

		if (bot_weapon_name.compare("0") == 0)
		{
			//do nothing
		}
		else
		{
			Ammo* ammo = new Ammo(Vector2f(0, 0), sf::Vector2f(0, bot_ammo_speed), bot_ammo_texture_name, sf::Vector2f(bot_ammo_width, bot_ammo_height), bot_ammo_damage, Enemy::LoadFX(bot_ammo_explosion_name));
			ammo->m_display_name = bot_ammo_name;
			ammo->m_range = bot_ammo_range;
			ammo->m_Pattern.currentPattern = (PatternType)bot_ammo_pattern_type;
			if (ammo->m_Pattern.currentPattern == Line_)
			{
				ammo->m_Pattern.patternSpeed = bot_ammo_pattern_speed;
				ammo->m_Pattern.patternParams->push_back(bot_ammo_pattern_arg2);
			}
			else if (ammo->m_Pattern.currentPattern > NoMovePattern)
			{
				ammo->m_Pattern.patternSpeed = bot_ammo_pattern_speed;
				ammo->m_Pattern.patternParams->push_back(bot_ammo_pattern_arg1);
				ammo->m_Pattern.patternParams->push_back(bot_ammo_pattern_arg2);
			}

			Weapon* weapon = new Weapon(ammo);
			weapon->m_display_name = bot_weapon_name;
			weapon->m_fire_direction = Vector2i(0, -1);
			weapon->m_rate_of_fire = bot_weapon_rate_of_fire;
			weapon->m_shot_mode = (ShotMode)bot_weapon_shot_mode;

			weapon->m_multishot = bot_weapon_multishot;
			weapon->m_xspread = bot_weapon_xspread;
			weapon->m_dispersion = bot_weapon_dispersion;
			weapon->m_rafale = bot_weapon_rafale;
			weapon->m_rafale_cooldown = bot_rafale_cooldown;

			weapon->m_textureName = bot_weapon_texture_name;
			weapon->m_size = sf::Vector2f(bot_weapon_width, bot_weapon_height);
			weapon->m_frameNumber = bot_weapon_frames;
			weapon->m_target_seaking = (TargetSeaking)bot_weapon_target_seaking;

			bot->m_weapon = weapon;
		}

		equipment->m_bot = bot;
	}

	return equipment;
}

Weapon* Ship::LoadWeaponFromLine(string line)
{
	string equipment_type;
	string display_name;

	//std::istringstream(line) >> equipment_type >> display_name;

	std::istringstream ss(line);
	ss >> equipment_type >> display_name;

	Equipment* equipment = new Equipment();

	string weapon_texture_name;
	int weapon_level;
	int weapon_credits;
	float weapon_quality;
	int weapon_width;
	int weapon_height;
	int weapon_frames;
	float weapon_rate_of_fire;
	int weapon_rafale;
	float rafale_cooldown;
	int weapon_multishot;
	float weapon_xspread;
	float weapon_dispersion;
	int weapon_shot_mode;
	int weapon_target_seaking;

	string ammo_name;
	int ammo_damage;
	float ammo_speed;
	float ammo_range;
	string ammo_texture_name;
	int ammo_width;
	int ammo_height;
	int ammo_frames;
	int ammo_pattern_type;
	float ammo_pattern_speed;
	float ammo_pattern_arg1;
	float ammo_pattern_arg2;
	string ammo_explosion_name;

	ss >> weapon_level >> weapon_credits >> weapon_quality >> weapon_texture_name >> weapon_width >> weapon_height >> weapon_frames >> weapon_rate_of_fire >>
		weapon_rafale >> rafale_cooldown >> weapon_multishot >> weapon_xspread >> weapon_dispersion >> weapon_shot_mode
		>> weapon_target_seaking >> ammo_name >> ammo_damage >> ammo_speed >> ammo_range >> ammo_texture_name >> ammo_width >> ammo_height >>
		ammo_frames >> ammo_explosion_name >> ammo_pattern_type;
	if (ammo_pattern_type == Line_)
	{
		ss >> ammo_pattern_speed >> ammo_pattern_arg2;
	}
	else if (ammo_pattern_type > NoMovePattern)
	{
		ss >> ammo_pattern_speed >> ammo_pattern_arg1 >> ammo_pattern_arg2;
	}

	EquipmentType type = NBVAL_Equipment;

	Ammo* ammo = new Ammo(Vector2f(0, 0), sf::Vector2f(0, ammo_speed), ammo_texture_name, sf::Vector2f(ammo_width, ammo_height), ammo_damage, Enemy::LoadFX(ammo_explosion_name));
	ammo->m_display_name = ammo_name;
	ammo->m_range = ammo_range;
	ammo->m_Pattern.currentPattern = (PatternType)ammo_pattern_type;
	if (ammo->m_Pattern.currentPattern == Line_)
	{
		ammo->m_Pattern.patternSpeed = ammo_pattern_speed;
		ammo->m_Pattern.patternParams->push_back(ammo_pattern_arg2);
	}
	else if (ammo->m_Pattern.currentPattern > NoMovePattern)
	{
		ammo->m_Pattern.patternSpeed = ammo_pattern_speed;
		ammo->m_Pattern.patternParams->push_back(ammo_pattern_arg1);
		ammo->m_Pattern.patternParams->push_back(ammo_pattern_arg2);
	}

	Weapon* weapon = new Weapon(ammo);
	weapon->m_display_name = display_name;
	weapon->m_level = weapon_level;
	weapon->m_credits = weapon_credits;
	weapon->m_quality = weapon_quality;
	weapon->m_fire_direction = Vector2i(0, -1);
	weapon->m_rate_of_fire = weapon_rate_of_fire;
	weapon->m_shot_mode = (ShotMode)weapon_shot_mode;

	weapon->m_multishot = weapon_multishot;
	weapon->m_xspread = weapon_xspread;
	weapon->m_dispersion = weapon_dispersion;
	weapon->m_rafale = weapon_rafale;
	weapon->m_rafale_cooldown = rafale_cooldown;

	weapon->m_textureName = weapon_texture_name;
	weapon->m_size = sf::Vector2f(weapon_width, weapon_height);
	weapon->m_frameNumber = weapon_frames;
	weapon->m_target_seaking = (TargetSeaking)weapon_target_seaking;

	return weapon;
}

bool Ship::LoadPlayerItems(string file, Ship* ship)
{
	LOGGER_WRITE(Logger::DEBUG, "Loading items from profile.\n");
	assert(ship != NULL);

	std::ifstream  data(file, ios::in);

	if (data) // si ouverture du fichier réussie
	{
		std::string line;
		int i = 0;
		while (std::getline(data, line))
		{
			string equipment_type;
			string display_name;

			//Loading equipment
			if (i < NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type >> display_name;
				if (display_name.compare("0") != 0)
				{
					ship->setShipEquipment(Ship::LoadEquipmentFromLine(line), true, true);
				}
			}
			//Loading weapon
			else if (i == NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type >> display_name;
				if (display_name.compare("0") != 0)
				{
					ship->setShipWeapon(Ship::LoadWeaponFromLine(line), true, true);
				}
			}
			//Loading stash
			else //if (i > NBVAL_Equipment)
			{
				std::istringstream(line) >> equipment_type;

				int index = i - NBVAL_Equipment - 1;
				if (!ship->m_HUD_SFPanel)
				{
					printf("Error: <!> in Ship::LoadPlayerItems() <!> this ship doesn't have a HUD panel.\n");
				}
				else
				{
					int r = index % ship->m_HUD_SFPanel->GetGrid(false, 2)->squares.y;
					int l = index / ship->m_HUD_SFPanel->GetGrid(false, 2)->squares.y;

					if (ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[l][r])
					{
						delete ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[l][r];
						ship->m_HUD_SFPanel->GetGrid(false, 2)->grid[l][r] = NULL;
					}
					
					if (equipment_type.compare("0") != 0)
					{
						if (equipment_type.compare("Weapon") == 0)
						{
							Weapon* weapon = Ship::LoadWeaponFromLine(line);
							GameObject* capsule = ship->CloneWeaponIntoGameObject(weapon);
							ship->m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*capsule, index, true);
							delete weapon;
						}
						else
						{
							Equipment* equipment = Ship::LoadEquipmentFromLine(line);
							GameObject* capsule = ship->CloneEquipmentIntoGameObject(equipment);
							ship->m_HUD_SFPanel->GetGrid(false, 2)->insertObject(*capsule, index, true);
							delete equipment;
						}
					}
				}
			}

			i++;
		}

		data.close();  // on ferme le fichier

		return i == NBVAL_Equipment + 1 + EQUIPMENT_GRID_NB_LINES*EQUIPMENT_GRID_NB_ROWS;
	}
	else  // si l'ouverture a échoué
	{
		cerr << "DEBUG: No ITEMS SAVE FILE found. A new file is going to be created.\n" << endl;
		return false;
	}
}

void Ship::updatePostCollision()
{
	m_previouslyCollidingWithInteractiveObject = m_isCollidingWithInteractiveObject;
	m_isCollidingWithInteractiveObject = No_Interaction;
}

float Ship::getFighterFloatStatValue(FighterStats stat)
{
	float new_stat_value = 0;
	float equipment_value = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			switch (stat)
			{
				case Fighter_Hyperspeed:
				{
					equipment_value += m_equipment[i]->m_hyperspeed;
					break;
				}
				case Fighter_MaxSpeed:
				{
					equipment_value += m_equipment[i]->m_max_speed;
					break;
				}
				case Fighter_Acceleration:
				{
					equipment_value += m_equipment[i]->m_acceleration;
					break;
				}
				case Fighter_Deceleration:
				{
					equipment_value += m_equipment[i]->m_deceleration;
					break;
				}
				case Fighter_ShieldRecovery:
				{
					equipment_value += m_equipment[i]->m_shield_recovery_time;
					break;
				}
				default:
				{
					equipment_value += 0;
				}
			}
		}
		else
		{
			equipment_value += 0;
		}
	}

	//adding ship model stats
	switch (stat)
	{
		case Fighter_Hyperspeed:
		{
			new_stat_value = m_ship_model->m_hyperspeed + equipment_value;
			break;
		}
		case Fighter_MaxSpeed:
		{
			new_stat_value = m_ship_model->m_max_speed + equipment_value;
			break;
		}
		case Fighter_Acceleration:
		{
			new_stat_value = m_ship_model->m_acceleration + equipment_value;
			break;
		}
		case Fighter_Deceleration:
		{
			new_stat_value = m_ship_model->m_deceleration + equipment_value;
			break;				 
		}
		case Fighter_ShieldRecovery:
		{
			new_stat_value = m_ship_model->m_shield_recovery_time + equipment_value;
			break;
		}
		default:
		{
			new_stat_value = equipment_value;
		}
	}

	return new_stat_value;
}

int Ship::getFighterIntStatValue(FighterStats stat)
{
	int new_stat_value = 0;
	int equipment_value = 0;

	for (int i = 0; i < NBVAL_Equipment; i++)
	{
		if (m_equipment[i])
		{
			switch (stat)
			{
				case Fighter_Armor:
				{
					return m_armor;
				}
				case Fighter_ArmorMax:
				{
					equipment_value += m_equipment[i]->m_armor;
					break;
				}
				case Fighter_Shield:
				{
					return m_shield;
				}
				case Fighter_ShieldMax:
				{
					equipment_value += m_equipment[i]->m_shield;
					break;
				}
				case Fighter_ShieldRegen:
				{
					equipment_value += m_equipment[i]->m_shield_regen;
					break;
				}
				case Fighter_HyperspeedFuel:
				{
					equipment_value += m_equipment[i]->m_hyperspeed_fuel;
					break;
				}
				case Fighter_ContactDamage:
				{
					equipment_value += m_equipment[i]->m_damage;
					break;
				}
				case Fighter_Credits:
				{
					equipment_value += m_equipment[i]->m_credits;
					break;
				}
				case Fighter_Level:
				{
					equipment_value += m_equipment[i]->m_level;
					break;
				}
				default:
				{
					equipment_value += 0;
				}
			}
		}
		else
		{
			equipment_value += 0;
		}
	}

	//adding ship model stats
	switch (stat)
	{
		case Fighter_ArmorMax:
		{
			new_stat_value = m_ship_model->m_armor + equipment_value;
			break;
		}
		case Fighter_ShieldMax:
		{
			new_stat_value = m_ship_model->m_shield + equipment_value;
			break;
		}
		case Fighter_ShieldRegen:
		{
			new_stat_value = m_ship_model->m_shield_regen + equipment_value;
			break;
		}
		case Fighter_HyperspeedFuel:
		{
			new_stat_value = m_ship_model->m_hyperspeed_fuel + equipment_value;
			break;
		}
		case Fighter_ContactDamage:
		{
			new_stat_value = m_ship_model->m_damage + equipment_value;
			break;
		}
		case Fighter_Credits:
		{
			new_stat_value = equipment_value;
			break;
		}
		case Fighter_Level:
		{
			new_stat_value = equipment_value;
			break;
		}
		default:
		{
			new_stat_value = equipment_value;
		}
	}

	return new_stat_value;
}

void Ship::GenerateBots(GameObject* target)
{
	for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
	{
		(*it)->m_automatic_fire = m_automatic_fire;
		(*it)->m_spread = GameObject::getSize_for_Direction((*CurrentGame).m_direction, (*it)->m_spread);
		(*it)->setTarget(target);
		(*it)->rotate(GameObject::getRotation_for_Direction((*CurrentGame).m_direction));
		(*it)->m_visible = !m_disable_bots;
		(*CurrentGame).addToScene((*it), BotLayer, Neutral);
	}
}

void Ship::SetBotsVisibility(bool visible)
{
	for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
	{
		(*it)->m_visible = visible;
	}
}

void Ship::DestroyBots()
{
	for (std::vector<Bot*>::iterator it = (m_bot_list.begin()); it != (m_bot_list.end()); it++)
	{
		(*it)->m_visible = false;
		(*it)->m_GarbageMe = true;
	}
	m_bot_list.clear();
}

void Ship::ContinueDialog()
{
	string next = m_SFPanel->GetDialog()->m_next_dialog_name;
	if (m_SFPanel && !m_SFPanel->GetDialog()->m_next_dialog_name.empty() && m_SFPanel->GetDialog()->m_next_dialog_name.compare("0") != 0)
	{
		m_is_asking_SFPanel = SFPanel_DialogNext;
	}
	else
	{
		(*CurrentGame).m_waiting_for_dialog_validation = false;
		(*CurrentGame).m_end_dialog_clock.restart();
	}
	m_targetDialogs.erase(m_targetDialogs.begin());
}

void Ship::Teleport(string destination_name)
{
	m_is_asking_teleportation = destination_name;

	(*CurrentGame).PlaySFX(SFX_Teleport);
}

void Ship::CenterMapView(sf::Vector2f offset)
{
	m_SFPanel->SetMapViewOffset(offset);
}