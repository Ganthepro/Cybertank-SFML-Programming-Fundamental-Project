#pragma once
#include <SFML/Graphics.hpp>
#include "ammoNdamage.h"
#include "Game.h"
#include <iostream>
#include <vector>
#include <array>
#include "SoundsStruct.h"

using namespace sf;
using namespace std;
class enemytankNmove
{
public:
	enemytankNmove(RenderWindow& win, ammoNdamage& Ammo, vector<RectangleShape> backgroundRect);
	void createEnemyTank(vector<RectangleShape> it_bg, FloatRect tankBox);
	void createEnemyBoss(vector<RectangleShape> it_bg, FloatRect tankBox);
	void findPos(vector<RectangleShape> it_bg, FloatRect tankBox);
	void draw(FloatRect tankPos, Vector2f playerTankPos);
	bool raycasting(float radian, vector<RectangleShape> backgroundRect, Vector2f Pos, FloatRect tankPos, bool showable, float distance, array<float,4> arr, int index);
	float enemyControlMouseMove(Vector2f tankPos, Vector2f targetPos);
	void enamyAIShoot(string mode, Sprite sprite, string status, string targetID, bool isBoss);
	void destroyTank(string tankID);
	void Wave(FloatRect tankBox);
	void findCharmTarget();
	void enemyCharmedControlMouseMove(Vector2f targetPos);
	Vector2f enamyAIMove(vector<bool> moveDirection, Vector2f it_pos, vector<Sprite> enamyTankSprite, vector<Vector2f> enamyTankpos, Vector2f playerTankPos, vector<RectangleShape> backgroundRect);
	float speed = 0;
	float acceleration = 100.f;
	float driveAcceleration = 600.f;
	float retardation = 50.f;
	float breakRetardation = 300.f;
	bool direction[4] = { false, false, false, false };
	float AI_raycastingDistance[4] = { 0, 0, 0, 0 };
	int waves = 1;
	int wave = 1;
	vector<float> missileFrotate;
	vector<Vector2f> enamyTankPos;
	vector<Sprite> enamyTankSprite;
	vector<float> enamyTankRotate;
	vector<float> enamyTankFrotate;
	vector<string> enamyTankMode;
	vector<int> enamyTankReloadTimer;
	vector<bool> enamyTankDoMove;
	vector<int> enamyTankStuntTimer;
	vector<bool> enamyTankShootable;
	vector<int> enamyTankCharmTimer;
	vector<int> enamyTankCharmTarget;
	vector<string> enamyTankID;
	vector<int> enamyTankHP;
	vector<int> enamyTankMaxHP;
	vector<Vector2f> enamyTankNextMove;
	vector<array<float, 4>> enamyTankDistance_to_Wall;
	vector<bool> enamyTankWallAlert;
	string mode;
	string status = "enamy";
	int count = 0;
	Sprite sprite;
	Sprite boss_sprite;
	Sprite bossSprite;
	vector<bool> moveDirection;
	int percentile_for_ult = 0;
private:
	int hp = 100;
	float conner[8] = { 90, 270, 180, 0, 45, 135, 225, 315 };
	float rotate = 0.f;
	float spaceRotate = 0.f;
	float fRotate = 0;
	float fRotate2 = 0.f;
	Vector2f playerPos;
	RenderWindow& window;
	ammoNdamage& ammo;
	Texture pngTank;
	Texture pngBoss;
	vector<RectangleShape> bg;
	bool waveFlag = false;
	bool shootable = false;
	Game game;
};

