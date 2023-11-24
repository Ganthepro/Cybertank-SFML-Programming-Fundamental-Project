#include "ammoNdamage.h"
#include <iostream>
#include <SFML/Audio.hpp>
using namespace std;
using namespace sf;

ammoNdamage::ammoNdamage(RenderWindow& win) : window(win) {
    pngRocket.loadFromFile("resource/rocket.png");
    pngBullet.loadFromFile("resource/bullet.png");
    pngMissile.loadFromFile("resource/missile.png");
    if (!game.bufferGun.loadFromFile("resource/gun.wav"))
        return;
    if (!game.bufferMissile.loadFromFile("resource/missile.wav"))
        return;
    if (!game.bufferRocket.loadFromFile("resource/rocket.wav"))
        return;
}

void ammoNdamage::createRocket(float rotate, Vector2f position, string status) {
    game.sounds.soundRocket_L.setBuffer(game.bufferRocket);
    game.sounds.soundRocket_R.setBuffer(game.bufferRocket);
    sprite = Sprite(pngRocket);
    sprite.setScale(50.f / pngRocket.getSize().x, 20.f / pngRocket.getSize().y);
    FloatRect shapeBounds = sprite.getLocalBounds();
    Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
    sprite.setRotation(rotate);
    sprite.setPosition(position);
    rocket.push_back(sprite);
    rocket_id.push_back(status);
    rocketSounds.emplace_back(game.sounds.soundRocket_L, game.sounds.soundRocket_R);
    game.sounds.soundRocket_L.setPosition(game.findPositionLeftSound(position.x, window.getSize().x), 0, 0);
    game.sounds.soundRocket_L.play();
    game.sounds.soundRocket_R.setPosition(game.findPositionRightSound(position.x, window.getSize().x), 0, 0);
    game.sounds.soundRocket_R.play();
}

void ammoNdamage::createBullet(float rotate, Vector2f position) {
    game.sounds.soundGun_L.setBuffer(game.bufferGun);
    game.sounds.soundGun_R.setBuffer(game.bufferGun);
    sprite = Sprite(pngBullet);
    sprite.setScale(30.f / pngBullet.getSize().x, 5.f / pngRocket.getSize().y);
    FloatRect shapeBounds = sprite.getLocalBounds();
    Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
    sprite.setRotation(rotate + (rand() % 6 - 2.5));
    sprite.setPosition(position);
    bullet.push_back(sprite);
    game.sounds.soundGun_L.setPosition(game.findPositionLeftSound(position.x, window.getSize().x), 0, 0);
    game.sounds.soundGun_L.play();
    game.sounds.soundGun_R.setPosition(game.findPositionRightSound(position.x, window.getSize().x), 0, 0);
    game.sounds.soundGun_R.play();
}

void ammoNdamage::createMissile(float rotate, Vector2f position, string targetID, string status) {
    game.sounds.soundMissile_L.setBuffer(game.bufferMissile);
    game.sounds.soundMissile_R.setBuffer(game.bufferMissile);
    sprite = Sprite(pngMissile);
    missileFrotate.push_back(360 - rotate);
    sprite.setScale(50.f / pngMissile.getSize().x, 20.f / pngMissile.getSize().y);
    FloatRect shapeBounds = sprite.getLocalBounds();
    Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
    sprite.setRotation(rotate);
    sprite.setPosition(position);
    missile.push_back(sprite);
    missile_id.push_back(status);
    missileTarget.push_back(targetID);
    game.sounds.soundMissile_L.setPosition(game.findPositionLeftSound(position.x, window.getSize().x), 0, 0);
    game.sounds.soundMissile_L.play();
    game.sounds.soundMissile_R.setPosition(game.findPositionRightSound(position.x, window.getSize().x), 0, 0);
    game.sounds.soundMissile_R.play();
}

void ammoNdamage::battleModeShoot1Keyboard(vector<string> targetID, vector<Vector2f> targetPos, Vector2f playerTargetPos) {
    int missileSpeed = 500;
    float rotateSpeed = 200;
    auto it = missile.begin();
    auto it_target = missileTarget.begin();
    auto it_id = missile_id.begin();
    i = 0;
    while (it != missile.end())
    {
        float targetXpos = 0;
        float targetYpos = 0;
        if (*it_target == "player") {
            targetXpos = playerTargetPos.x;
            targetYpos = playerTargetPos.y;
        }
        else {
            auto it_enamyID = targetID.begin();
            auto it_enamyPos = targetPos.begin();
            while (it_enamyID != targetID.end()) {
                if (*it_enamyID == *it_target) {
                    targetXpos = it_enamyPos->x;
                    targetYpos = it_enamyPos->y;
                    break;
                }
                it_enamyID++;
                it_enamyPos++;
            }
        }
        float rotate = 0;
        window.draw(*it);
        if (targetXpos > it->getPosition().x && targetYpos < it->getPosition().y) {
            float opp = it->getPosition().y - targetYpos;
            float adj = targetXpos - it->getPosition().x;
            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
            rotate = asin(opp / hyp) * (180.f / 3.14);
        }
        else if (targetXpos < it->getPosition().x && targetYpos < it->getPosition().y) {
            float opp = it->getPosition().y - targetYpos;
            float adj = it->getPosition().x - targetXpos;
            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
            rotate = (90.f - (asin(opp / hyp) * (180.f / 3.14))) + 90.f;
        }
        else if (targetXpos < it->getPosition().x && targetYpos > it->getPosition().y) {
            float opp = targetYpos - it->getPosition().y;
            float adj = it->getPosition().x - targetXpos;
            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
            rotate = asin(opp / hyp) * (180.f / 3.14) + 180.f;
        }
        else if (it->getPosition().x < targetXpos && targetYpos > it->getPosition().y) {
            float opp = targetYpos - it->getPosition().y;
            float adj = targetXpos - it->getPosition().x;
            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
            rotate = (180.f - (asin(opp / hyp) * (180.f / 3.14))) + 180.f;
        }
        float additionRotate = 0.f;
        if (int(rotate) != missileFrotate[i]) {
            if (int(rotate) >= 0 && rotate < 90) {
                if (missileFrotate[i] >= 180) {
                    missileFrotate[i] += rotateSpeed / 60;
                    if (missileFrotate[i] >= 360.f) 
                        missileFrotate[i] = 0.f;
                }
                else if (int(rotate) > missileFrotate[i]) 
                    missileFrotate[i] += rotateSpeed / 60;
                else 
                    missileFrotate[i] -= rotateSpeed / 60;
            }
            else if (int(rotate) >= 90 && int(rotate) < 180) {
                if (rotate > missileFrotate[i]) 
                    missileFrotate[i] += rotateSpeed / 60;
                else 
                    missileFrotate[i] -= rotateSpeed / 60;
            }
            else if (int(rotate) >= 180 && int(rotate) < 270) {
                if (int(rotate) > missileFrotate[i]) 
                    missileFrotate[i] += rotateSpeed / 60;
                else 
                    missileFrotate[i] -= rotateSpeed / 60;
            }
            else if (int(rotate) >= 270 && int(rotate) < 360) {
                if (missileFrotate[i] <= 180) {
                    missileFrotate[i] -= rotateSpeed / 60;
                    if (missileFrotate[i] <= 0.f) 
                        missileFrotate[i] = 360.f;
                }
                else if (int(rotate) > missileFrotate[i]) 
                    missileFrotate[i] += rotateSpeed / 60;
                else 
                    missileFrotate[i] -= rotateSpeed / 60;
            }
        }
        if ((rotate - missileFrotate[i] < rotateSpeed / 60 && rotate - missileFrotate[i] > 0) || (missileFrotate[i] - rotate < rotateSpeed / 60 && missileFrotate[i] - rotate > 0))
            additionRotate = rotate - missileFrotate[i];
        it->setRotation(-(missileFrotate[i] + additionRotate));
        float radians = (missileFrotate[i] + additionRotate) * 3.14 / 180.0;
        Vector2f nextMove = Vector2f(cos(radians) * missileSpeed / 60, -(sin(radians) * missileSpeed / 60));
        it->move(nextMove);
        if (it->getPosition().x > window.getSize().x || it->getPosition().x < 0
            || it->getPosition().y > window.getSize().y || it->getPosition().y < 0) {
            missileFrotate.erase(missileFrotate.begin() + i);
            it = missile.erase(it);
            it_target = missileTarget.erase(it_target);
            it_id = missile_id.erase(it_id);
            i--;
        }
        else
        {
            it++;
            i++;
            it_target++;
            it_id++;
        }
    }
}

void ammoNdamage::battleModeShootLeftMouse() {
    auto it = bullet.begin();
    while (it != bullet.end())
    {
        window.draw(*it);
        float radians = -(it->getRotation()) * 3.14 / 180.0;
        it->move(cos(radians) * bulletSpeed / 60, -(sin(radians) * bulletSpeed / 60));
        if (it->getPosition().x > window.getSize().x || it->getPosition().x < 0
            || it->getPosition().y > window.getSize().y || it->getPosition().y < 0) {
            it = bullet.erase(it);
        }
        else 
            it++;
    }
}

void ammoNdamage::battleModeShootMiddleMouse() {
    auto it = rocket.begin();
    auto it_rocket_id = rocket_id.begin();
    while (it != rocket.end())
    {
        window.draw(*it);
        float radians = -(it->getRotation()) * 3.14 / 180.0;
        it->move(cos(radians) * rocketSpeed / 60, -(sin(radians) * rocketSpeed / 60));
        if (it->getPosition().x > window.getSize().x || it->getPosition().x < 0
            || it->getPosition().y > window.getSize().y || it->getPosition().y < 0) {
            it = rocket.erase(it);
            it_rocket_id = rocket_id.erase(it_rocket_id);
        }
        else {
            it++;
            it_rocket_id++;
        }
    }
}