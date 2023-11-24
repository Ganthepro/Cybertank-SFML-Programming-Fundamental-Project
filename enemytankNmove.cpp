#include "enemytankNmove.h"
#include <array>

enemytankNmove::enemytankNmove(RenderWindow& win, ammoNdamage& Ammo, vector<RectangleShape> backgroundRect) : window(win),ammo(Ammo) {
    pngTank.loadFromFile("resource/tank_enemy.png");
    pngBoss.loadFromFile("resource/boss.png");
    sprite = Sprite(pngTank);
    boss_sprite = Sprite(pngBoss);
    sprite.setScale(90.f / pngTank.getSize().y, 48.f / pngTank.getSize().x);
    boss_sprite.setScale(90.f * 1.5 / pngBoss.getSize().y, 60.f * 1.5 / pngBoss.getSize().x);
    FloatRect shapeBounds = sprite.getLocalBounds();
    FloatRect shapeBoundsBoss = boss_sprite.getLocalBounds();
    Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
    Vector2f shapeCenterBoss(shapeBoundsBoss.left + shapeBoundsBoss.width / 2.f, shapeBoundsBoss.top + shapeBoundsBoss.height / 2.f);
    sprite.setOrigin(shapeCenter);
    boss_sprite.setOrigin(shapeCenterBoss);
    sprite.setRotation(90.f);
    boss_sprite.setRotation(90.f);
    bg = backgroundRect;
    if (!game.bufferExplosion2.loadFromFile("resource/explosion2.wav"))
        return;
}

void enemytankNmove::findPos(vector<RectangleShape> it_bg, FloatRect tankBox) {
    while (true) {
        auto it = it_bg.begin();
        bool flag = false;
        int x = rand() % window.getSize().x / 1.2 + 200;
        int y = rand() % window.getSize().y / 2 + 200;
        while (it != it_bg.end()) {
            if ((it->getGlobalBounds().contains(x - 45, y - 24) || it->getGlobalBounds().contains(x + 45, y - 24)
                || it->getGlobalBounds().contains(x - 45, y + 24) || it->getGlobalBounds().contains(x + 45, y + 24))
                || (tankBox.contains(x - 45, y - 24) || tankBox.contains(x + 45, y - 24))
                || tankBox.contains(x - 45, y + 24) || tankBox.contains(x + 45, y + 24))
                flag = true;
            it++;
        }
        if (!flag) {
            enamyTankPos.push_back(Vector2f(x, y));
            break;
        }
    }
}

void enemytankNmove::createEnemyTank(vector<RectangleShape> it_bg, FloatRect tankBox) {
    this->findPos(it_bg, tankBox);
    enamyTankSprite.push_back(sprite);
    enamyTankRotate.push_back(0);
    if (count % 2 == 0)
        enamyTankMode.push_back("missile");
    else
        enamyTankMode.push_back("rocket");
    enamyTankReloadTimer.push_back(0);
    enamyTankFrotate.push_back(0);
    enamyTankDoMove.push_back(true);
    enamyTankID.push_back("enamy" + to_string(count));
    enamyTankHP.push_back(100);
    enamyTankMaxHP.push_back(100);
    enamyTankDistance_to_Wall.push_back({0,0,0,0});
    enamyTankStuntTimer.push_back(0);
    enamyTankCharmTimer.push_back(0);
    enamyTankCharmTarget.push_back(-1);
    enamyTankShootable.push_back(false);
    count++;
}

void enemytankNmove::createEnemyBoss(vector<RectangleShape> it_bg, FloatRect tankBox) {
    this->findPos(it_bg, tankBox);
    enamyTankSprite.push_back(boss_sprite);
    enamyTankRotate.push_back(0);
    enamyTankMode.push_back("boss");
    enamyTankReloadTimer.push_back(0);
    enamyTankFrotate.push_back(0);
    enamyTankDoMove.push_back(true);
    enamyTankID.push_back("enamy" + to_string(count));
    enamyTankHP.push_back(500);
    enamyTankMaxHP.push_back(500);
    enamyTankDistance_to_Wall.push_back({ 0,0,0,0 });
    enamyTankStuntTimer.push_back(0);
    enamyTankCharmTimer.push_back(0);
    enamyTankCharmTarget.push_back(-1);
    enamyTankShootable.push_back(false);
    count++;
}

void enemytankNmove::findCharmTarget() {
    auto it_sprite = enamyTankSprite.begin();
    auto it_charm = enamyTankCharmTimer.begin();
    auto it_charmTarget = enamyTankCharmTarget.begin();
    while (it_charm != enamyTankCharmTimer.end()) {
        if (*it_charm > 0) {
            auto it_sprite2 = enamyTankSprite.begin();
            float distance = NULL;
            int closest_i = 0;
            int i = 0;
            while (it_sprite2 != enamyTankSprite.end()) {
                if (it_sprite2 != it_sprite) {
                    float opp = it_sprite2->getPosition().y - it_sprite->getPosition().y;
                    float adj = it_sprite2->getPosition().x - it_sprite->getPosition().x;
                    float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
                    if (hyp < distance || distance == NULL) {
                        distance = hyp;
                        closest_i = i;
                    }
                }
                it_sprite2++;
                i++;
            }
            *it_charmTarget = closest_i;
        }
        it_sprite++;
        it_charm++;
        it_charmTarget++;
    }
}

void enemytankNmove::draw(FloatRect tankBox, Vector2f playerTankPos) {
    this->findCharmTarget();
    auto it = enamyTankPos.begin();
    auto it_sprite = enamyTankSprite.begin();
    auto it_rotate = enamyTankRotate.begin();
    auto it_fRotate = enamyTankFrotate.begin();
    auto it_mode = enamyTankMode.begin();
    auto it_timer = enamyTankReloadTimer.begin();
    auto it_move = enamyTankDoMove.begin();
    auto it_id = enamyTankID.begin();
    auto it_hp = enamyTankHP.begin();
    auto it_Distance_to_Wall = enamyTankDistance_to_Wall.begin();
    auto it_stunt = enamyTankStuntTimer.begin();
    auto it_charm = enamyTankCharmTimer.begin();
    auto it_charmTarget = enamyTankCharmTarget.begin();
    auto it_shootable = enamyTankShootable.begin();
    float rotateSpeed = 90;
    while (it_sprite != enamyTankSprite.end()) {
        if (*it_charm > 0)
            *it_charm -= 1;
        if (*it_stunt > 0) 
            *it_stunt -= 1;
        else {
            moveDirection = { false, false, false, false };
            it_sprite->setPosition(*it);
            if (*it_charm > 0 && *it_charmTarget != -1)
                *it_rotate = this->enemyControlMouseMove(*it, enamyTankPos[*it_charmTarget]);
            else 
                *it_rotate = this->enemyControlMouseMove(*it, playerTankPos);
            float additionRotate = 0.f;
            if (int(*it_rotate) != *it_fRotate && *it_move) {
                if (int(*it_rotate) >= 0 && *it_rotate < 90) {
                    if (*it_fRotate >= 180) {
                        *it_fRotate += rotateSpeed / 60;
                        if (*it_fRotate >= 360.f) {
                            *it_fRotate = 0.f;
                        }
                    }
                    else if (int(*it_rotate) > *it_fRotate) {
                        *it_fRotate += rotateSpeed / 60;
                    }
                    else {
                        *it_fRotate -= rotateSpeed / 60;
                    }
                }
                else if (int(*it_rotate) >= 90 && int(*it_rotate) < 180) {
                    if (*it_rotate > *it_fRotate) {
                        *it_fRotate += rotateSpeed / 60;
                    }
                    else {
                        *it_fRotate -= rotateSpeed / 60;
                    }
                }
                else if (int(*it_rotate) >= 180 && int(*it_rotate) < 270) {
                    if (int(*it_rotate) > *it_fRotate) {
                        *it_fRotate += rotateSpeed / 60;
                    }
                    else {
                        *it_fRotate -= rotateSpeed / 60;
                    }
                }
                else if (int(*it_rotate) >= 270 && int(*it_rotate) < 360) {
                    if (*it_fRotate <= 180) {
                        *it_fRotate -= rotateSpeed / 60;
                        if (*it_fRotate <= 0.f) {
                            *it_fRotate = 360.f;
                        }
                    }
                    else if (int(*it_rotate) > *it_fRotate) {
                        *it_fRotate += rotateSpeed / 60;
                    }
                    else {
                        *it_fRotate -= rotateSpeed / 60;
                    }
                }
                if ((*it_rotate - *it_fRotate < 120 / 60 && *it_rotate - *it_fRotate > 0) || (*it_fRotate - *it_rotate < 120 / 60 && *it_fRotate - *it_rotate > 0))
                    additionRotate = *it_rotate - *it_fRotate;
            }
            it_sprite->setRotation(-(*it_fRotate + additionRotate));
            for (int i = 0; i < 8; i++) {
                if (*it_charm > 0) 
                    this->raycasting(conner[i], bg, it_sprite->getPosition(), enamyTankSprite[*it_charmTarget].getGlobalBounds(), false, 50, *it_Distance_to_Wall, i);
                else 
                    this->raycasting(conner[i], bg, it_sprite->getPosition(), tankBox, false, 50, *it_Distance_to_Wall, i);
            }
            if ((*it_mode == "missile" && *it_timer == 360) || (*it_mode == "rocket" && *it_timer == 180) || (*it_mode == "boss" && *it_timer != 0 && *it_timer % 280 == 0))
                if ((*it_charm > 0 && this->raycasting(*it_fRotate + additionRotate, bg, it_sprite->getPosition(), enamyTankSprite[*it_charmTarget].getGlobalBounds(), !*it_move, 10, *it_Distance_to_Wall, 9)) 
                    || (*it_charm <= 0 && this->raycasting(*it_fRotate + additionRotate, bg, it_sprite->getPosition(), tankBox, !*it_move, 10, *it_Distance_to_Wall, 9)))
                {
                    *it_shootable = true;
                }
            if (*it_mode == "rocket" && *it_timer != 180)
                *it_timer += 1;
            else if (*it_mode == "missile" && *it_timer != 360)
                *it_timer += 1;
            if (((*it_mode == "rocket" && *it_timer == 180) || (*it_mode == "missile" && *it_timer == 360) || (*it_mode == "boss" && 280 <= *it_timer && *it_timer < 360)) && *it_shootable)
                *it_move = false;
            if (!*it_move && *it_mode != "boss") {
                if (*it_charm > 0) 
                    this->raycasting(*it_fRotate + additionRotate, bg, it_sprite->getPosition(), enamyTankSprite[*it_charmTarget].getGlobalBounds(), !*it_move, 10, *it_Distance_to_Wall, 9);
                else 
                    this->raycasting(*it_fRotate + additionRotate, bg, it_sprite->getPosition(), tankBox, !*it_move, 10, *it_Distance_to_Wall, 9);            
            }
            if (!*it_move && ((*it_mode == "rocket" && *it_timer == 300) || (*it_mode == "missile" && *it_timer == 480))) {
                if (*it_charm > 0)
                    this->enamyAIShoot(*it_mode, *it_sprite, *it_id, enamyTankID[*it_charmTarget], false);
                else
                    this->enamyAIShoot(*it_mode, *it_sprite, *it_id, "player", false);  
                *it_shootable = false;
                *it_move = true;
                *it_timer = 0;
            }
            else if (!*it_move && *it_mode != "boss")
                *it_timer += 1;
            if (*it_mode == "boss" && 280 <= *it_timer && *it_timer < 360) {
                this->raycasting(*it_fRotate + additionRotate, bg, it_sprite->getPosition(), tankBox, true, 10, *it_Distance_to_Wall, 9);
                this->raycasting(*it_fRotate + additionRotate - 15, bg, it_sprite->getPosition(), tankBox, true, 10, *it_Distance_to_Wall, 9);
                this->raycasting(*it_fRotate + additionRotate + 15, bg, it_sprite->getPosition(), tankBox, true, 10, *it_Distance_to_Wall, 9);
                *it_move = false;
            }
            else if (*it_mode == "boss")
                *it_move = true;
            if (*it_mode == "boss" && *it_timer == 360 && *it_shootable) {
                this->enamyAIShoot("rocket", *it_sprite, *it_id, "player",true);
                *it_shootable = false;
            }
            if (*it_mode == "boss" && *it_timer != 0 && *it_timer % 480 == 0) {
                this->enamyAIShoot("missile", *it_sprite, *it_id, "player", true);
                *it_timer = 0;
            }
            if (*it_mode == "boss") 
                *it_timer += 1;
            if (*it_move)
                if (*it_charm > 0)
                    *it = this->enamyAIMove(moveDirection, *it, enamyTankSprite, enamyTankPos, enamyTankSprite[*it_charmTarget].getPosition(), bg);
                else
                    *it = this->enamyAIMove(moveDirection, *it, enamyTankSprite, enamyTankPos, playerTankPos, bg);
        }
        window.draw(*it_sprite);
        it++;
        it_sprite++;
        it_rotate++;
        it_fRotate++;
        it_mode++;
        it_timer++;
        it_move++;
        it_id++;
        it_hp++;
        it_Distance_to_Wall++;
        it_stunt++;
        it_charm++;
        it_charmTarget++;
        it_shootable++;
    }
}

bool enemytankNmove::raycasting(float angle, vector<RectangleShape> backgroundRect, Vector2f Pos, FloatRect tankBox, bool showable, float d, array<float, 4> arr, int index) {
    constexpr float PI = 3.14159f;
    const float radians = angle * PI / 180.0f;
    VertexArray line(Lines, 2);
    line[0].position = Pos;
    float distance = 0;
    bool hitObstacle = false;
    bool hitTank = false;
    while (!hitObstacle) {
        for (const auto& obstacle : backgroundRect) {
            if (obstacle.getGlobalBounds().contains(Vector2f(Pos.x + (cos(radians) * distance),
                Pos.y + (-sin(radians) * distance))) || tankBox.contains(Vector2f(Pos.x + (cos(radians) * distance),
                    Pos.y + (-sin(radians) * distance)))) {
                line[1].position = Vector2f(Pos.x + (cos(radians) * (distance - 5)),
                    Pos.y + (-sin(radians) * (distance - 5)));
                float opp = line[1].position.y - line[0].position.y;
                float adj = line[0].position.x - line[1].position.x;
                float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
                if (hyp < 100 && index != 9) {
                    if (index == 0)
                        moveDirection[1] = true;
                    if (index == 1)
                        moveDirection[0] = true;
                    if (index == 2)
                        moveDirection[3] = true;
                    if (index == 3)
                        moveDirection[2] = true;
                    if (index == 4 && hyp < 83.5) {
                        moveDirection[1] = true;
                        moveDirection[2] = true;
                    }
                    if (index == 5 && hyp < 83.5) {
                        moveDirection[1] = true;
                        moveDirection[3] = true;
                    }
                    if (index == 6 && hyp < 83.5) {
                        moveDirection[0] = true;
                        moveDirection[3] = true;
                    }
                    if (index == 7 && hyp < 83.5) {
                        moveDirection[0] = true;
                        moveDirection[2] = true;
                    }
                }
                if (tankBox.contains(Vector2f(Pos.x + (cos(radians) * distance), Pos.y + (-sin(radians) * distance))))
                    hitTank = true;
                hitObstacle = true;
                break;
            }
        }
        distance += d;
        if (distance >= 100 && index != 9)
            break;
    }
    if (showable && hitTank) {
        line[0].color = Color::Red;
        line[1].color = Color::Red;
    }
    else if (showable && !hitTank) {
        line[0].color = Color::Blue;
        line[1].color = Color::Blue;
    }
    else {
        line[0].color = Color::Transparent;
        line[1].color = Color::Transparent;
    }
    window.draw(line);
    return hitTank;
}

Vector2f enemytankNmove::enamyAIMove(vector<bool> moveDirection, Vector2f it_pos, vector<Sprite> enamyTankSprite, vector<Vector2f> enamyTankpos, Vector2f playerTankPos, vector<RectangleShape> backgroundRect) {
    for (int i = 0; i < enamyTankSprite.size() - 1; i++) {
        for (int k = i + 1; k < enamyTankSprite.size(); k++) {
            float opp = enamyTankSprite[i].getPosition().y - enamyTankSprite[k].getPosition().y;
            float adj = enamyTankSprite[k].getPosition().x - enamyTankSprite[i].getPosition().x;
            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
            if (hyp < 100) {
                bool flagI = false;
                bool flagK = false;
                auto it_bg = backgroundRect.begin();
                while (it_bg != backgroundRect.end()) {
                    if (it_bg->getGlobalBounds().intersects(enamyTankSprite[i].getGlobalBounds()))
                        flagI = true;
                    if (it_bg->getGlobalBounds().intersects(enamyTankSprite[k].getGlobalBounds()))
                        flagK = true;
                    it_bg++;
                }
                if (enamyTankSprite[i].getPosition().y > enamyTankSprite[k].getPosition().y) {
                    if (!flagI)
                        enamyTankPos[i].y += 0.5;
                    if (!flagK)
                        enamyTankPos[k].y -= 0.5;
                }
                else {
                    if (!flagI)
                        enamyTankPos[i].y -= 0.5;
                    if (!flagK)
                        enamyTankPos[k].y += 0.5;
                }
                if (enamyTankSprite[i].getPosition().x > enamyTankSprite[k].getPosition().x) {
                    if (!flagI)
                        enamyTankPos[i].x += 0.5;
                    if (!flagK)
                        enamyTankPos[k].x -= 0.5;
                }
                else {
                    if (!flagI)
                        enamyTankPos[i].x -= 0.5;
                    if (!flagK)
                        enamyTankPos[k].x += 0.5;
                }
                VertexArray line(Lines, 2);
                line[0].position = enamyTankSprite[i].getPosition();
                line[1].position = enamyTankSprite[k].getPosition();
                line[0].color = Color::Yellow;
                line[1].color = Color::Yellow;
            }
        }
    }
    int up = 0; int down = 1; int left = 2; int right = 3;
    float opp = playerTankPos.y - it_pos.y;
    float adj = playerTankPos.x - it_pos.x;
    float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
    int mainDistance = 400;
    if (it_pos.x > playerTankPos.x && it_pos.y < playerTankPos.y && hyp > mainDistance) {
        moveDirection[down] = true;
        moveDirection[left] = true;
    }
    if (it_pos.x < playerTankPos.x && it_pos.y < playerTankPos.y && hyp > mainDistance) {
        moveDirection[down] = true;
        moveDirection[right] = true;
    }
    if (it_pos.x < playerTankPos.x && it_pos.y > playerTankPos.y && hyp > mainDistance) {
        moveDirection[up] = true;
        moveDirection[right] = true;
    }
    if (it_pos.x > playerTankPos.x && it_pos.y > playerTankPos.y && hyp > mainDistance) {
        moveDirection[up] = true;
        moveDirection[left] = true;
    }
    if (it_pos.x > playerTankPos.x && it_pos.y < playerTankPos.y && hyp < mainDistance) {
        moveDirection[up] = true;
        moveDirection[right] = true;
    }
    if (it_pos.x < playerTankPos.x && it_pos.y < playerTankPos.y && hyp < mainDistance) {
        moveDirection[up] = true;
        moveDirection[left] = true;
    }
    if (it_pos.x < playerTankPos.x && it_pos.y > playerTankPos.y && hyp < mainDistance) {
        moveDirection[down] = true;
        moveDirection[left] = true;
    }
    if (it_pos.x > playerTankPos.x && it_pos.y > playerTankPos.y && hyp < mainDistance) {
        moveDirection[down] = true;
        moveDirection[right] = true;
    }
    if (moveDirection[up])
        it_pos.y -= 1;
    if (moveDirection[down])
        it_pos.y += 1;
    if (moveDirection[left])
        it_pos.x -= 1;
    if (moveDirection[right])
        it_pos.x += 1;
    return Vector2f(it_pos.x, it_pos.y);
}

void enemytankNmove::Wave(FloatRect tankBox) {
    if (wave != waves && this->enamyTankSprite.size() == 0) {
        wave++;
        waveFlag = true;
    }
    else if (wave == waves) {
        waves++;
        wave = 1;
        this->createEnemyBoss(bg, tankBox);
    }
    if (this->enamyTankSprite.size() < wave && waveFlag)
        this->createEnemyTank(bg, tankBox);
    //if (waves == 5)
    //    this->createEnemyBoss(bg, tankBox);
    if (this->enamyTankSprite.size() == wave)
        waveFlag = false;
}

void enemytankNmove::destroyTank(string tankID) {
    auto it = enamyTankPos.begin();
    auto it_rotate = enamyTankRotate.begin();
    auto it_fRotate = enamyTankFrotate.begin();
    auto it_mode = enamyTankMode.begin();
    auto it_timer = enamyTankReloadTimer.begin();
    auto it_move = enamyTankDoMove.begin();
    auto it_id = enamyTankID.begin();
    auto it_dtw = enamyTankDistance_to_Wall.begin();
    auto it_stunt = enamyTankStuntTimer.begin();
    auto it_charm = enamyTankCharmTimer.begin();
    auto it_charmTarget = enamyTankCharmTarget.begin();
    auto it_shootable = enamyTankShootable.begin();
    auto it_maxHP = enamyTankMaxHP.begin();
    while (it != enamyTankPos.end()) {
        if (*it_id == tankID) {
            game.sounds.soundExplosion2_L.setBuffer(game.bufferExplosion2);
            game.sounds.soundExplosion2_R.setBuffer(game.bufferExplosion2);
            game.sounds.soundExplosion2_L.setPosition(game.findPositionLeftSound(it->x, window.getSize().x), 0, 0);
            game.sounds.soundExplosion2_L.setVolume(45.f);
            game.sounds.soundExplosion2_L.play();
            game.sounds.soundExplosion2_R.setPosition(game.findPositionRightSound(it->x, window.getSize().x), 0, 0);
            game.sounds.soundExplosion2_R.setVolume(45.f);
            game.sounds.soundExplosion2_R.play();
            enamyTankPos.erase(it);
            enamyTankRotate.erase(it_rotate);
            enamyTankFrotate.erase(it_fRotate);
            enamyTankMode.erase(it_mode);
            enamyTankReloadTimer.erase(it_timer);
            enamyTankDoMove.erase(it_move);
            enamyTankDistance_to_Wall.erase(it_dtw);
            enamyTankStuntTimer.erase(it_stunt);
            enamyTankCharmTimer.erase(it_charm);
            enamyTankCharmTarget.erase(it_charmTarget);
            enamyTankShootable.erase(it_shootable);
            enamyTankMaxHP.erase(it_maxHP);
            break;
        }
        it++;
        it_rotate++;
        it_fRotate++;
        it_mode++;
        it_timer++;
        it_move++;
        it_id++;
        it_dtw++;
        it_stunt++;
        it_charm++;
        it_charmTarget++;
        it_shootable++;
        it_maxHP++;
    }
    if (percentile_for_ult < 100)
        percentile_for_ult += 30;
}

float enemytankNmove::enemyControlMouseMove(Vector2f tankPos,Vector2f targetPos) {
    if (targetPos.x > tankPos.x && targetPos.y < tankPos.y) {
        float opp = tankPos.y - targetPos.y;
        float adj = targetPos.x - tankPos.x;
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        return asin(opp / hyp) * (180.f / 3.14);
    }
    else if (targetPos.x < tankPos.x && targetPos.y < tankPos.y) {
        float opp = tankPos.y - targetPos.y;
        float adj = tankPos.x - targetPos.x;
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        return (90.f - (asin(opp / hyp) * (180.f / 3.14))) + 90.f;
    }
    else if (targetPos.x < tankPos.x && targetPos.y > tankPos.y) {
        float opp = targetPos.y - tankPos.y;
        float adj = tankPos.x - targetPos.x;
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        return asin(opp / hyp) * (180.f / 3.14) + 180.f;
    }
    else if (tankPos.x < targetPos.x && targetPos.y > tankPos.y) {
        float opp = targetPos.y - tankPos.y;
        float adj = targetPos.x - tankPos.x;
        float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
        return (180.f - (asin(opp / hyp) * (180.f / 3.14))) + 180.f;
    }
}

void enemytankNmove::enamyAIShoot(string mode, Sprite sprite, string status, string targetID, bool isBoss) {
    if (mode == "rocket") 
        ammo.createRocket(sprite.getRotation(), sprite.getPosition(), status);
    else if (mode == "missile") {
        ammo.createMissile(sprite.getRotation(), sprite.getPosition(), targetID, status);
    }
    if (mode == "rocket" && isBoss) {
        ammo.createRocket(sprite.getRotation() + 15, sprite.getPosition(), status);
        ammo.createRocket(sprite.getRotation() - 15, sprite.getPosition(), status);
    }
    if (mode == "missile" && isBoss) {
        ammo.createMissile(sprite.getRotation() + 90, sprite.getPosition(), targetID, status);
        ammo.createMissile(sprite.getRotation() - 90, sprite.getPosition(), targetID, status);
    }
}