#pragma once
#include <SFML/Graphics.hpp>
using namespace std;
using namespace sf;

class Map
{
public:
    Map(int modeWidth, int modeHeight, RenderWindow& win);
    void render();
    void createBackground(int boxsX, int boxsY);
    vector<RectangleShape> backgroundRect;
private:
    int winWidth, winHeight;
    double box_Xposition, box_Yposition;
    RenderWindow& window;
};