#include "itemNskill.h"
#include "enemytankNmove.h"

itemNskill::itemNskill(RenderWindow& win, enemytankNmove& enemy, ammoNdamage& Ammo) : window(win), enemyTank(enemy), ammo(Ammo) {
	icon_emp.loadFromFile(skillsIcon[0]);
	icon_charm.loadFromFile(skillsIcon[1]);
	icon_2xReload.loadFromFile(skillsIcon[2]);
	icon_ult.loadFromFile(skillsIcon[3]);
	empTexture.loadFromFile("resource/ani_electric.png");
	charmTexture.loadFromFile("resource/charm.png");
	empSprite.setTexture(empTexture);
	charmSprite.setTexture(charmTexture);
	texture.push_back(icon_emp);
	texture.push_back(icon_charm);
	texture.push_back(icon_2xReload);
	texture.push_back(icon_ult);
};

void itemNskill::dropItem(Vector2f position) {
	int randNum = rand() % 3;
	sprite = Sprite(texture[randNum]);
	sprite.setPosition(position);
	sprite.setScale(30.f / texture[randNum].getSize().x, 30.f / texture[randNum].getSize().y);
	itemsDropedPosition.push_back(position);
	itemsDropedSkill.push_back(skills[randNum]);
	itemsDropedSprite.push_back(sprite);
	itemsDropedTimer.push_back(0);
}

void itemNskill::draw(FloatRect tankBox, int *bulletReloadTimerLimit) {
	auto it = itemsDropedSprite.begin();
	auto it_timer = itemsDropedTimer.begin();
	auto it_skill = itemsDropedSkill.begin();
	auto it_enemyPos = enemyTank.enamyTankPos.begin();
	auto it_enemyStunt = enemyTank.enamyTankStuntTimer.begin();
	auto it_enemyCharm = enemyTank.enamyTankCharmTimer.begin();
	while (it != itemsDropedSprite.end()) {
		window.draw(*it);
		if (*it_timer == 900) {
			this->destroyItem(it->getPosition());
			break;
		}
		it_timer++;
		it++;
	}
	if (collectedSkill != "") 
		window.draw(collectedSprite);
	else
		this->colletedItem(tankBox);
	if (TwoxReloadTimer > 0)
		TwoxReloadTimer--;
	else
		*bulletReloadTimerLimit = 5;
	bool flagPlayAni = false;
	while (it_enemyPos != enemyTank.enamyTankPos.end()) {
		if (*it_enemyStunt > 0) {
			flagPlayAni = true;
			empSprite.setTextureRect(IntRect(empAniXpos, 0, 83.3, 150));
			empSprite.setPosition(*it_enemyPos - Vector2f(83.3 / 2.f,150 / 2.f));
			window.draw(empSprite);
		}
		if (*it_enemyCharm > 0) {
			charmSprite.setScale(30.f / charmTexture.getSize().x, 30.f / charmTexture.getSize().y);
			charmSprite.setPosition(*it_enemyPos - Vector2f(80, 70));
			window.draw(charmSprite);
		}
		it_enemyPos++;
		it_enemyStunt++;
		it_enemyCharm++;
	}
	if (flagPlayAni) {
		empAniXpos += 83.3;
		if (empAniXpos > 500)
			empAniXpos = 0;
	}
}

void itemNskill::destroyItem(Vector2f pos) {
	auto it = itemsDropedSprite.begin();
	auto it_timer = itemsDropedTimer.begin();
	auto it_pos = itemsDropedPosition.begin();
	auto it_skill = itemsDropedSkill.begin();
	while (it != itemsDropedSprite.end()) {
		if (*it_pos == pos) {
			itemsDropedPosition.erase(it_pos);
			itemsDropedTimer.erase(it_timer);
			itemsDropedSkill.erase(it_skill);
			itemsDropedSprite.erase(it);
			break;
		}
		it++;
		it_timer++;
		it_pos++;
		it_skill++;
	}
}

void itemNskill::colletedItem(FloatRect tankBox) {
	auto it = itemsDropedSprite.begin();
	auto it_skill = itemsDropedSkill.begin();
	if (collectedSkill == "") {
		while (it != itemsDropedSprite.end()) {
			if (it->getGlobalBounds().intersects(tankBox) && collectedSkill == "") {
				collectedSkill = *it_skill;
				collectedSprite.setTexture(*it->getTexture());
				float scale_x = 30.f / (*it->getTexture()).getSize().x;
				float scale_y = 30.f / (*it->getTexture()).getSize().y;
				collectedSprite.setScale(scale_x, scale_y);
				collectedSprite.setPosition(175, 25);
				this->destroyItem(it->getPosition());
				break;
			}
			it++;
			it_skill++;
		}
	}
}

void itemNskill::use_emp(Vector2f TankPos) {
	float radian = 250.f;
	auto it = enemyTank.enamyTankSprite.begin();
	auto it_move = enemyTank.enamyTankDoMove.begin();
	auto it_stunt = enemyTank.enamyTankStuntTimer.begin();
	while (it != enemyTank.enamyTankSprite.end()) {
		float opp = it->getPosition().y - TankPos.y;
		float adj = it->getPosition().x - TankPos.x;
		float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
		if (hyp < radian) 
			*it_stunt = 420;
		it++;
		it_move++;
		it_stunt++;
	}
}

void itemNskill::use_charm(Vector2f TankPos) {
	auto it = enemyTank.enamyTankSprite.begin();
	auto it_mode = enemyTank.enamyTankMode.begin();
	float distance = NULL;
	int i = 0;
	int closest_i = NULL;
	while (it != enemyTank.enamyTankSprite.end()) {
		if (*it_mode != "boss") {
			float opp = it->getPosition().y - TankPos.y;
			float adj = it->getPosition().x - TankPos.x;
			float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
			if (hyp < distance || distance == NULL) {
				distance = hyp;
				closest_i = i;
			}
		}
		it++;
		it_mode++;
		i++;
	}
	try {
		enemyTank.enamyTankCharmTimer[closest_i] = 900;
	}
	catch (const exception&) {}
}

void itemNskill::use_2xReload(int* bulletReloadTimerLimit) {
	*bulletReloadTimerLimit = 2.5;
	TwoxReloadTimer = 420;
}

void itemNskill::use_ult(Vector2f tankPos, float rotate) {
	auto it = enemyTank.enamyTankSprite.begin();
	auto it_rotate = enemyTank.enamyTankFrotate.begin();
	auto it_id = enemyTank.enamyTankID.begin();
	while (it != enemyTank.enamyTankSprite.end()) {
		ammo.createMissile(rotate, tankPos, *it_id, "player");
		it++;
		it_rotate++;
		it_id++;
	}
	enemyTank.percentile_for_ult = 0;
}