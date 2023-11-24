#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "ammoNdamage.h"
using namespace sf;
using namespace std;
class tankNmove
{
public:
	tankNmove(RenderWindow& win, ammoNdamage& Ammo, vector<RectangleShape> backgroundRect);
	void createTank();
	void draw(bool shootMissile, string targetID);
	void battleModeControlMouseMove(float mouseXpos, float mouseYpos);
	void battleModeControlKeyboardMove(float mouseXpos, float mouseYpos, vector<RectangleShape> backgroundRect);
	void driveModeControlKeyboardMove(vector<RectangleShape> backgroundRect);
	void raycasting(float radian,vector<RectangleShape> backgroundRect);
	bool checkRotateCollusion(vector<RectangleShape> backgroundRect);
	void shootMissile(Sprite sprite, string targetID);
	void resetTank();
	bool collusion = false;
	bool collusionFlag = false;
	string flagDirection = "";
	float getRotate();
	Vector2f getPos();
	FloatRect getBox();
	float speed = 0;
	float acceleration = 100.f;
	float driveAcceleration = 600.f;
	float retardation = 350.f;
	float breakRetardation = 500.f;
	bool direction[4] = { false, false, false, false };
	int angle_of_rotate;
	string mode;
	float tankPos[2];
	Sprite sprite;
	int hp = 100;
private:
	float rotate = 0.f;
	float spaceRotate = 0.f;
	float fRotate = 0;
	bool flagDirections[4] = { false, false, false, false };
	bool flagCollusion = false;
	Texture pngTank;
	ammoNdamage& ammo;
	RenderWindow& window;
	vector<RectangleShape> bg;
	bool flagSpeed = false;
	int count = 0;
	int count1 = 0;
	CircleShape shape;
};

