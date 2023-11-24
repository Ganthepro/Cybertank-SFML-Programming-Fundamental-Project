#include "Game.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "tankNmove.h"
#include "enemytankNmove.h"
#include "ammoNdamage.h"
#include "Ux_Ui.h"
#include "itemNskill.h"
#include <chrono>
#include <vector>
#include <thread>
#include "Map.h"
#include "SoundsStruct.h"
using namespace sf;
using namespace std;

Game::Game() {
    if (!bufferGun.loadFromFile("resource/gun.wav"))
        return;
    if (!bufferMissile.loadFromFile("resource/missile.wav"))
        return;
    if (!bufferRocket.loadFromFile("resource/rocket.wav"))
        return;
    if (!bufferExplosion.loadFromFile("resource/explosion.wav"))
        return;
    if (!bufferExplosion2.loadFromFile("resource/explosion2.wav"))
        return;
}

float Game::findPositionLeftSound(int tankPos, int winSize) {
    float pos_L;
    if (tankPos < winSize / 2) 
        pos_L = -1;
    else 
        pos_L = 8 - (tankPos * 8 / (winSize / 2));
    return pos_L;
}

float Game::findPositionRightSound(int tankPos, int winSize) {
    float pos_R;
    if (tankPos < winSize / 2)
        pos_R = 8 - (tankPos * 8 / (winSize / 2));
    else 
        pos_R = 1;
    return pos_R;
}

void Game::run(int modeWidth, int modeHeight, bool* flagGameStart) {
    float rocketSpeed = 6000;
    int rocketReloadTimer = 0;
    int rocket_missileReloadTimerLimit = 360;
    int missileReloadTimer = 0;
    int bulletReloadTimer = 0;
    int bulletReloadTimerLimit = 5;
    bool playerShootMissile = false;
    bool lock_on_showable = false;
    string playerShootMissileTargetID = "";
    VideoMode desktopMode = VideoMode::getDesktopMode();
    RenderWindow window(VideoMode(modeWidth, modeHeight), "Cybertank", Style::Fullscreen);
    //RenderWindow window(VideoMode(1400, 900), "Cybertank");
    RectangleShape lock_on;
    lock_on.setFillColor(Color::Transparent);
    lock_on.setRotation(45);
    lock_on.setOutlineColor(Color::Green);
    lock_on.setOutlineThickness(1.f);
    lock_on.setSize(Vector2f(100.f, 100.f));
    window.setFramerateLimit(60);
    Map winArt(modeWidth, modeHeight, window);
    winArt.createBackground(12, 12);
    ammoNdamage ammo(window);
    tankNmove tank(window, ammo, winArt.backgroundRect);
    enemytankNmove enemyTank(window, ammo, winArt.backgroundRect);
    itemNskill item(window, enemyTank, ammo);
    Ux_Ui ux_ui(window, modeWidth, modeHeight);
    ux_ui.inputText = &*userName;
    string method;
    int w, a, s, d;
    w = 0; a = 1; s = 2; d = 3;
    tank.createTank();
    sounds.soundExplosion_L.setBuffer(bufferExplosion);
    sounds.soundExplosion_R.setBuffer(bufferExplosion);
    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
                window.close();
            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Middle && tank.mode == "battle" && rocketReloadTimer == 180) {
                ammo.createRocket(tank.getRotate(), tank.getPos(), "player");
                rocketReloadTimer = 0;
            }
            if (event.key.code == Keyboard::E && item.collectedSkill != "") {
                if (item.collectedSkill == "emp")
                    item.use_emp(tank.getPos());
                else if (item.collectedSkill == "charm")
                    item.use_charm(tank.getPos());
                else if (item.collectedSkill == "2xReload")
                    item.use_2xReload(&bulletReloadTimerLimit);
                item.collectedSkill = "";
            }
            if (event.key.code == Keyboard::R && enemyTank.percentile_for_ult >= 100) {
                item.use_ult(tank.getPos(), tank.getRotate());
                item.collectedSkill = "";
            }
            if (event.key.code == Keyboard::Space && tank.speed < 100) {
                tank.speed += 300;
            }
            if (event.key.code == Keyboard::Num1 && tank.mode == "battle" && event.type == Event::KeyPressed && missileReloadTimer >= 180) {
                playerShootMissile = true;
                missileReloadTimer = 0;
            }
            if (event.type == sf::Event::KeyPressed) {
                if (ux_ui.pauseScreenFlag && event.key.code == Keyboard::Q) {
                    *flagGameStart = false;
                    window.close();
                }
                if (event.key.code == Keyboard::Escape) {
                    if (!ux_ui.pauseScreenFlag)
                        ux_ui.pauseScreenFlag = true;
                    else
                        ux_ui.pauseScreenFlag = false;
                }
            }
        }
        if (ux_ui.pauseScreenFlag)
            ux_ui.pauseScreen();
        else {
            if (Mouse::isButtonPressed(Mouse::Right))
                tank.mode = "battle";
            else
                tank.mode = "drive";
            if (rocketReloadTimer < 180)
                rocketReloadTimer++;
            if (missileReloadTimer < 180)
                missileReloadTimer++;
            if (bulletReloadTimer < bulletReloadTimerLimit)
                bulletReloadTimer++;
            auto it = item.itemsDropedTimer.begin();
            while (it != item.itemsDropedTimer.end()) {
                *it += 1;
                it++;
            }
            if (tank.mode == "battle") {
                lock_on_showable = true;
                if (Keyboard::isKeyPressed(Keyboard::W))
                {
                    tank.direction[w] = true;
                    if (tank.speed < 100) {
                        tank.speed += tank.acceleration / 60;
                    }
                    else {
                        tank.speed -= tank.acceleration / 60;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::D))
                {
                    tank.direction[d] = true;
                    if (tank.speed < 100) {
                        tank.speed += tank.acceleration / 60;
                    }
                    else {
                        tank.speed -= tank.acceleration / 60;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::A))
                {
                    tank.direction[a] = true;
                    if (tank.speed < 100) {
                        tank.speed += tank.acceleration / 60;
                    }
                    else {
                        tank.speed -= tank.acceleration / 60;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::S))
                {
                    tank.direction[s] = true;
                    if (tank.speed < 100) {
                        tank.speed += tank.acceleration / 60;
                    }
                    else {
                        tank.speed -= tank.acceleration / 60;
                    }
                }
                if (Mouse::isButtonPressed(Mouse::Left)) {
                    if (bulletReloadTimer >= bulletReloadTimerLimit) {
                        ammo.createBullet(tank.getRotate(), tank.getPos());
                        bulletReloadTimer = 0;
                    }
                }
                if ((!Keyboard::isKeyPressed(Keyboard::W) && !Keyboard::isKeyPressed(Keyboard::A) && !Keyboard::isKeyPressed(Keyboard::D)
                    && !Keyboard::isKeyPressed(Keyboard::S)) || Keyboard::isKeyPressed(Keyboard::Space))
                {
                    while (tank.speed > 0.f) {
                        if (rocketReloadTimer < 180)
                            rocketReloadTimer++;
                        if (missileReloadTimer < 180)
                            missileReloadTimer++;
                        if (bulletReloadTimer < bulletReloadTimerLimit)
                            bulletReloadTimer++;
                        auto it = item.itemsDropedTimer.begin();
                        while (it != item.itemsDropedTimer.end()) {
                            *it += 1;
                            it++;
                        }
                        if (Mouse::isButtonPressed(Mouse::Left)) {
                            if (bulletReloadTimer >= bulletReloadTimerLimit) {
                                ammo.createBullet(tank.getRotate(), tank.getPos());
                                bulletReloadTimer = 0;
                            }
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Space)) {
                            tank.speed -= tank.breakRetardation / 60;
                        }
                        else {
                            tank.speed -= tank.retardation / 60;
                        }
                        while (window.pollEvent(event))
                        {
                            if (event.type == Event::Closed)
                                window.close();
                            if (event.key.code == Keyboard::E && item.collectedSkill != "") {
                                if (item.collectedSkill == "emp")
                                    item.use_emp(tank.getPos());
                                else if (item.collectedSkill == "charm")
                                    item.use_charm(tank.getPos());
                                else if (item.collectedSkill == "2xReload")
                                    item.use_2xReload(&bulletReloadTimerLimit);
                                item.collectedSkill = "";
                            }
                            if (event.key.code == Keyboard::R && enemyTank.percentile_for_ult >= 100) {
                                item.use_ult(tank.getPos(), tank.getRotate());
                                item.collectedSkill = "";
                            }
                            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Middle && tank.mode == "battle" && rocketReloadTimer == 180) {
                                ammo.createRocket(tank.getRotate(), tank.getPos(), "player");
                                rocketReloadTimer = 0;
                            }
                            if (event.key.code == Keyboard::Num1 && tank.mode == "battle" && event.type == Event::KeyPressed && missileReloadTimer >= 180) {
                                playerShootMissile = true;
                                missileReloadTimer = 0;
                        float pos_L, pos_R;
                        if (tank.getPos().x < window.getSize().x / 2) {
                            pos_L = -1;
                            pos_R = 8 - (tank.getPos().x * 8 / (window.getSize().x / 2));
                        }
                        else {
                            pos_R = 1;
                            pos_L = 8 - (tank.getPos().x * 8 / (window.getSize().x / 2));
                        }
                        sounds.soundGun_L.setPosition(pos_L, 0, 0);
                        sounds.soundGun_L.play();
                        sounds.soundGun_R.setPosition(pos_R, 0, 0);
                        sounds.soundGun_R.play();
                            }
                        }
                        auto it_enamyPos = enemyTank.enamyTankPos.begin();
                        auto it_enamyID = enemyTank.enamyTankID.begin();
                        float distance = NULL;
                        Vector2f Pos;
                        string targetID = "";
                        while (it_enamyPos != enemyTank.enamyTankPos.end()) {
                            float opp = it_enamyPos->y - float(Mouse::getPosition(window).y);
                            float adj = float(Mouse::getPosition(window).x) - it_enamyPos->x;
                            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
                            if (distance == NULL && targetID == "") {
                                distance = hyp;
                                targetID = *it_enamyID;
                                Pos = *it_enamyPos;
                            }
                            else if (hyp < distance) {
                                distance = hyp;
                                targetID = *it_enamyID;
                                Pos = *it_enamyPos;
                            }
                            it_enamyPos++;
                            it_enamyID++;
                        }
                        playerShootMissileTargetID = targetID;
                        lock_on.setPosition(Pos.x, Pos.y - 70.f);
                        tank.battleModeControlKeyboardMove(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y), winArt.backgroundRect);
                        tank.battleModeControlMouseMove(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y));
                        window.clear();
                        winArt.render();
                        tank.draw(playerShootMissile, playerShootMissileTargetID);
                        ux_ui.healthBar(tank.getPos(), tank.hp, enemyTank.enamyTankPos, enemyTank.enamyTankHP, enemyTank.enamyTankMaxHP);
                        ux_ui.playerLockOn(ammo.missileTarget, ammo.missile, tank.getPos());
                        ux_ui.reloadTimer(rocketReloadTimer, missileReloadTimer, bulletReloadTimer);
                        window.draw(lock_on);
                        enemyTank.Wave(tank.getBox());
                        ux_ui.waveBar(enemyTank.wave, enemyTank.waves);
                        ux_ui.ultBar(enemyTank.percentile_for_ult);
                        if (ux_ui.pauseScreenFlag)
                            ux_ui.pauseScreen();
                        if (playerShootMissile)
                            playerShootMissile = false;
                        item.draw(tank.getBox(), &bulletReloadTimerLimit);
                        enemyTank.draw(tank.getBox(), tank.getPos());
                        if (ammo.rocket.size() > 0)
                        {
                            ammo.battleModeShootMiddleMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it != ammo.rocket.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.rocket.erase(it);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it++;
                                        it_rocketID++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_rocket = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it_rocket != ammo.rocket.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID))
                                        || (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_rocket = ammo.rocket.erase(it_rocket);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it_rocket++;
                                        it_rocketID++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.bullet.size() > 0)
                        {
                            ammo.battleModeShootLeftMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.bullet.begin();
                                
                                while (it != ammo.bullet.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        it = ammo.bullet.erase(it);
                                    }
                                    else {
                                        it++;
                                        
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (it_enemy->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        *it_enemyHP -= 5;
                                        if (*it_enemyHP < 0) {
                                            enemyTank.destroyTank(*it_enemyID);
                                            ux_ui.score += 50;
                                            int Rand = rand() % 50;
                                            bool flag_drop = false;
                                            for (int i = 2; i < Rand; i++) {
                                                if (Rand % i == 0)
                                                    flag_drop = true;
                                            }
                                            if (!flag_drop)
                                                item.dropItem(it_enemy->getPosition());
                                            it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                            it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                            it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                            flag = true;
                                        }
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        it_bullet++;
                                        
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.missile.size() > 0)
                        {
                            ammo.battleModeShoot1Keyboard(enemyTank.enamyTankID, enemyTank.enamyTankPos, Vector2f(tank.tankPos[0], tank.tankPos[1]));
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it != ammo.missile.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds()))
                                    {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.missile.erase(it);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                    }
                                    else
                                    {
                                        it++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_missile = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it_missile != ammo.missile.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds()) && *it_missileID == "player")
                                        || (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds()) && *it_missileID == "player") {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        it_missile++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                            auto it_missile = ammo.missile.begin();
                            auto it_missileID = ammo.missile_id.begin();
                            auto it_missileFrotate = ammo.missileFrotate.begin();
                            auto it_missileTarget = ammo.missileTarget.begin();
                            flag = false;
                            while (it_missile != ammo.missile.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (*it_missileID != "player" && it_missile->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        flag = true;
                                        break;
                                    }
                                    it_bullet++;
                                    
                                }
                                if (flag)
                                    break;
                                it_missile++;
                                it_missileID++;
                                it_missileFrotate++;
                                it_missileTarget++;
                            }
                        }
                        window.display();
                        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::A) || Keyboard::isKeyPressed(Keyboard::D) || Keyboard::isKeyPressed(Keyboard::S))
                            break;
                    }
                }
                if ((tank.direction[w] && Keyboard::isKeyPressed(Keyboard::S)) || (tank.direction[s] && Keyboard::isKeyPressed(Keyboard::W))) {
                    while (tank.speed > 0) {
                        if (rocketReloadTimer < 180)
                            rocketReloadTimer++;
                        if (missileReloadTimer < 180)
                            missileReloadTimer++;
                        if (bulletReloadTimer < bulletReloadTimerLimit)
                            bulletReloadTimer++;
                        auto it = item.itemsDropedTimer.begin();
                        while (it != item.itemsDropedTimer.end()) {
                            *it += 1;
                            it++;
                        }
                        if (Mouse::isButtonPressed(Mouse::Left)) {
                            if (bulletReloadTimer >= bulletReloadTimerLimit) {
                                ammo.createBullet(tank.getRotate(), tank.getPos());
                                bulletReloadTimer = 0;
                            }
                        }
                        tank.speed -= tank.breakRetardation / 60;
                        while (window.pollEvent(event))
                        {
                            if (event.type == Event::Closed)
                                window.close();
                            if (event.type == Event::MouseButtonPressed && event.mouseButton.button == Mouse::Middle && tank.mode == "battle" && rocketReloadTimer == 180) {
                                ammo.createRocket(tank.getRotate(), tank.getPos(), "player");
                                rocketReloadTimer = 0;
                            }
                            if (event.key.code == Keyboard::E && item.collectedSkill != "") {
                                if (item.collectedSkill == "emp")
                                    item.use_emp(tank.getPos());
                                else if (item.collectedSkill == "charm")
                                    item.use_charm(tank.getPos());
                                else if (item.collectedSkill == "2xReload")
                                    item.use_2xReload(&bulletReloadTimerLimit);
                                item.collectedSkill = "";
                            }
                            if (event.key.code == Keyboard::R && enemyTank.percentile_for_ult >= 100) {
                                item.use_ult(tank.getPos(), tank.getRotate());
                                item.collectedSkill = "";
                            }
                            if (event.key.code == Keyboard::Num1 && tank.mode == "battle" && event.type == Event::KeyPressed && missileReloadTimer >= 180) {
                                playerShootMissile = true;
                                missileReloadTimer = 0;
                            }
                        }
                        auto it_enamyPos = enemyTank.enamyTankPos.begin();
                        auto it_enamyID = enemyTank.enamyTankID.begin();
                        float distance = NULL;
                        Vector2f Pos;
                        string targetID = "";
                        while (it_enamyPos != enemyTank.enamyTankPos.end()) {
                            float opp = it_enamyPos->y - float(Mouse::getPosition(window).y);
                            float adj = float(Mouse::getPosition(window).x) - it_enamyPos->x;
                            float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
                            if (distance == NULL && targetID == "") {
                                distance = hyp;
                                targetID = *it_enamyID;
                                Pos = *it_enamyPos;
                            }
                            else if (hyp < distance) {
                                distance = hyp;
                                targetID = *it_enamyID;
                                Pos = *it_enamyPos;
                            }
                            it_enamyPos++;
                            it_enamyID++;
                        }
                        playerShootMissileTargetID = targetID;
                        lock_on.setPosition(Pos.x, Pos.y - 70.f);
                        tank.battleModeControlKeyboardMove(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y), winArt.backgroundRect);
                        tank.battleModeControlMouseMove(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y));
                        window.clear();
                        winArt.render();
                        tank.draw(playerShootMissile, playerShootMissileTargetID);
                        ux_ui.healthBar(tank.getPos(), tank.hp, enemyTank.enamyTankPos, enemyTank.enamyTankHP, enemyTank.enamyTankMaxHP);
                        ux_ui.playerLockOn(ammo.missileTarget, ammo.missile, tank.getPos());
                        ux_ui.reloadTimer(rocketReloadTimer, missileReloadTimer, bulletReloadTimer);
                        window.draw(lock_on);
                        enemyTank.Wave(tank.getBox());
                        ux_ui.waveBar(enemyTank.wave, enemyTank.waves);
                        ux_ui.ultBar(enemyTank.percentile_for_ult);
                        if (ux_ui.pauseScreenFlag)
                            ux_ui.pauseScreen();
                        if (playerShootMissile)
                            playerShootMissile = false;
                        item.draw(tank.getBox(), &bulletReloadTimerLimit);
                        enemyTank.draw(tank.getBox(), tank.getPos());
                        if (ammo.rocket.size() > 0)
                        {
                            ammo.battleModeShootMiddleMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it != ammo.rocket.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.rocket.erase(it);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it++;
                                        it_rocketID++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_rocket = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it_rocket != ammo.rocket.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID))
                                        || (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_rocket = ammo.rocket.erase(it_rocket);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it_rocket++;
                                        it_rocketID++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.bullet.size() > 0)
                        {
                            ammo.battleModeShootLeftMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.bullet.begin();
                                
                                while (it != ammo.bullet.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        it = ammo.bullet.erase(it);
                                        
                                    }
                                    else {
                                        it++;
                                        
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (it_enemy->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        *it_enemyHP -= 5;
                                        if (*it_enemyHP < 0) {
                                            enemyTank.destroyTank(*it_enemyID);
                                            ux_ui.score += 50;
                                            int Rand = rand() % 50;
                                            bool flag_drop = false;
                                            for (int i = 2; i < Rand; i++) {
                                                if (Rand % i == 0)
                                                    flag_drop = true;
                                            }
                                            if (!flag_drop)
                                                item.dropItem(it_enemy->getPosition());
                                            it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                            it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                            it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                            flag = true;
                                        }
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        if (flag)
                                            break;
                                    }
                                    else 
                                        it_bullet++;
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.missile.size() > 0)
                        {
                            ammo.battleModeShoot1Keyboard(enemyTank.enamyTankID, enemyTank.enamyTankPos, Vector2f(tank.tankPos[0], tank.tankPos[1]));
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it != ammo.missile.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds()))
                                    {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.missile.erase(it);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                    }
                                    else
                                    {
                                        it++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_missile = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it_missile != ammo.missile.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds()) && (*it_missileID == "player" || *it_enemyID != *it_missileID))
                                        || (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds())
                                            && (*it_missileID == "player" || *it_enemyID != *it_missileID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        it_missile++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                            auto it_missile = ammo.missile.begin();
                            auto it_missileID = ammo.missile_id.begin();
                            auto it_missileFrotate = ammo.missileFrotate.begin();
                            auto it_missileTarget = ammo.missileTarget.begin();
                            flag = false;
                            while (it_missile != ammo.missile.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (*it_missileID != "player" && it_missile->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        flag = true;
                                        break;
                                    }
                                    it_bullet++;
                                    
                                }
                                if (flag)
                                    break;
                                it_missile++;
                                it_missileID++;
                                it_missileFrotate++;
                                it_missileTarget++;
                            }
                        }
                        window.display();
                    }
                }
                if (!Keyboard::isKeyPressed(Keyboard::W))
                    tank.direction[w] = false;
                if (!Keyboard::isKeyPressed(Keyboard::A))
                    tank.direction[a] = false;
                if (!Keyboard::isKeyPressed(Keyboard::S))
                    tank.direction[s] = false;
                if (!Keyboard::isKeyPressed(Keyboard::D))
                    tank.direction[d] = false;
                tank.battleModeControlKeyboardMove(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y), winArt.backgroundRect);
                tank.battleModeControlMouseMove(float(Mouse::getPosition(window).x), float(Mouse::getPosition(window).y));
            }
            else if (tank.mode == "drive") {
                lock_on_showable = false;
                if (Keyboard::isKeyPressed(Keyboard::W)) {
                    tank.direction[w] = true;
                    if (Mouse::isButtonPressed(Mouse::Left)) {
                        if (tank.speed < 600) {
                            tank.speed += (tank.driveAcceleration * 2) / 60;
                        }
                    }
                    else {
                        if (tank.speed < 300) {
                            tank.speed += tank.driveAcceleration / 60;
                        }
                        else {
                            tank.speed -= tank.breakRetardation / 60;
                        }
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::A)) {
                    tank.direction[a] = true;
                    if (tank.speed < 300) {
                        tank.speed += tank.driveAcceleration / 60;
                    }
                }
                else {
                    tank.direction[a] = false;
                }
                if (Keyboard::isKeyPressed(Keyboard::S)) {
                    tank.direction[s] = true;
                    if (tank.speed < 300) {
                        tank.speed += tank.driveAcceleration / 60;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::D)) {
                    tank.direction[d] = true;
                    if (tank.speed < 300) {
                        tank.speed += tank.driveAcceleration / 60;
                    }
                }
                else {
                    tank.direction[d] = false;
                }
                if ((!Keyboard::isKeyPressed(Keyboard::W) && !Keyboard::isKeyPressed(Keyboard::S)) || Keyboard::isKeyPressed(Keyboard::Space))
                {
                    while (tank.speed > 0.f) {
                        if (rocketReloadTimer < 180)
                            rocketReloadTimer++;
                        if (missileReloadTimer < 180)
                            missileReloadTimer++;
                        if (bulletReloadTimer < 5)
                            bulletReloadTimer++;
                        auto it = item.itemsDropedTimer.begin();
                        while (it != item.itemsDropedTimer.end()) {
                            *it += 1;
                            it++;
                        }
                        if (Keyboard::isKeyPressed(Keyboard::Space))
                            tank.speed -= tank.breakRetardation / 60;
                        else
                            tank.speed -= tank.retardation / 60;
                        tank.driveModeControlKeyboardMove(winArt.backgroundRect);
                        window.clear();
                        item.draw(tank.getBox(), &bulletReloadTimerLimit);
                        enemyTank.draw(tank.getBox(), tank.getPos());
                        winArt.render();
                        tank.draw(playerShootMissile, playerShootMissileTargetID);
                        ux_ui.healthBar(tank.getPos(), tank.hp, enemyTank.enamyTankPos, enemyTank.enamyTankHP, enemyTank.enamyTankMaxHP);
                        ux_ui.playerLockOn(ammo.missileTarget, ammo.missile, tank.getPos());
                        ux_ui.reloadTimer(rocketReloadTimer, missileReloadTimer, bulletReloadTimer);
                        enemyTank.Wave(tank.getBox());
                        ux_ui.waveBar(enemyTank.wave, enemyTank.waves);
                        ux_ui.ultBar(enemyTank.percentile_for_ult);
                        if (ux_ui.pauseScreenFlag)
                            ux_ui.pauseScreen();
                        if (playerShootMissile)
                            playerShootMissile = false;
                        if (ammo.rocket.size() > 0)
                        {
                            ammo.battleModeShootMiddleMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it != ammo.rocket.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.rocket.erase(it);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it++;
                                        it_rocketID++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_rocket = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it_rocket != ammo.rocket.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID))
                                        || (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_rocket = ammo.rocket.erase(it_rocket);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it_rocket++;
                                        it_rocketID++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.bullet.size() > 0)
                        {
                            ammo.battleModeShootLeftMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.bullet.begin();
                                
                                while (it != ammo.bullet.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        it = ammo.bullet.erase(it);
                                        
                                    }
                                    else {
                                        it++;
                                        
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (it_enemy->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        *it_enemyHP -= 5;
                                        if (*it_enemyHP < 0) {
                                            enemyTank.destroyTank(*it_enemyID);
                                            ux_ui.score += 50;
                                            int Rand = rand() % 50;
                                            bool flag_drop = false;
                                            for (int i = 2; i < Rand; i++) {
                                                if (Rand % i == 0)
                                                    flag_drop = true;
                                            }
                                            if (!flag_drop)
                                                item.dropItem(it_enemy->getPosition());
                                            it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                            it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                            it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                            flag = true;
                                        }
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        it_bullet++;
                                        
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.missile.size() > 0)
                        {
                            ammo.battleModeShoot1Keyboard(enemyTank.enamyTankID, enemyTank.enamyTankPos, Vector2f(tank.tankPos[0], tank.tankPos[1]));
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it != ammo.missile.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds()))
                                    {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.missile.erase(it);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                    }
                                    else
                                    {
                                        it++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_missile = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it_missile != ammo.missile.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds()) && (*it_missileID == "player" || *it_enemyID != *it_missileID))
                                        || (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds())
                                            && (*it_missileID == "player" || *it_enemyID != *it_missileID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        it_missile++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                            auto it_missile = ammo.missile.begin();
                            auto it_missileID = ammo.missile_id.begin();
                            auto it_missileFrotate = ammo.missileFrotate.begin();
                            auto it_missileTarget = ammo.missileTarget.begin();
                            flag = false;
                            while (it_missile != ammo.missile.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (*it_missileID != "player" && it_missile->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        flag = true;
                                        break;
                                    }
                                    it_bullet++;
                                    
                                }
                                if (flag)
                                    break;
                                it_missile++;
                                it_missileID++;
                                it_missileFrotate++;
                                it_missileTarget++;
                            }
                        }
                        window.display();
                        if (Keyboard::isKeyPressed(Keyboard::W) || Keyboard::isKeyPressed(Keyboard::S))
                            break;
                        if (Keyboard::isKeyPressed(Keyboard::A))
                            tank.direction[a] = true;
                        else
                            tank.direction[a] = false;
                        if (Keyboard::isKeyPressed(Keyboard::D))
                            tank.direction[d] = true;
                        else
                            tank.direction[d] = false;
                        if (Mouse::isButtonPressed(Mouse::Right))
                            break;
                    }
                }
                if ((tank.direction[w] && Keyboard::isKeyPressed(Keyboard::S)) || (tank.direction[s] && Keyboard::isKeyPressed(Keyboard::W))) {
                    while (tank.speed > 0) {
                        if (rocketReloadTimer < 180)
                            rocketReloadTimer++;
                        if (missileReloadTimer < 180)
                            missileReloadTimer++;
                        if (bulletReloadTimer < 5)
                            bulletReloadTimer++;
                        auto it = item.itemsDropedTimer.begin();
                        while (it != item.itemsDropedTimer.end()) {
                            *it += 1;
                            it++;
                        }
                        tank.speed -= tank.breakRetardation / 60;
                        tank.driveModeControlKeyboardMove(winArt.backgroundRect);
                        window.clear();
                        item.draw(tank.getBox(), &bulletReloadTimerLimit);
                        enemyTank.draw(tank.getBox(), tank.getPos());
                        winArt.render();
                        tank.draw(playerShootMissile, playerShootMissileTargetID);
                        ux_ui.healthBar(tank.getPos(), tank.hp, enemyTank.enamyTankPos, enemyTank.enamyTankHP, enemyTank.enamyTankMaxHP);
                        ux_ui.playerLockOn(ammo.missileTarget, ammo.missile, tank.getPos());
                        ux_ui.reloadTimer(rocketReloadTimer, missileReloadTimer, bulletReloadTimer);
                        enemyTank.Wave(tank.getBox());
                        ux_ui.waveBar(enemyTank.wave, enemyTank.waves);
                        ux_ui.ultBar(enemyTank.percentile_for_ult);
                        if (ux_ui.pauseScreenFlag)
                            ux_ui.pauseScreen();
                        if (playerShootMissile)
                            playerShootMissile = false;
                        if (ammo.rocket.size() > 0)
                        {
                            ammo.battleModeShootMiddleMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it != ammo.rocket.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.rocket.erase(it);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it++;
                                        it_rocketID++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_rocket = ammo.rocket.begin();
                                auto it_rocketID = ammo.rocket_id.begin();
                                while (it_rocket != ammo.rocket.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID))
                                        || (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_rocket = ammo.rocket.erase(it_rocket);
                                        it_rocketID = ammo.rocket_id.erase(it_rocketID);
                                    }
                                    else {
                                        it_rocket++;
                                        it_rocketID++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.bullet.size() > 0)
                        {
                            ammo.battleModeShootLeftMouse();
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.bullet.begin();
                                
                                while (it != ammo.bullet.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                                        it = ammo.bullet.erase(it);
                                        
                                    }
                                    else {
                                        it++;
                                        
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (it_enemy->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        *it_enemyHP -= 5;
                                        if (*it_enemyHP < 0) {
                                            enemyTank.destroyTank(*it_enemyID);
                                            ux_ui.score += 50;
                                            int Rand = rand() % 50;
                                            bool flag_drop = false;
                                            for (int i = 2; i < Rand; i++) {
                                                if (Rand % i == 0)
                                                    flag_drop = true;
                                            }
                                            if (!flag_drop)
                                                item.dropItem(it_enemy->getPosition());
                                            it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                            it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                            it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                            flag = true;
                                        }
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        
                                        it_bullet++;
                                    }
                                }
                                if (flag)
                                    break;
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                        }
                        if (ammo.missile.size() > 0)
                        {
                            ammo.battleModeShoot1Keyboard(enemyTank.enamyTankID, enemyTank.enamyTankPos, Vector2f(tank.tankPos[0], tank.tankPos[1]));
                            auto it_bg = winArt.backgroundRect.begin();
                            while (it_bg != winArt.backgroundRect.end())
                            {
                                auto it = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it != ammo.missile.end()) {
                                    if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds()))
                                    {
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it = ammo.missile.erase(it);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                    }
                                    else
                                    {
                                        it++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                it_bg++;
                            }
                            auto it_enemy = enemyTank.enamyTankSprite.begin();
                            auto it_enemyID = enemyTank.enamyTankID.begin();
                            auto it_enemyHP = enemyTank.enamyTankHP.begin();
                            bool flag = false;
                            while (it_enemy != enemyTank.enamyTankSprite.end()) {
                                auto it_missile = ammo.missile.begin();
                                auto it_missileID = ammo.missile_id.begin();
                                auto it_missileFrotate = ammo.missileFrotate.begin();
                                auto it_missileTarget = ammo.missileTarget.begin();
                                int i = 0;
                                while (it_missile != ammo.missile.end()) {
                                    if ((it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds()) && (*it_missileID == "player" || *it_enemyID != *it_missileID))
                                        || (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player")) {
                                        if (it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds())
                                            && (*it_missileID == "player" || *it_enemyID != *it_missileID)) {
                                            *it_enemyHP -= 30;
                                            if (*it_enemyHP < 0) {
                                                enemyTank.destroyTank(*it_enemyID);
                                                ux_ui.score += 50;
                                                int Rand = rand() % 50;
                                                bool flag_drop = false;
                                                for (int i = 2; i < Rand; i++) {
                                                    if (Rand % i == 0)
                                                        flag_drop = true;
                                                }
                                                if (!flag_drop)
                                                    item.dropItem(it_enemy->getPosition());
                                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                                flag = true;
                                            }
                                        }
                                        else if (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player") {
                                            tank.hp -= 30;
                                            if (tank.hp <= 0) {
                                                ux_ui.deathScreen();
                                                if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                                    ux_ui.updateData();
                                                    window.close();
                                                    return;
                                                }
                                                if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                                    ux_ui.updateData();
                                                    *flagGameStart = false;
                                                    return;
                                                    window.close();
                                                }
                                                continue;
                                            }
                                        }
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        i--;
                                        if (flag)
                                            break;
                                    }
                                    else {
                                        it_missile++;
                                        it_missileID++;
                                        it_missileFrotate++;
                                        it_missileTarget++;
                                        i++;
                                    }
                                }
                                it_enemy++;
                                it_enemyID++;
                                it_enemyHP++;
                            }
                            auto it_missile = ammo.missile.begin();
                            auto it_missileID = ammo.missile_id.begin();
                            auto it_missileFrotate = ammo.missileFrotate.begin();
                            auto it_missileTarget = ammo.missileTarget.begin();
                            flag = false;
                            while (it_missile != ammo.missile.end()) {
                                auto it_bullet = ammo.bullet.begin();
                                
                                while (it_bullet != ammo.bullet.end()) {
                                    if (*it_missileID != "player" && it_missile->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                                        it_bullet = ammo.bullet.erase(it_bullet);
                                        sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_L.play();
                                        sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                                        sounds.soundExplosion_R.play();
                                        it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                                        it_missile = ammo.missile.erase(it_missile);
                                        it_missileID = ammo.missile_id.erase(it_missileID);
                                        it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                                        flag = true;
                                        break;
                                    }
                                    it_bullet++;
                                    
                                }
                                if (flag)
                                    break;
                                it_missile++;
                                it_missileID++;
                                it_missileFrotate++;
                                it_missileTarget++;
                            }
                        }
                        window.display();
                        if (Mouse::isButtonPressed(Mouse::Right))
                            break;
                    }
                }
                tank.driveModeControlKeyboardMove(winArt.backgroundRect);
            }
            if (lock_on_showable) {
                auto it_enamyPos = enemyTank.enamyTankPos.begin();
                auto it_enamyID = enemyTank.enamyTankID.begin();
                float distance = NULL;
                Vector2f Pos;
                string targetID = "";
                while (it_enamyPos != enemyTank.enamyTankPos.end()) {
                    float opp = it_enamyPos->y - float(Mouse::getPosition(window).y);
                    float adj = float(Mouse::getPosition(window).x) - it_enamyPos->x;
                    float hyp = sqrtf(powf(opp, 2.f) + powf(adj, 2.f));
                    if (distance == NULL && targetID == "") {
                        distance = hyp;
                        targetID = *it_enamyID;
                        Pos = *it_enamyPos;
                    }
                    else if (hyp < distance) {
                        distance = hyp;
                        targetID = *it_enamyID;
                        Pos = *it_enamyPos;
                    }
                    it_enamyPos++;
                    it_enamyID++;
                }
                playerShootMissileTargetID = targetID;
                lock_on.setPosition(Pos.x, Pos.y - 70.f);
            }
            window.clear();
            item.draw(tank.getBox(), &bulletReloadTimerLimit);
            enemyTank.draw(tank.getBox(), tank.getPos());
            winArt.render();
            tank.draw(playerShootMissile, playerShootMissileTargetID);
            ux_ui.healthBar(tank.getPos(), tank.hp, enemyTank.enamyTankPos, enemyTank.enamyTankHP, enemyTank.enamyTankMaxHP);
            ux_ui.playerLockOn(ammo.missileTarget, ammo.missile, tank.getPos());
            ux_ui.reloadTimer(rocketReloadTimer, missileReloadTimer, bulletReloadTimer);
            enemyTank.Wave(tank.getBox());
            ux_ui.waveBar(enemyTank.wave, enemyTank.waves);
            ux_ui.ultBar(enemyTank.percentile_for_ult);
            if (ux_ui.pauseScreenFlag)
                ux_ui.pauseScreen();
            if (lock_on_showable)
                window.draw(lock_on);
            if (playerShootMissile)
                playerShootMissile = false;
            if (ammo.rocket.size() > 0)
            {
                ammo.battleModeShootMiddleMouse();
                auto it_bg = winArt.backgroundRect.begin();
                while (it_bg != winArt.backgroundRect.end())
                {
                    auto it = ammo.rocket.begin();
                    auto it_rocketID = ammo.rocket_id.begin();
                    while (it != ammo.rocket.end()) {
                        if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                            sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_L.play();
                            sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_R.play();
                            it = ammo.rocket.erase(it);
                            it_rocketID = ammo.rocket_id.erase(it_rocketID);
                        }
                        else {
                            it++;
                            it_rocketID++;
                        }
                    }
                    it_bg++;
                }
                auto it_enemy = enemyTank.enamyTankSprite.begin();
                auto it_enemyID = enemyTank.enamyTankID.begin();
                auto it_enemyHP = enemyTank.enamyTankHP.begin();
                bool flag = false;
                while (it_enemy != enemyTank.enamyTankSprite.end()) {
                    auto it_rocket = ammo.rocket.begin();
                    auto it_rocketID = ammo.rocket_id.begin();
                    while (it_rocket != ammo.rocket.end()) {
                        if ((it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID))
                            || (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player")) {
                            if (it_enemy->getGlobalBounds().intersects(it_rocket->getGlobalBounds()) && (*it_rocketID == "player" || *it_rocketID != *it_enemyID)) {
                                *it_enemyHP -= 30;
                                if (*it_enemyHP < 0) {
                                    enemyTank.destroyTank(*it_enemyID);
                                    ux_ui.score += 50;
                                    int Rand = rand() % 50;
                                    bool flag_drop = false;
                                    for (int i = 2; i < Rand; i++) {
                                        if (Rand % i == 0)
                                            flag_drop = true;
                                    }
                                    if (!flag_drop)
                                        item.dropItem(it_enemy->getPosition());
                                    it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                    it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                    it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                    flag = true;
                                }
                            }
                            else if (tank.getBox().intersects(it_rocket->getGlobalBounds()) && *it_rocketID != "player") {
                                tank.hp -= 30;
                                if (tank.hp <= 0) {
                                    ux_ui.deathScreen();
                                    if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                        ux_ui.updateData();
                                        window.close();
                                        return;
                                    }
                                    if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                        ux_ui.updateData();
                                        *flagGameStart = false;
                                        return;
                                        window.close();
                                    }
                                    continue;
                                }
                            }
                            sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_L.play();
                            sounds.soundExplosion_R.setPosition(findPositionRightSound(it_rocket->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_R.play();
                            it_rocket = ammo.rocket.erase(it_rocket);
                            it_rocketID = ammo.rocket_id.erase(it_rocketID);
                        }
                        else {
                            it_rocket++;
                            it_rocketID++;
                        }
                    }
                    if (flag)
                        break;
                    it_enemy++;
                    it_enemyID++;
                    it_enemyHP++;
                }
            }
            if (ammo.bullet.size() > 0)
            {
                ammo.battleModeShootLeftMouse();
                auto it_bg = winArt.backgroundRect.begin();
                while (it_bg != winArt.backgroundRect.end())
                {
                    auto it = ammo.bullet.begin();
                    
                    while (it != ammo.bullet.end()) {
                        if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds())) {
                            it = ammo.bullet.erase(it);
                            
                        }
                        else {
                            it++;
                            
                        }
                    }
                    it_bg++;
                }
                auto it_enemy = enemyTank.enamyTankSprite.begin();
                auto it_enemyID = enemyTank.enamyTankID.begin();
                auto it_enemyHP = enemyTank.enamyTankHP.begin();
                bool flag = false;
                while (it_enemy != enemyTank.enamyTankSprite.end()) {
                    auto it_bullet = ammo.bullet.begin();
                    
                    while (it_bullet != ammo.bullet.end()) {
                        if (it_enemy->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                            *it_enemyHP -= 5;
                            if (*it_enemyHP < 0) {
                                enemyTank.destroyTank(*it_enemyID);
                                ux_ui.score += 50;
                                int Rand = rand() % 50;
                                bool flag_drop = false;
                                for (int i = 2; i < Rand; i++) {
                                    if (Rand % i == 0)
                                        flag_drop = true;
                                }
                                if (!flag_drop)
                                    item.dropItem(it_enemy->getPosition());
                                it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                flag = true;
                            }
                            it_bullet = ammo.bullet.erase(it_bullet);
                            
                            if (flag)
                                break;
                        }
                        else {
                            it_bullet++;
                            
                        }
                    }
                    if (flag)
                        break;
                    it_enemy++;
                    it_enemyID++;
                    it_enemyHP++;
                }
            }
            if (ammo.missile.size() > 0)
            {
                ammo.battleModeShoot1Keyboard(enemyTank.enamyTankID, enemyTank.enamyTankPos, Vector2f(tank.tankPos[0], tank.tankPos[1]));
                auto it_bg = winArt.backgroundRect.begin();
                while (it_bg != winArt.backgroundRect.end())
                {
                    auto it = ammo.missile.begin();
                    auto it_missileID = ammo.missile_id.begin();
                    auto it_missileFrotate = ammo.missileFrotate.begin();
                    auto it_missileTarget = ammo.missileTarget.begin();
                    int i = 0;
                    while (it != ammo.missile.end()) {
                        if (it_bg->getGlobalBounds().intersects(it->getGlobalBounds()))
                        {
                            sounds.soundExplosion_L.setPosition(findPositionLeftSound(it->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_L.play();
                            sounds.soundExplosion_R.setPosition(findPositionRightSound(it->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_R.play();
                            it = ammo.missile.erase(it);
                            it_missileID = ammo.missile_id.erase(it_missileID);
                            it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                            it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                            i--;
                        }
                        else
                        {
                            it++;
                            it_missileID++;
                            it_missileFrotate++;
                            it_missileTarget++;
                            i++;
                        }
                    }
                    it_bg++;
                }
                auto it_enemy = enemyTank.enamyTankSprite.begin();
                auto it_enemyID = enemyTank.enamyTankID.begin();
                auto it_enemyHP = enemyTank.enamyTankHP.begin();
                bool flag = false;
                while (it_enemy != enemyTank.enamyTankSprite.end()) {
                    auto it_missile = ammo.missile.begin();
                    auto it_missileID = ammo.missile_id.begin();
                    auto it_missileFrotate = ammo.missileFrotate.begin();
                    auto it_missileTarget = ammo.missileTarget.begin();
                    int i = 0;
                    while (it_missile != ammo.missile.end()) {
                        if ((it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds()) && (*it_missileID == "player" || *it_enemyID != *it_missileID))
                            || (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player")) {
                            if (it_enemy->getGlobalBounds().intersects(it_missile->getGlobalBounds())
                                && (*it_missileID == "player" || *it_enemyID != *it_missileID)) {
                                *it_enemyHP -= 30;
                                if (*it_enemyHP < 0) {
                                    enemyTank.destroyTank(*it_enemyID);
                                    ux_ui.score += 50;
                                    int Rand = rand() % 50;
                                    bool flag_drop = false;
                                    for (int i = 2; i < Rand; i++) {
                                        if (Rand % i == 0)
                                            flag_drop = true;
                                    }
                                    if (!flag_drop)
                                        item.dropItem(it_enemy->getPosition());
                                    it_enemyID = enemyTank.enamyTankID.erase(it_enemyID);
                                    it_enemy = enemyTank.enamyTankSprite.erase(it_enemy);
                                    it_enemyHP = enemyTank.enamyTankHP.erase(it_enemyHP);
                                    flag = true;
                                }
                            }
                            else if (tank.getBox().intersects(it_missile->getGlobalBounds()) && *it_missileID != "player") {
                                tank.hp -= 30;
                                if (tank.hp <= 0) {
                                    ux_ui.deathScreen();
                                    if (Keyboard::isKeyPressed(Keyboard::Space)) {
                                        ux_ui.updateData();
                                        window.close();
                                        return;
                                    }
                                    if (Keyboard::isKeyPressed(Keyboard::Q)) {
                                        ux_ui.updateData();
                                        *flagGameStart = false;
                                        return;
                                        window.close();
                                    }
                                    continue;
                                }
                            }
                            sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_L.play();
                            sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_R.play();
                            it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                            it_missile = ammo.missile.erase(it_missile);
                            it_missileID = ammo.missile_id.erase(it_missileID);
                            it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                            i--;
                            if (flag)
                                break;
                        }
                        else {
                            it_missile++;
                            it_missileID++;
                            it_missileFrotate++;
                            it_missileTarget++;
                            i++;
                        }
                    }
                    if (flag)
                        break;
                    it_enemy++;
                    it_enemyID++;
                    it_enemyHP++;
                }
                auto it_missile = ammo.missile.begin();
                auto it_missileID = ammo.missile_id.begin();
                auto it_missileFrotate = ammo.missileFrotate.begin();
                auto it_missileTarget = ammo.missileTarget.begin();
                flag = false;
                while (it_missile != ammo.missile.end()) {
                    auto it_bullet = ammo.bullet.begin();
                    
                    while (it_bullet != ammo.bullet.end()) {
                        if (*it_missileID != "player" && it_missile->getGlobalBounds().intersects(it_bullet->getGlobalBounds())) {
                            it_bullet = ammo.bullet.erase(it_bullet);
                            sounds.soundExplosion_L.setPosition(findPositionLeftSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_L.play();
                            sounds.soundExplosion_R.setPosition(findPositionRightSound(it_missile->getPosition().x, window.getSize().x), 0, 0);
                            sounds.soundExplosion_R.play();
                            it_missileFrotate = ammo.missileFrotate.erase(it_missileFrotate);
                            it_missile = ammo.missile.erase(it_missile);
                            it_missileID = ammo.missile_id.erase(it_missileID);
                            it_missileTarget = ammo.missileTarget.erase(it_missileTarget);
                            flag = true;
                            break;
                        }
                        it_bullet++;
                        
                    }
                    if (flag)
                        break;
                    it_missile++;
                    it_missileID++;
                    it_missileFrotate++;
                    it_missileTarget++;
                }
            }
        }
        window.display();
    }
}