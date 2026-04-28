#include "Minimap.hpp"
#include <iostream>

Minimap::Minimap() {
    if (!mapTexture.loadFromFile("assets/map.png")) {
        std::cout << "Error loading map\n";
    }

    mapSprite.setTexture(mapTexture);

    // scale to minimap size
    mapSprite.setScale(0.2f, 0.2f);

    // position top-right
    mapSprite.setPosition(800, 20);

    // player dot
    playerDot.setRadius(5);
    playerDot.setFillColor(sf::Color::Red);
}

void Minimap::update(float playerX, float playerY) {
    float miniX = (playerX / 1000.0f) * 150 + 800;
    float miniY = (playerY / 700.0f) * 150 + 20;

    playerDot.setPosition(miniX, miniY);
}

void Minimap::draw(sf::RenderWindow& window) {
    window.draw(mapSprite);
    window.draw(playerDot);
}
