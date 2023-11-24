#include "Game.h"
#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "Ux_Ui.h"
#include "Data.h"
using namespace std;
using namespace sf;

int main()
{
    bool flagGameStart = false;
    int modeWidth = 1920; int modeHeight = 1080;
    int count = 0;
    bool isFirstTime = true;
    string usernameText = "";
    while (true) {
        if (!flagGameStart) {
            RenderWindow window(VideoMode(modeWidth, modeHeight), "Cybertank", Style::Fullscreen);
            Ux_Ui ux_ui(window, modeWidth, modeHeight);
            window.setFramerateLimit(60);
            ux_ui.isFirstTime = &isFirstTime;
            ux_ui.inputText = &usernameText;
            ux_ui.loadDataToDatas();
            SoundBuffer buffer;
            if (!buffer.loadFromFile("resource/menu_soundtrack.wav"))
                return 0;
            Sound sound;
            sound.setBuffer(buffer);
            sound.setLoop(true);
            sound.play();
            while (window.isOpen()) {
                Event event;
                while (window.pollEvent(event)) {
                    if (event.type == Event::Closed || event.key.code == Keyboard::Escape) {
                        window.close();
                        return 0;
                    }
                    if (isFirstTime) {
                        if (event.key.code == Keyboard::Enter) {
                            isFirstTime = false;
                            ux_ui.usernameText.setString(usernameText);
                            FloatRect shapeBounds = ux_ui.usernameText.getLocalBounds();
                            Vector2f shapeCenter(shapeBounds.left + shapeBounds.width / 2.f, shapeBounds.top + shapeBounds.height / 2.f);
                            ux_ui.usernameText.setOrigin(shapeCenter);
                            ux_ui.newData();
                        }
                        if (event.type == Event::TextEntered) {
                            if (event.text.unicode < 128) {
                                if (event.text.unicode == '\b' && !usernameText.empty())
                                    usernameText.pop_back();
                                else
                                    usernameText += static_cast<char>(event.text.unicode);
                            }
                        }
                    }
                    else if (ux_ui.isLeaderboardShow) {
                        if (event.key.code == Keyboard::Q) 
                            ux_ui.isLeaderboardShow = false;
                    }
                    else {
                        if (event.key.code == Keyboard::Space) {
                            flagGameStart = true;
                            window.close();
                        }
                        if (event.key.code == Keyboard::E)
                            ux_ui.isLeaderboardShow = true;
                    }
                }
                if (!isFirstTime)
                    ux_ui.usernameText.setString(usernameText);
                ux_ui.startMenu(count);
                if (count == 60)
                    count = 0;
                else
                    count++;
                window.display();
            }
        }
        else {
            Game game;
            game.userName = &usernameText;
            SoundBuffer buffer;
            if (!buffer.loadFromFile("resource/main_soundtrack.wav"))
                return 0;
            Sound sound;
            sound.setBuffer(buffer);
            sound.setLoop(true);
            sound.setVolume(60.f);
            sound.play();
            game.run(modeWidth, modeHeight, &flagGameStart);
        }
    }
    return 0;
}