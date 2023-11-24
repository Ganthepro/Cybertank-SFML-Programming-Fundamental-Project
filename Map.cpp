#include "Map.h"

Map::Map(int modeWidth, int modeHeight, RenderWindow& win) : window(win) {
    winWidth = modeWidth;
    winHeight = modeHeight;
}

void Map::render() {
    auto it = backgroundRect.begin();
    while (it != backgroundRect.end()) {
        window.draw(*it);
        it++;
    }
}

void Map::createBackground(int boxsX, int boxsY) {
    RectangleShape rect;
    double boxWidth = winWidth / boxsX;
    double boxHeight = winHeight / boxsY;
    box_Xposition = 4.f;
    box_Yposition = 3.f;
    for (int y = 0; y < boxsY; y++) {
        for (int x = 0; x < boxsX; x++) {
            rect.setSize(Vector2f(boxWidth, boxHeight));
            if (box_Xposition == 4.f || box_Yposition == 3.f || x == boxsX - 1 || y == boxsY - 1) {
                Color outlineColor(255, 0, 0, 125);
                rect.setOutlineColor(outlineColor);
                rect.setFillColor(Color::Transparent);
                rect.setOutlineThickness(1.f);
                rect.setPosition(box_Xposition, box_Yposition);
                backgroundRect.push_back(rect);
            }
            if ((x == 2 || x == 3 || x == 8 || x == 9) && (y == 2 || y == 3 || y == 8 || y == 9)) {
                Color outlineColor(0, 0, 255, 125);
                rect.setOutlineColor(outlineColor);
                rect.setFillColor(Color::Transparent);
                rect.setOutlineThickness(1.f);
                rect.setPosition(box_Xposition, box_Yposition);
                backgroundRect.push_back(rect);
            }
            box_Xposition += boxWidth;
        }
        box_Yposition += boxHeight;
        box_Xposition = 4.f;
    }
}