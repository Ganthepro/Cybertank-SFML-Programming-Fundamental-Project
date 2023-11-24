#include "Ux_Ui.h"
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <iostream>
#include <fstream>
#include <string>
using namespace sf;
using namespace std;

Ux_Ui::Ux_Ui(RenderWindow& win, int winWidth, int winHeight) : window(win) {
	width = winWidth;
	height = winHeight;
	mainMenu.setFont(font);
	mainMenu.setStyle(Text::Bold);
	mainMenu.setFillColor(Color::White);
	mainMenu.setCharacterSize(50);
	mainMenu.setPosition(width / 2, height / 2);
	mainMenu.setOutlineColor(Color::Black);
	mainMenu.setOutlineThickness(3.f);
	usernameText.setFont(font);
	usernameText.setFillColor(Color::White);
	usernameText.setPosition(window.getSize().x / 2, 30);
	usernameText.setOutlineColor(Color::Black);
	usernameText.setOutlineThickness(1.f);
	scoreText.setFont(font);
	scoreText.setFillColor(Color::White);
	scoreText.setPosition(width - 200, 30);
	boardText.setFont(font);
	boardText.setFillColor(Color::White);
	boardText.setPosition(width / 2, 30);
	boardText.setOutlineColor(Color::Black);
	boardText.setOutlineThickness(1.f);
	boardText.setCharacterSize(50);
	boardText.setString("Leaderboard (Press Q to Back)");
	boardText.setStyle(Text::Bold);
	devNameText.setFont(font);
	devNameText.setFillColor(Color::White);
	devNameText.setPosition(width / 2, 30);
	devNameText.setOutlineColor(Color::Black);
	devNameText.setOutlineThickness(1.f);
	devNameText.setCharacterSize(50);
	devNameText.setPosition(10.f, 10.f);
	rect.setPosition(0, 0);
	rect.setSize(Vector2f(window.getSize()));
}

void Ux_Ui::loadDataToDatas() {
	string line;
	ifstream inputFile("resource/leaderboard.txt");
	if (!inputFile.is_open())
		return;
	Data data;
	while (getline(inputFile, line)) {
		if (line != "---") {
			try {
				data.score = stod(line);
				datas.push_back(data);
			}
			catch (const invalid_argument&) {
				data.name = line;
			}
		}
	}
	inputFile.close();
}

void Ux_Ui::leaderboardScreen() {
	string line;
	for (int i = 0; i < datas.size(); i++) {
		for (int n = i; n < datas.size(); n++) {
			if (datas[i].score < datas[n].score) {
				Data x = datas[i];
				datas[i] = datas[n];
				datas[n] = x;
			}
		}
	}
	ofstream outputFile("resource/leaderboard.txt");
	if (!outputFile.is_open()) {
		cout << "Error opening file." << endl;
		return;
	}
	auto it_line = datas.begin();
	while (it_line != datas.end()) {
		outputFile << "---" << endl;
		outputFile << it_line->name << endl;
		outputFile << to_string(it_line->score) << endl;
		it_line++;
	}
	outputFile.close();
	ifstream inputFile("resource/leaderboard.txt");
	if (!inputFile.is_open()) {
		cout << "Error opening file." << endl;
		return;	
	}
	int list = 1;
	int counter = 0;
	int boardPos_Y = 40;
	while (getline(inputFile, line)) {
		Text text;
		text.setFont(font);
		text.setFillColor(Color::White);
		text.setOutlineColor(Color::Black);
		text.setOutlineThickness(0.5);
		text.setStyle(Text::Bold);
		if (line == "---") {
			counter = 0;
			boardPos_Y += 42.5;
			text.setString(to_string(list));
			list++;
		}
		else {
			text.setString(line);
			counter++;
		}
		FloatRect shapeBounds = text.getLocalBounds();
		Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
		text.setOrigin(shapeCenter);
		text.setPosition(boardPos_X[counter], boardPos_Y);
		window.draw(text);
	}
	inputFile.close();
	return;
}

void Ux_Ui::newData() {
	bool isAgain = false;
	ifstream inputFile("resource/leaderboard.txt");
	if (!inputFile.is_open()) 
		return;
	string line;
	int list = 1;
	int counter = 0;
	while (getline(inputFile, line)) {
		try {
			stoi(line);
		} 
		catch (const invalid_argument&) {
			if (line != "---") {
				if (line == *inputText) {
					isAgain = true;
					break;
				}
			}
		}
	}
	inputFile.close();
	if (!isAgain) {
		ofstream outputFile("resource/leaderboard.txt", ios::app);
		if (!outputFile.is_open()) 
			return;
		outputFile << "---" << endl;
		outputFile << *inputText << endl;
		outputFile << to_string(0) << endl;
		data.name = *inputText;
		data.score = 0;
		datas.push_back(data);
		outputFile.close();
	}
	return;
}

void Ux_Ui::updateData() {
	ifstream inputFile("resource/leaderboard.txt");
	if (!inputFile.is_open())
		return;
	string line;
	bool isInput = false;
	while (getline(inputFile, line)) {
		try {
			stoi(line);
		}
		catch (const invalid_argument&) {
			if (line != "---") {
				if (isInput == true) {
					*inputText = line;
					break;
				}
				isInput = true;
			}
			else
				isInput = false;
		}
	}
	vector<string> lines;
	int i = 0;
	int editLine = 0;
	ifstream inputFile2("resource/leaderboard.txt");
	if (!inputFile2.is_open())
		return;
	while (getline(inputFile2, line)) {
		lines.push_back(line);
		if (line == *inputText) 
			editLine = i;
		i++;
	}
	inputFile2.close();
	if (lines.size() < editLine) 
		return;
	if (stod(lines[editLine + 1]) < score) 
		lines[editLine + 1] = to_string(score);
	ofstream outputFile("resource/leaderboard.txt");
	if (!outputFile.is_open())
		return;
	for (const string& updatedLine : lines) 
		outputFile << updatedLine << endl;
	outputFile.close();
	return;
}

void Ux_Ui::startMenu(int count) {
	window.clear();
	if (count == 60)
		index = rand() % 5;
	rect.setFillColor(colors[index]);
	if (!font.loadFromFile("resource/Montserrat-VariableFont_wght.ttf")) {}
	FloatRect shapeBounds = mainMenu.getLocalBounds();
	Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
	mainMenu.setOrigin(shapeCenter);
	boardText.setOrigin(Vector2f(boardText.getLocalBounds().left + boardText.getLocalBounds().width / 2.f, boardText.getLocalBounds().top + boardText.getLocalBounds().height / 2.f));
	window.draw(rect);
	if (!*isFirstTime) {
		if (isLeaderboardShow) {
			this->leaderboardScreen();
			mainMenu.setString("");
			window.draw(boardText);
		}
		else {
			mainMenu.setString("Press SPACE to Start\nPress E to see Leaderboard\nPress ESC to Exit");
			window.draw(usernameText);
		}
	}
	else 
		mainMenu.setString("Username : " + *inputText);
	devNameText.setString(devNameString);
	window.draw(devNameText);
	window.draw(mainMenu);
}

void Ux_Ui::deathScreen() {
	window.clear();
	rect.setSize(Vector2f(window.getSize()));
	rect.setFillColor(Color::Red);
	rect.setPosition(0, 0);
	Font font;
	if (!font.loadFromFile("resource/Montserrat-VariableFont_wght.ttf")) {}
	deathText.setFont(font);
	deathText.setStyle(Text::Bold);
	deathText.setFillColor(Color::White);
	deathText.setCharacterSize(50);
	FloatRect shapeBounds = deathText.getLocalBounds();
	Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
	deathText.setOrigin(shapeCenter);
	deathText.setPosition(width / 2, height / 2);
	deathText.setString("Press SPACE to Restart\nPress Q to Exit");
	window.draw(rect);
	window.draw(deathText);
	window.display();
}

void Ux_Ui::clearDeathScreen() {
	window.clear();
	rect.setFillColor(Color::Transparent);
	deathText.setFillColor(Color::Transparent);
	rect.setSize(Vector2f(0, 0));
	window.draw(rect);
	window.display();
}

void Ux_Ui::playerLockOn(vector<string> enamyTarget, vector<Sprite> missilePos, Vector2f tankPos) {
	auto it_enamyTarget = enamyTarget.begin();
	auto it_enamyMissilePos = missilePos.begin();
	while (it_enamyTarget != enamyTarget.end()) {
		float rotate = 0.f;
		if (*it_enamyTarget == "player") {
			if (it_enamyMissilePos->getPosition().x > tankPos.x && it_enamyMissilePos->getPosition().y < tankPos.y) {
				float opp = tankPos.y - it_enamyMissilePos->getPosition().y;
				float adj = it_enamyMissilePos->getPosition().x - tankPos.x;
				float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
				rotate = asin(opp / hyp) * (180.f / 3.14);
			}
			else if (it_enamyMissilePos->getPosition().x < tankPos.x && it_enamyMissilePos->getPosition().y < tankPos.y) {
				float opp = tankPos.y - it_enamyMissilePos->getPosition().y;
				float adj = tankPos.x - it_enamyMissilePos->getPosition().x;
				float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
				rotate = (90.f - (asin(opp / hyp) * (180.f / 3.14))) + 90.f;
			}
			else if (it_enamyMissilePos->getPosition().x < tankPos.x && it_enamyMissilePos->getPosition().y > tankPos.y) {
				float opp = it_enamyMissilePos->getPosition().y - tankPos.y;
				float adj = tankPos.x - it_enamyMissilePos->getPosition().x;
				float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
				rotate = asin(opp / hyp) * (180.f / 3.14) + 180.f;
			}
			else if (tankPos.x < it_enamyMissilePos->getPosition().x && it_enamyMissilePos->getPosition().y > tankPos.y) {
				float opp = it_enamyMissilePos->getPosition().y - tankPos.y;
				float adj = it_enamyMissilePos->getPosition().x - tankPos.x;
				float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
				rotate = (180.f - (asin(opp / hyp) * (180.f / 3.14))) + 180.f;
			}
			float radians = rotate * 3.14 / 180.0;
			VertexArray line(Lines, 2);
			line[0].position = Vector2f(tankPos.x + (cos(radians) * 50), tankPos.y + (-(sin(radians)) * 50));
			line[1].position = Vector2f(tankPos.x + (cos(radians) * 100), tankPos.y + (-(sin(radians)) * 100));
			line[0].color = Color(255, 165, 0);
			line[1].color = Color(255, 165, 0);
			window.draw(line);
		}
		it_enamyTarget++;
		it_enamyMissilePos++;
	}
}

void Ux_Ui::healthBar(Vector2f tankPos, int hp, vector<Vector2f> enamyTankPos, vector<int> enamyHP, vector<int> enamyMaxHP) {
	int healthBarWidth = 60;
	float GreenHealthBarWidth = hp * healthBarWidth / 100;
	GreenHealthBar.setFillColor(Color::Green);
	RedHealthBar.setFillColor(Color::Red);
	GreenHealthBar.setSize(Vector2f(GreenHealthBarWidth, 5));
	RedHealthBar.setSize(Vector2f(healthBarWidth - GreenHealthBarWidth, 5));
	GreenHealthBar.setPosition(tankPos.x - 30, tankPos.y - 50);
	RedHealthBar.setPosition(GreenHealthBar.getPosition().x + GreenHealthBar.getGlobalBounds().width, GreenHealthBar.getPosition().y);
	auto it_enamyPos = enamyTankPos.begin();
	auto it_enamyHP = enamyHP.begin();
	auto it_enamyMaxHP = enamyMaxHP.begin();
	while (it_enamyPos != enamyTankPos.end()) {
		RectangleShape enamyGreenHealthBar;
		RectangleShape enamyRedHealthBar;
		GreenHealthBarWidth = *it_enamyHP * healthBarWidth / *it_enamyMaxHP;
		enamyGreenHealthBar.setFillColor(Color::Green);
		enamyRedHealthBar.setFillColor(Color::Red);
		enamyGreenHealthBar.setSize(Vector2f(GreenHealthBarWidth, 5));
		enamyRedHealthBar.setSize(Vector2f(healthBarWidth - GreenHealthBarWidth, 5));
		enamyGreenHealthBar.setPosition(it_enamyPos->x - 45, it_enamyPos->y - 50);
		enamyRedHealthBar.setPosition(enamyGreenHealthBar.getPosition().x + enamyGreenHealthBar.getGlobalBounds().width, enamyGreenHealthBar.getPosition().y);
		window.draw(enamyGreenHealthBar);
		window.draw(enamyRedHealthBar);
		it_enamyPos++;
		it_enamyHP++;
		it_enamyMaxHP++;
	}
	window.draw(GreenHealthBar);
	window.draw(RedHealthBar);
}

void Ux_Ui::ultBar(int percertile) {
	RectangleShape ultBarPurple;
	RectangleShape ultBarGray;
	ultBarPurple.setFillColor(Color::Magenta);
	ultBarGray.setFillColor(Color(88, 94, 89));
	int barWidth = 300;
	float UltBarWidth = percertile * barWidth / 120;
	ultBarPurple.setSize(Vector2f(UltBarWidth, 5));
	ultBarGray.setSize(Vector2f(barWidth - UltBarWidth, 5));
	ultBarPurple.setPosition(1000, 40);
	ultBarGray.setPosition(ultBarPurple.getPosition().x + ultBarPurple.getGlobalBounds().width, ultBarPurple.getPosition().y);
	window.draw(ultBarPurple);
	window.draw(ultBarGray);
}

void Ux_Ui::reloadTimer(int rocketReloadTimer, int missileReloadTimer, int bulletReloadTimer) {
	RectangleShape bulletTimerWhite;
	RectangleShape bulletTimerGray;
	RectangleShape missileTimerWhite;
	RectangleShape missileTimerGray;
	RectangleShape rocketTimerWhite;
	RectangleShape rocketTimerGray;
	bulletTimerWhite.setFillColor(Color::Yellow);
	missileTimerWhite.setFillColor(Color::Blue);
	rocketTimerWhite.setFillColor(Color::Red);
	bulletTimerGray.setFillColor(Color::White);
	missileTimerGray.setFillColor(Color::White);
	rocketTimerGray.setFillColor(Color::White);
	int barWidth = 120;
	float bulletTimerWhiteWidth = bulletReloadTimer * barWidth / 5;
	float missileTimerWhiteWidth = missileReloadTimer * barWidth / 180;
	float rocketTimerWhiteWidth = rocketReloadTimer * barWidth / 180;
	bulletTimerWhite.setSize(Vector2f(bulletTimerWhiteWidth, 5));
	bulletTimerGray.setSize(Vector2f(barWidth - bulletTimerWhiteWidth, 5));
	missileTimerWhite.setSize(Vector2f(missileTimerWhiteWidth, 5));
	missileTimerGray.setSize(Vector2f(barWidth - missileTimerWhiteWidth, 5));
	rocketTimerWhite.setSize(Vector2f(rocketTimerWhiteWidth, 5));
	rocketTimerGray.setSize(Vector2f(barWidth - rocketTimerWhiteWidth, 5));
	bulletTimerWhite.setPosition(30, 20);
	missileTimerWhite.setPosition(30, 40);
	rocketTimerWhite.setPosition(30, 60);
	bulletTimerGray.setPosition(bulletTimerWhite.getPosition().x + bulletTimerWhite.getGlobalBounds().width, bulletTimerWhite.getPosition().y);
	missileTimerGray.setPosition(missileTimerWhite.getPosition().x + missileTimerWhite.getGlobalBounds().width, missileTimerWhite.getPosition().y);
	rocketTimerGray.setPosition(rocketTimerWhite.getPosition().x + rocketTimerWhite.getGlobalBounds().width, rocketTimerWhite.getPosition().y);
	window.draw(bulletTimerWhite);
	window.draw(missileTimerWhite);
	window.draw(rocketTimerWhite);
	window.draw(bulletTimerGray);
	window.draw(missileTimerGray);
	window.draw(rocketTimerGray);
}

void Ux_Ui::waveBar(int wave, int waves) {
	if (!font.loadFromFile("resource/Montserrat-VariableFont_wght.ttf")) {}
	scoreText.setString("Score : " + to_string(score));
	RectangleShape waveBarWhite;
	RectangleShape waveBarGray;
	waveBarGray.setFillColor(Color(88, 94, 89));
	waveBarWhite.setFillColor(Color::White);
	int barWidth = 300;
	float waveBarWhiteWidth = wave * barWidth / waves;
	waveBarWhite.setSize(Vector2f(waveBarWhiteWidth, 5));
	waveBarGray.setSize(Vector2f(barWidth - waveBarWhiteWidth, 5));
	waveBarWhite.setPosition(1000, 20);
	waveBarGray.setPosition(waveBarWhite.getPosition().x + waveBarWhite.getGlobalBounds().width, waveBarWhite.getPosition().y);
	window.draw(waveBarWhite);
	window.draw(waveBarGray);
	window.draw(scoreText);
}

void Ux_Ui::pauseScreen() {
	Font font;
	window.clear();
	rect.setSize(Vector2f(window.getSize()));
	rect.setFillColor(Color::Blue);
	rect.setPosition(0, 0);
	if (!font.loadFromFile("resource/Montserrat-VariableFont_wght.ttf")) {}
	pauseText.setFont(font);
	pauseText.setStyle(Text::Bold);
	pauseText.setFillColor(Color::White);
	pauseText.setCharacterSize(50);
	FloatRect shapeBounds = pauseText.getLocalBounds();
	Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
	pauseText.setOrigin(shapeCenter);
	pauseText.setPosition(width / 2, height / 2);
	pauseText.setString("Press ESC to Continue\nPress Q to Exit");
	window.draw(rect);
	window.draw(pauseText);
}

void Ux_Ui::clearPauseScreen() {
	window.clear();
	rect.setFillColor(Color::Transparent);
	deathText.setFillColor(Color::Transparent);
	rect.setSize(Vector2f(0, 0));
	window.draw(rect);
	window.display();
}