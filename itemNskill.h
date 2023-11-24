#pragma once
#include <SFML/Graphics.hpp>
#include "enemytankNmove.h"
#include <iostream>
#include <vector>

using namespace std;
using namespace sf;

class itemNskill
{
public:
	itemNskill(RenderWindow& win, enemytankNmove& enemy, ammoNdamage& ammo);
	void dropItem(Vector2f position);
	void colletedItem(FloatRect tankBox);
	void draw(FloatRect tankBox, int *bulletReloadTimerLimit);
	void destroyItem(Vector2f pos);
	vector<int> itemsDropedTimer;
	void use_emp(Vector2f TankPos);
	void use_charm(Vector2f TankPos);
	void use_2xReload(int * bulletReloadTimerLimit);
	void use_ult(Vector2f tankPos, float rotate);
	string collectedSkill = "";
private:
	void usedItem();
	int TwoxReloadTimer = 0;
	int empAniXpos = 0;
	RenderWindow& window;
	enemytankNmove& enemyTank;
	ammoNdamage& ammo;
	vector<string> itemsDropedSkill;
	vector<Vector2f> itemsDropedPosition;
	vector<Sprite> itemsDropedSprite;
	vector<Texture> texture;
	Sprite collectedSprite;
	Sprite empSprite;
	Sprite charmSprite;
	Texture empTexture;
	Texture charmTexture;
	string skills[4] = { "emp","charm","2xReload","ult" }; // เพิ่ม heal
	string skillsIcon[4] = { "resource/emp.png","resource/charm.png","resource/2x.png","resource/ult.png" };
	Texture icon_emp;
	Texture icon_charm;
	Texture icon_2xReload;
	Texture icon_ult;
	Sprite sprite;
};