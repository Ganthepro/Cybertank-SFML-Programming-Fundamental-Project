#include "tankNmove.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <iomanip>
#include <sstream>

using namespace sf;
using namespace std;

tankNmove::tankNmove(RenderWindow& win, ammoNdamage& Ammo, vector<RectangleShape> backgroundRect) : window(win), ammo(Ammo) {
    bg = backgroundRect;
    pngTank.loadFromFile("resource/tank.png");
}

void tankNmove::createTank() {
    tankPos[0] = 1400.f / 2.f;
    tankPos[1] = 1050.f / 2.f;
    sprite = Sprite(pngTank);
    sprite.setScale(90.f / pngTank.getSize().y, 48.f / pngTank.getSize().x);
    FloatRect shapeBounds = sprite.getLocalBounds();
    Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
    sprite.setOrigin(shapeCenter);
    sprite.setRotation(90.f);
}

void tankNmove::draw(bool shoot, string targetID) {
    sprite.setPosition(tankPos[0], tankPos[1]);
    if (!this->checkRotateCollusion(bg)) {
        float additionRotate = 0.f;
        if (mode == "battle") {
            count = 0;
            float rotateSpeed = 360;
            if (int(rotate) != fRotate) {
                if (int(rotate) >= 0 && rotate < 90) {
                    if (fRotate >= 180) {
                        fRotate += rotateSpeed / 60;
                        if (fRotate >= 360.f) {
                            fRotate = 0.f;
                        }
                    }
                    else if (int(rotate) > fRotate) {
                        fRotate += rotateSpeed / 60;
                    }
                    else {
                        fRotate -= rotateSpeed / 60;
                    }
                }
                else if (int(rotate) >= 90 && int(rotate) < 180) {
                    if (rotate > fRotate) {
                        fRotate += rotateSpeed / 60;
                    }
                    else {
                        fRotate -= rotateSpeed / 60;
                    }
                }
                else if (int(rotate) >= 180 && int(rotate) < 270) {
                    if (int(rotate) > fRotate) {
                        fRotate += rotateSpeed / 60;
                    }
                    else {
                        fRotate -= rotateSpeed / 60;
                    }
                }
                else if (int(rotate) >= 270 && int(rotate) < 360) {
                    if (fRotate <= 180) {
                        fRotate -= rotateSpeed / 60;
                        if (fRotate <= 0.f) {
                            fRotate = 360.f;
                        }
                    }
                    else if (int(rotate) > fRotate) {
                        fRotate += rotateSpeed / 60;
                    }
                    else {
                        fRotate -= rotateSpeed / 60;
                    }
                }
            }
            if ((rotate - fRotate < 360 / 60 && rotate - fRotate > 0) || (fRotate - rotate < 360 / 60 && fRotate - rotate > 0)) 
                additionRotate = rotate - fRotate;
            if (collusion == true && count1 == 0)
                count1++;
            else if (collusion == false && count1 == 1)
                count1--;
            sprite.setRotation(-(fRotate + additionRotate));
            if (shoot)
                this->shootMissile(sprite, targetID);
        }
        else {
            if (count != 1) {
                rotate = fRotate + additionRotate;
                count++;
            }
            if (collusion == true && count1 == 0)
            {
                rotate = angle_of_rotate - rotate;
                count1++;
            }
            else if (collusion == false && count1 == 1)
                count1--;
            sprite.setRotation(-rotate);
        }
    }
    window.draw(shape);
    window.draw(sprite);
}

bool tankNmove::checkRotateCollusion(vector<RectangleShape> backgroundRect) {
    auto it_bg = backgroundRect.begin();
    while (it_bg != backgroundRect.end()) {
        if (it_bg->getGlobalBounds().intersects(sprite.getGlobalBounds()))
            return true;
        it_bg++;
    }
    return false;
}

void tankNmove::shootMissile(Sprite sprite, string targetID) {
    ammo.createMissile(sprite.getRotation(), sprite.getPosition(), targetID, "player");
}

void tankNmove::resetTank() {
    tankPos[0] = 1400.f / 2.f;
    tankPos[1] = 1050.f / 2.f;
    rotate = 0.f;
    spaceRotate = 0.f;
    fRotate = 0;
    flagCollusion = false;
    hp = 130;
}

void tankNmove::battleModeControlKeyboardMove(float mouseXpos, float mouseYpos, vector<RectangleShape> backgroundRect) {
    int w, a, s, d;
    w = 0; a = 1; s = 2; d = 3;
    auto it_bg = backgroundRect.begin();
    bool flag = false;
    shape.setRadius(50.f);
    shape.setFillColor(Color::Transparent);
    Vector2f spriteSize = Vector2f(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height);
    Vector2f spriteMiddlePosition = sprite.getPosition();
    shape.setOrigin(shape.getRadius(), shape.getRadius());
    shape.setPosition(spriteMiddlePosition);
    while (it_bg != backgroundRect.end()) {
        if (sprite.getGlobalBounds().intersects(it_bg->getGlobalBounds())) {
            flag = true;
            flagSpeed = true;
            if (speed > 0) {
                if (flagDirections[w] == false && flagDirections[s] == false && flagDirections[a] == false && flagDirections[d] == false) {
                    if (direction[w])
                        flagDirections[s] = true;
                    if (direction[s])
                        flagDirections[w] = true;
                    if (direction[a])
                        flagDirections[d] = true;
                    if (direction[d])
                        flagDirections[a] = true;
                    if (direction[w]) {
                        direction[w] = false; direction[s] = true;
                    }
                    else if (direction[s]) {
                        direction[s] = false; direction[w] = true;
                    }
                    if (direction[a]) {
                        direction[a] = false; direction[d] = true;
                    }
                    else if (direction[d]) {
                        direction[d] = false; direction[a] = true;
                    }
                }
            }
            break;
        }
        if (shape.getGlobalBounds().intersects(it_bg->getGlobalBounds())) {
            flagDirections[w] = false; flagDirections[s] = false; flagDirections[a] = false; flagDirections[d] = false;
            if (shape.getPosition().x < it_bg->getPosition().x) 
                tankPos[0] -= 5.f;
            else if (shape.getPosition().x > it_bg->getGlobalBounds().width) 
                tankPos[0] += 5.f;
            if (shape.getPosition().y < it_bg->getPosition().y) 
                tankPos[1] -= 5.f;
            else if (shape.getPosition().y > it_bg->getGlobalBounds().height) 
                tankPos[1] += 5.f;
            speed = 0;
            break;
        }
        it_bg++;
    }
    if (!flag) {
        flagDirections[w] = false; flagDirections[s] = false; flagDirections[a] = false; flagDirections[d] = false;
        if (direction[w]) 
            tankPos[1] -= speed / 60;
        if (direction[s])
            tankPos[1] += speed / 60;
        if (direction[a])
            tankPos[0] -= speed / 60;
        if (direction[d])
            tankPos[0] += speed / 60;
    }
    else if (flagSpeed) {
        if (flagDirections[w]) {
            float radians = -(sprite.getRotation()) * 3.14 / 180.0;
            tankPos[0] += cos(radians) * speed / 60;
            tankPos[1] += -(sin(radians) * speed / 60);
        } if (flagDirections[s]) {
            float radians = -(sprite.getRotation()) * 3.14 / 180.0;
            tankPos[0] += -(cos(radians) * speed / 60);
            tankPos[1] += sin(radians) * speed / 60;
        } if (flagDirections[a]) {
            float radians = (-(sprite.getRotation()) + 90) * 3.14 / 180.0;
            tankPos[0] += cos(radians) * speed / 60;
            tankPos[1] += -(sin(radians) * speed / 60);
        } if (flagDirections[d]) {
            float radians = (-(sprite.getRotation()) - 90) * 3.14 / 180.0;
            tankPos[0] += cos(radians) * speed / 60;
            tankPos[1] += -(sin(radians) * speed / 60);
        }
        if (speed <= 0)
            flagSpeed = false;
    }
}

void tankNmove::driveModeControlKeyboardMove(vector<RectangleShape> backgroundRect) {
    int w, a, s, d;
    w = 0; a = 1; s = 2; d = 3;
    float rotateSpeed = 120.f;
    float additionalSpeed = 0;
    auto it_bg = backgroundRect.begin();
    while (it_bg != backgroundRect.end()) {
        if (sprite.getGlobalBounds().intersects(it_bg->getGlobalBounds())) {
            if (speed > 0) {
                additionalSpeed = 0.f;
                if (direction[a])
                    direction[a] = false;
                if (direction[d])
                    direction[d] = false;
                if (direction[w] && flagDirection != "s") {
                    direction[w] = false; direction[s] = true;
                    flagDirection = "s";
                }
                else if (direction[s] && flagDirection != "w") {
                    direction[s] = false; direction[w] = true;
                    flagDirection = "w";
                }
                break;
            }
        }
        it_bg++;
    }
    if (speed <= 0) {
        direction[w] = false;
        direction[s] = false;
        flagDirection = "";
    }
    if (flagDirection == "") {
        if (direction[w]) {
            float radians = rotate * 3.14 / 180.0;
            tankPos[0] += cos(radians) * ((speed) / 60 + additionalSpeed);
            tankPos[1] += -(sin(radians) * ((speed) / 60) + additionalSpeed);
        }
        if (direction[s]) {
            float radians = rotate * 3.14 / 180.0;
            tankPos[0] += -(cos(radians) * ((speed) / 60) + additionalSpeed);
            tankPos[1] += sin(radians) * ((speed) / 60 + additionalSpeed);
        }
    }
    else {
        if (flagDirection == "w") {
            float radians = rotate * 3.14 / 180.0;
            tankPos[0] += cos(radians) * ((speed) / 60 + additionalSpeed);
            tankPos[1] += -(sin(radians) * ((speed) / 60) + additionalSpeed);
        }
        if (flagDirection == "s") {
            float radians = rotate * 3.14 / 180.0;
            tankPos[0] += -(cos(radians) * ((speed) / 60) + additionalSpeed);
            tankPos[1] += sin(radians) * ((speed) / 60 + additionalSpeed);
        }
    }
    if (direction[a]) {
        rotate += rotateSpeed / 60;
        if (rotate > 360) {
            rotate = 0.f;
        }
        fRotate = rotate;
    }
    if (direction[d]) {
        rotate -= rotateSpeed / 60;
        if (rotate < 0) {
            rotate = 360.f;
        }
        fRotate = rotate;
    }
}

void tankNmove::battleModeControlMouseMove(float mouseXpos, float mouseYpos) {
    VertexArray line(Lines, 2);
    line[0].position = Vector2f(tankPos[0], tankPos[1]);
    line[0].color = Color::Red;
    if (mouseXpos > tankPos[0] && mouseYpos < tankPos[1]) {
        float opp = tankPos[1] - mouseYpos;
        float adj = mouseXpos - tankPos[0];
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        rotate = asin(opp / hyp) * (180.f / 3.14);
    }
    else if (mouseXpos < tankPos[0] && mouseYpos < tankPos[1]) {
        float opp = tankPos[1] - mouseYpos;
        float adj = tankPos[0] - mouseXpos;
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        rotate = (90.f - (asin(opp / hyp) * (180.f / 3.14))) + 90.f;
    }
    else if (mouseXpos < tankPos[0] && mouseYpos > tankPos[1]) {
        float opp = mouseYpos - tankPos[1];
        float adj = tankPos[0] - mouseXpos;
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        rotate = asin(opp / hyp) * (180.f / 3.14) + 180.f;
    }
    else if (tankPos[0] < mouseXpos && mouseYpos > tankPos[1]) {
        float opp = mouseYpos - tankPos[1];
        float adj = mouseXpos - tankPos[0];
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        rotate = (180.f - (asin(opp / hyp) * (180.f / 3.14))) + 180.f;
    }
}

void tankNmove::raycasting(float angle, vector<RectangleShape> backgroundRect) {
    constexpr float PI = 3.14159f;
    const float radians = angle * PI / 180.0f;
    VertexArray line(Lines, 2);
    line[0].position = Vector2f(tankPos[0], tankPos[1]);
    line[0].color = Color::Red;
    float distance = 0;
    bool hitObstacle = false;
    while (!hitObstacle) {
        for (const auto& obstacle : backgroundRect) {
            if (obstacle.getGlobalBounds().contains(Vector2f(tankPos[0] + (cos(radians) * distance),
                tankPos[1] + (-sin(radians) * distance)))) {
                line[1].position = Vector2f(tankPos[0] + (cos(radians) * (distance - 5)),
                    tankPos[1] + (-sin(radians) * (distance - 5)));
                hitObstacle = true;
                break;
            }
        }
        distance += 10;
    }
    line[1].color = Color::Red;
    window.draw(line);
}

float tankNmove::getRotate() {
    return sprite.getRotation();
}

Vector2f tankNmove::getPos() {
    return sprite.getPosition();
}

FloatRect tankNmove::getBox() {
    return sprite.getGlobalBounds();
}