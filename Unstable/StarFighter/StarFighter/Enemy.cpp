#include "Enemy.h"

extern Game* CurrentGame;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	movepattern_type = 0;//type de pattern hardcodé pour l'instant
	FX_death = m_FX_death;
	hasWeapon = false;
}

void Enemy::update(sf::Time deltaTime)
{

	//sheld regen if not maximum
	if (shield < shield_max)
	{
		shield += shield_regen;
		//canceling over-regen
		if (shield > shield_max)
		{
			shield = shield_max;
		}
	}

	//automatic fire
	if(isOnScene & hasWeapon)
	{
		weapon->weaponOffset = sf::Vector2f((((this->m_size.x)/2) + (weapon->ammunition->m_size.x/2)) ,((this->m_size.y)/2) - (weapon->ammunition->m_size.y/2 *weapon->fire_direction.y)) ;
		weapon->setPosition(this->getPosition().x, this->getPosition().y);
		//weapon->setPosition(this->getPosition().x, (this->getPosition().y - ((this->m_size.y)/2)) );
		weapon->Fire(IndependantType::EnemyFire);

		//sheld regen if not maximum
		if (shield < getIndependantShield())
		{
			shield += getIndependantShieldRegen();
			//canceling over-regen
			if (shield > getIndependantShield())
			{
				shield = getIndependantShield();
			}
		}
	}
	Independant::update(deltaTime);
}

Enemy* Enemy::Clone()
{
	sf::Vector2f s = this->speed;

	Enemy* enemy = new Enemy(this->getPosition(), this->speed, this->textureName, this->m_size, this->FX_death);

	((Independant*)enemy)->armor = this->getIndependantArmor();
	((Independant*)enemy)->shield = this->getIndependantShield();
	((Independant*)enemy)->shield_regen = this->getIndependantShieldRegen();
	((Independant*)enemy)->damage = this->getIndependantDamage();
	enemy->hasWeapon = this->hasWeapon;
	if (enemy->hasWeapon)
		enemy->weapon = this->weapon->Clone();

	((Independant*)enemy)->addMoney(this->getMoney());
	enemy->hasEquipmentLoot = this->hasEquipmentLoot;
	enemy->equipment_loot = this->getEquipmentLoot();
	enemy->hasWeaponLoot = this->hasWeaponLoot;
	enemy->weapon_loot = this->getWeaponLoot();
	enemy->display_name = this->display_name;

	return enemy;
}

void Enemy::Death()
{
	FX* myFX = this->FX_death->Clone();
	myFX->setPosition(this->getPosition().x, this->getPosition().y);
	(*CurrentGame).addToScene(myFX,LayerType::ExplosionLayer, IndependantType::Neutral);
}

void Enemy::GenerateLoot()
{
	//TO DO : générer soit de la money, soit de l'équipment, soit une arme, et la donner à l'ennemi avec setMoney, setEquipmentLoot et setEquipmentWeapon
	if (this->hasWeaponLoot)
	{
		Loot* new_loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), this->getWeaponLoot()->textureName, sf::Vector2f(this->getWeaponLoot()->size.x, this->getWeaponLoot()->size.y), this->getWeaponLoot()->display_name);
		new_loot->get_weapon_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}

	else if (this->hasEquipmentLoot)
	{
		Loot* new_loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), this->getEquipmentLoot()->textureName, sf::Vector2f(this->getEquipmentLoot()->size.x, this->getEquipmentLoot()->size.y), this->getEquipmentLoot()->display_name);
		new_loot->get_equipment_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}

	else if (this->money > 0)
	{
		Loot* new_loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Money");
		new_loot->get_money_from(*this);
		(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
	}
	else
	{
		//que dalle
	}
}

#define PROBA_OF_DROPPING_SOMETHING			0.70
#define PROBA_OF_DROPPING_MONEY				0.40

#define MIN_DECELLERATION_DROP_STAT			200
#define DECELLERATION_PER_VALUE_UNIT		20

#define MIN_ACCELLERATION_DROP_STAT			100
#define ACCELLERATION_PER_VALUE_UNIT		10
#define MIN_MAX_SPEED_DROP_STAT				50
#define MAX_SPEED_PER_VALUE_UNIT			10

#define MIN_ARMOR_DROP_STAT					50
#define ARMOR_PER_VALUE_UNIT				30

#define MIN_SHIELD_DROP_STAT				100
#define SHIELD_PER_VALUE_UNIT				20
#define MIN_SHIELD_REGEN_DROP_STAT			100
#define SHIELD_REGEN_PER_VALUE_UNIT			20

#define MONEY_DROP_MIN_RATE					0.20
#define MONEY_DROP_MAX_RATE					1.0

#define EQUIPMENT_DROP_MIN_RATE				0.7
#define EQUIPMENT_DROP_MAX_RATE				1.3

#define WEAPON_BASE_RATE_OF_FIRE_MULTIPLIER		5
#define WEAPON_DISPERSION_DROP_RATE				0.50
#define WEAPON_DISPERSION_MAX_VALUE				170
#define WEAPON_ALTERNATE_DROP_RATE				0.25
#define WEAPON_MULTISHOT_MIN_VALUE				2
#define WEAPON_MULTISHOT_MAX_VALUE				10
#define WEAPON_MULTISHOT_BOT_MAX_VALUE			3
#define WEAPON_AMMO_SPEED_MIN_MULTIPLIER		1.5
#define WEAPON_AMMO_SPEED_MAX_MULTIPLIER		5
#define WEAPON_AMMO_BASE_DAMAGE					100
#define WEAPON_AMMO_BASE_BOT_DAMAGE				100
#define WEAPON_AMMO_BASE_VSPEED					300
#define WEAPON_AMMO_BASE_BOT_VSPEED				300
#define WEAPON_AMMO_DAMAGE_BASE_MULTIPLIER		3		

int RandomizeIntBetweenRatios(int value, float min_ratio, float max_ratio)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (max_ratio - min_ratio);
	random_rate += min_ratio;
	int i = (int) ((random_rate*value) + 0.5);

	return i;
}

float RandomizeFloatBetweenRatios(float value, float min_ratio, float max_ratio)
{
	double random_rate = ((double) rand() / RAND_MAX);
	random_rate *= (max_ratio - min_ratio);
	random_rate += min_ratio;
	float f = (float) (random_rate*value);

	return f;
}

int RandomizeIntBetweenValues(int min_value, int max_value)
{
	int random_value = rand() % (max_value - min_value +1);
	random_value += min_value;

	return random_value;
}

int RandomizeFloatBetweenValues(float min_value, float max_value)
{
	float random_value = (float) ((double) rand() / RAND_MAX);
	random_value *= (max_value - min_value);
	random_value += min_value;

	return random_value;
}

Loot* Enemy::CreateRandomLoot(int m_value)
{
	Loot* loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH), "Empty drop");
	this->setMoney(0);

	double random_number = (double) rand() / (RAND_MAX);

	if (random_number > PROBA_OF_DROPPING_SOMETHING)
	{	
		return loot;//empty drop: money = 0
	}
	else if (random_number < PROBA_OF_DROPPING_MONEY*PROBA_OF_DROPPING_SOMETHING)
	{
		int money = RandomizeIntBetweenRatios(m_value, MONEY_DROP_MIN_RATE, MONEY_DROP_MAX_RATE);
		loot->addMoney(money);

		return loot;
	}

	else
	{
		int dice_roll = rand() % ((int)EquipmentType::NBVAL_EQUIPMENT + 1);

		float equipment_drop_rate = RandomizeFloatBetweenRatios(1, EQUIPMENT_DROP_MIN_RATE, EQUIPMENT_DROP_MAX_RATE);
		int value = m_value*equipment_drop_rate;

		switch (dice_roll)
		{
		case (int)EquipmentType::Airbrake:
			{
				Equipment* equipment = new Equipment();
				int m_decelleration = MIN_DECELLERATION_DROP_STAT + (value*DECELLERATION_PER_VALUE_UNIT);
				equipment->Init((int)EquipmentType::Airbrake, sf::Vector2f(0,0),m_decelleration,sf::Vector2f(0,0),0,0,0,"Assets/2D/Airbrake.png",sf::Vector2f(64,64),1,"Airbrake");

				loot->setEquipmentLoot(equipment);
				return loot;
			}

		case (int)EquipmentType::Engine:
			{
				Equipment* equipment = new Equipment();
				sf::Vector2f m_acceleration = sf::Vector2f(MIN_ACCELLERATION_DROP_STAT + (value*ACCELLERATION_PER_VALUE_UNIT), MIN_ACCELLERATION_DROP_STAT + (value*ACCELLERATION_PER_VALUE_UNIT));
				sf::Vector2f m_max_speed = sf::Vector2f(MIN_MAX_SPEED_DROP_STAT + (value*MAX_SPEED_PER_VALUE_UNIT), MIN_MAX_SPEED_DROP_STAT + (value*MAX_SPEED_PER_VALUE_UNIT));
				equipment->Init((int)EquipmentType::Engine, m_max_speed,0,m_acceleration,0,0,0,"Assets/2D/Thruster.png",sf::Vector2f(64,64),1,"Engine");

				loot->setEquipmentLoot(equipment);
				return loot;
			}

		case (int)EquipmentType::Armor:
			{
				Equipment* equipment = new Equipment();
				int m_armor = MIN_ARMOR_DROP_STAT + (value*ARMOR_PER_VALUE_UNIT);
				equipment->Init((int)EquipmentType::Armor, sf::Vector2f(0,0),0,sf::Vector2f(0,0),m_armor,0,0,"Assets/2D/Armor.png",sf::Vector2f(64,64),1,"Armor");

				loot->setEquipmentLoot(equipment);
				return loot;
			}

		case (int)EquipmentType::Shield:
			{
				Equipment* equipment = new Equipment();
				int m_shield = MIN_SHIELD_DROP_STAT + (value*SHIELD_PER_VALUE_UNIT);
				int m_shield_regen = MIN_SHIELD_REGEN_DROP_STAT + (value*SHIELD_REGEN_PER_VALUE_UNIT);
				equipment->Init((int)EquipmentType::Shield, sf::Vector2f(0,0),0,sf::Vector2f(0,0),0,m_shield,m_shield_regen,"Assets/2D/Shield.png",sf::Vector2f(64,64),1,"Shield");

				loot->setEquipmentLoot(equipment);
				return loot;
			}

		case (int)EquipmentType::Module:
			{
				Equipment* equipment = new Equipment();
				Bot* bot = new Bot(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/enemyY.png", sf::Vector2f (35,35));
				bot->display_name = "Bot gerard";
				bot->radius = 500;
				bot->vspeed = 300;
				bot->spread = sf::Vector2f(-50,0);

				vector<float>* v = new vector<float>;
				//v->push_back(bot->radius); // rayon 500px
				//v->push_back(1);  // clockwise (>)

				PatternType pattern_type = PatternType::NoMovePattern;
				bot->Pattern.SetPattern(pattern_type,bot->vspeed,v); //vitesse angulaire (degres/s)

				FX* fx = new FX(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/FX_explosion_S_blue.png", sf::Vector2f (320,236), 2, sf::seconds(0.4f));
				Ammo* ammo = new Ammo(this->getPosition(),sf::Vector2f (0,WEAPON_AMMO_BASE_BOT_VSPEED),"Assets/2D/laser_blue.png",sf::Vector2f (4,16),WEAPON_AMMO_BASE_BOT_DAMAGE, fx);

				ammo->damage += value * WEAPON_AMMO_DAMAGE_BASE_MULTIPLIER;

				Weapon* weapon = new Weapon(ammo);

				int random_stat = rand() % (2+1);
				float combo_drop_rate = equipment_drop_rate;
				switch (random_stat)
				{
				case 0: 
					{
						float random_rate_of_fire_rate = RandomizeFloatBetweenRatios(1, EQUIPMENT_DROP_MIN_RATE, EQUIPMENT_DROP_MAX_RATE);
						weapon->rate_of_fire /= (random_rate_of_fire_rate * WEAPON_BASE_RATE_OF_FIRE_MULTIPLIER);
						if (combo_drop_rate < 1.5)
						{
							break;
						}
						else
						{
							combo_drop_rate /= 2;
						}

					}
				case 1:
					{
						weapon->multishot = RandomizeIntBetweenValues(WEAPON_MULTISHOT_MIN_VALUE, WEAPON_MULTISHOT_BOT_MAX_VALUE);

						if (weapon->multishot * weapon->xspread > 32)
							weapon->xspread = 32 / weapon->multishot;

						double dispersion_drop_rate = (double) rand() / (RAND_MAX);
						if (dispersion_drop_rate < WEAPON_DISPERSION_DROP_RATE)
						{
							int dispersion_roll = rand() % (WEAPON_DISPERSION_MAX_VALUE+1);
							weapon->dispersion = dispersion_roll;
						}

						double alternate_drop_rate = (double) rand() / (RAND_MAX);
						if (alternate_drop_rate < WEAPON_ALTERNATE_DROP_RATE)
						{
							weapon->alternate = true;
						}

						if (combo_drop_rate < 1.5)
						{
							break;
						}
						else
						{
							combo_drop_rate /= 2;
						}
					}
				case 2:
					{
						float random_ammo_speed_rate = RandomizeFloatBetweenRatios(1, EQUIPMENT_DROP_MIN_RATE, EQUIPMENT_DROP_MAX_RATE);
						weapon->ammunition->speed.y *= RandomizeFloatBetweenValues(WEAPON_AMMO_SPEED_MIN_MULTIPLIER, WEAPON_AMMO_SPEED_MAX_MULTIPLIER) * random_ammo_speed_rate;

						if (combo_drop_rate < 1.5)
						{
							break;
						}
						else
						{
							combo_drop_rate /= 2;
						}
					}

				default: break;
				}
				
				weapon->display_name = "laser_blue";
				weapon->fire_direction = Vector2i(0,-1);

				weapon->textureName = "Assets/2D/laser_red.png";
				weapon->size = sf::Vector2f(64,64);
				weapon->frameNumber = 1;

				bot->weapon = weapon;
				bot->hasWeapon = true;

				equipment->bot = bot;
				equipment->hasBot = true;

				equipment->Init((int)EquipmentType::Module, sf::Vector2f(0,0),0,sf::Vector2f(0,0),0,0,0,"Assets/2D/Module.png",sf::Vector2f(64,64),1,"Module");

				loot->setEquipmentLoot(equipment);
				return loot;
			}

		case (int)EquipmentType::NBVAL_EQUIPMENT://weapon drop
			{
				FX* fx = new FX(sf::Vector2f (0,0), sf::Vector2f (0,0), "Assets/2D/FX_explosion_S_blue.png", sf::Vector2f (320,236), 2, sf::seconds(0.4f));
				Ammo* ammo = new Ammo(this->getPosition(),sf::Vector2f (0,WEAPON_AMMO_BASE_VSPEED),"Assets/2D/laser_blue.png",sf::Vector2f (4,16),WEAPON_AMMO_BASE_DAMAGE, fx);
				
				ammo->damage += value * WEAPON_AMMO_DAMAGE_BASE_MULTIPLIER;

				Weapon* weapon = new Weapon(ammo);

				int random_stat = rand() % (2+1);
				float combo_drop_rate = equipment_drop_rate;
				switch (random_stat)
				{
				case 0: 
					{
						float random_rate_of_fire_rate = RandomizeFloatBetweenRatios(1, EQUIPMENT_DROP_MIN_RATE, EQUIPMENT_DROP_MAX_RATE);
						weapon->rate_of_fire /= (random_rate_of_fire_rate * WEAPON_BASE_RATE_OF_FIRE_MULTIPLIER);

						if (combo_drop_rate < 1.5)
						{
							break;
						}
						else
						{
							combo_drop_rate /= 2;
						}

					}
				case 1:
					{
						weapon->multishot = RandomizeIntBetweenValues(WEAPON_MULTISHOT_MIN_VALUE, WEAPON_MULTISHOT_MAX_VALUE);

						if (weapon->multishot * weapon->xspread > 32)
							weapon->xspread = 32 / weapon->multishot;

						double dispersion_drop_rate = (double) rand() / (RAND_MAX);
						if (dispersion_drop_rate < WEAPON_DISPERSION_DROP_RATE)
						{
							int dispersion_roll = rand() % (WEAPON_DISPERSION_MAX_VALUE+1);
							weapon->dispersion = dispersion_roll;
						}

						double alternate_drop_rate = (double) rand() / (RAND_MAX);
						if (alternate_drop_rate < WEAPON_ALTERNATE_DROP_RATE)
						{
							weapon->alternate = true;
						}

						if (combo_drop_rate < 1.5)
						{
							break;
						}
						else
						{
							combo_drop_rate /= 2;
						}
					}
				case 2:
					{
						float random_ammo_speed_rate = RandomizeFloatBetweenRatios(1, EQUIPMENT_DROP_MIN_RATE, EQUIPMENT_DROP_MAX_RATE);
						weapon->ammunition->speed.y *= RandomizeFloatBetweenValues(WEAPON_AMMO_SPEED_MIN_MULTIPLIER, WEAPON_AMMO_SPEED_MIN_MULTIPLIER) * random_ammo_speed_rate;
						
						if (combo_drop_rate < 1.5)
						{
							break;
						}
						else
						{
							combo_drop_rate /= 2;
						}
					}

				default: break;
				}

				weapon->display_name = "laser_blue";
				weapon->fire_direction = Vector2i(0,-1);

				weapon->textureName = "Assets/2D/laser_red.png";
				weapon->size = sf::Vector2f(64,64);
				weapon->frameNumber = 1;

				loot->setWeaponLoot(weapon);
				return loot;
			}

		default:
			return loot;
		}
	}
}