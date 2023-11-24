#pragma once
#include "Data.h"
#include <iostream>
#include <SFML/Audio.hpp>
#include "SoundsStruct.h"
using namespace std;
using namespace sf;
class Game
{
public:
	Game();
	void run(int width, int height, bool *flagGameStart);
	string *userName;
	float findPositionLeftSound(int tankPos, int winSize);
	float findPositionRightSound(int tankPos, int winSize);
	SoundsStruct sounds;
	SoundBuffer bufferGun;
	SoundBuffer bufferMissile;
	SoundBuffer bufferRocket;
	SoundBuffer bufferExplosion;
	SoundBuffer bufferExplosion2;
};

