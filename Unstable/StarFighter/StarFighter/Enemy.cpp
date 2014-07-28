#include "Enemy.h"

extern Game* CurrentGame;

Enemy::Enemy(sf::Vector2f position, sf::Vector2f speed, std::string textureName, sf::Vector2f size, FX* m_FX_death)  : Independant(position, speed,  textureName, size) 
{
	collider_type = IndependantType::EnemyObject;
	visible = true;
	movepattern_type = 0;//type de pattern hardcod� pour l'instant
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
	enemy->loot_equipment = this->getEquipmentLoot();
	enemy->hasWeaponLoot = this->hasWeaponLoot;
	enemy->loot_weapon = this->getWeaponLoot();

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
	//TO DO : g�n�rer soit de la money, soit de l'�quipment, soit une arme, et la donner � l'ennemi avec setMoney, setEquipmentLoot et setEquipmentWeapon
	Loot* new_loot = new Loot(this->getPosition(),sf::Vector2f(0, LOOT_SPEED_Y), LOOT_FILENAME, sf::Vector2f(LOOT_HEIGHT, LOOT_WIDTH));
	new_loot->get_money_from(*this);
	new_loot->get_equipment_from(*this);
	new_loot->get_weapon_from(*this);
	(*CurrentGame).addToScene((Independant*)new_loot, LayerType::PlayerShipLayer, IndependantType::LootObject);
}