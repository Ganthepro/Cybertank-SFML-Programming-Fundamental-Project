#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include "Game.h"
using namespace sf;
using namespace std;

class ammoNdamage
{
public:
	ammoNdamage(RenderWindow& win);
	void createRocket(float rotate, Vector2f position, string status);
	void createBullet(float rotate, Vector2f position);
	void createMissile(float rotate, Vector2f position, string targetID, string status);
	void battleModeShootLeftMouse();
	void battleModeShootMiddleMouse();
	void battleModeShoot1Keyboard(vector<string> targetID, vector<Vector2f> targetPos, Vector2f playerTargetPos);
	void destroyMissile(RectangleShape it);
	vector<Sprite> rocket;
	vector<Sprite> bullet;
	vector<Sprite> missile;
	vector<string> missileTarget;
	vector<string> rocket_id;
	vector<string> missile_id;
	vector<float> missileFrotate;
	vector<pair<Sound, Sound>> rocketSounds;
	Sprite sprite;
	int i = 0;
private:
	RenderWindow& window;
	Texture pngRocket;
	Texture pngBullet;
	Texture pngMissile;
	int bulletSpeed = 2000;
	int rocketSpeed = 2000;
	Game game;
};

