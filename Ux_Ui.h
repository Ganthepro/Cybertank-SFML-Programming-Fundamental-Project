#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include "Data.h"
using namespace sf;
using namespace std;
class Ux_Ui
{
public:
	Ux_Ui(RenderWindow& win,int winWidth,int winHeight);
	void deathScreen();
	void startMenu(int count);
	void leaderboardScreen();
	void clearDeathScreen();
	void healthBar(Vector2f tankPos, int HP, vector<Vector2f> enamyTankPos, vector<int> enamyHP, vector<int> enamyMaxHP);
	void playerLockOn(vector<string> enamyTarget, vector<Sprite> missilePos, Vector2f tankPos);
	void reloadTimer(int rocketReloadTimer, int missileReloadTimer, int bulletReloadTimer);
	void waveBar(int wave, int waves);
	void ultBar(int percertile);
	void pauseScreen();
	void clearPauseScreen();
	void newData();
	void updateData();
	RectangleShape rect;
	RectangleShape GreenHealthBar;
	RectangleShape RedHealthBar;
	bool pauseScreenFlag = false;
	bool *isFirstTime;
	bool isLeaderboardShow = false;
	string *inputText;
	string devNameString = "Tamtikorn Kietipungchai 66010357";
	Text usernameText;
	Text devNameText;
	Text scoreText;
	int score = 0;
	Data data;
	vector<Data> datas;
	void loadDataToDatas();
private:
	RenderWindow& window;
	bool isMultipleScreen = false;
	float boardPos_X[3] = {float(window.getSize().x / 2 - 180),float(window.getSize().x / 2),float(window.getSize().x / 2 + 180)};
	Text pauseText;
	Text deathText;
	Text mainMenu;
	Text boardText;
	Font font;
	int width, height;
	Color colors[6] = { Color::Blue,Color::Red,Color::Yellow,Color::Green,Color::Magenta,Color::Cyan };
	int index = 0;
};