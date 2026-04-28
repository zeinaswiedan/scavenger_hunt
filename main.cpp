#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <cmath>
#include "Ocean.hpp"
#include "Minimap.hpp"

// 🏝️ Location with image
struct Location {
    float x, y;
    bool visited;

    sf::Texture texture;
    sf::Sprite sprite;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Ocean Scene");

sf::Texture testTexture;
if (!testTexture.loadFromFile("assets/map.png")) {
    std::cout << "FAILED TO LOAD MAP\n";
} else {
    std::cout << "MAP LOADED\n";
}

sf::Sprite testSprite;
testSprite.setTexture(testTexture);
testSprite.setPosition(0, 0);
testSprite.setScale(0.3f, 0.3f);



    Ocean ocean;
    Minimap minimap;

    float playerX = 500;
    float playerY = 350;

    // 🏝️ Locations
    std::vector<Location> locations = {
        {100, 100, false},
        {300, 250, false},
        {800, 150, false},
        {600, 400, false},
        {200, 500, false},
        {900, 600, false},
        {450, 350, false}
    };

    // 📂 Image paths (MUST match your assets folder)
    std::vector<std::string> files = {
        "assets/pyramid.png",
        "assets/statue.png",
        "assets/spongebob.png",
        "assets/temple.png",
        "assets/submarine.png",
        "assets/octopus.png",
        "assets/compass.png"
    };

    // 🖼️ Load images into locations
    for (int i = 0; i < locations.size(); i++) {
        if (!locations[i].texture.loadFromFile(files[i])) {
            std::cout << "Error loading " << files[i] << "\n";
        }

        locations[i].sprite.setTexture(locations[i].texture);
        locations[i].sprite.setScale(0.1f, 0.1f); // adjust size if needed
    }

    // 🚢 Treasure
    float treasureX = 50;
    float treasureY = 650;
    bool treasureFound = false;

    sf::Texture shipTexture;
    if (!shipTexture.loadFromFile("assets/ship.png")) {
        std::cout << "Error loading ship\n";
    }

sf::Sprite ship;
testSprite.setTexture(shipTexture);
testSprite.setScale(0.2f, 0.2f);




    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // 🎮 movement
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) playerY -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) playerY += 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) playerX -= 1;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) playerX += 1;

        // 🧠 check hints
        for (auto &loc : locations) {
            float dx = playerX - loc.x;
            float dy = playerY - loc.y;

            if (std::sqrt(dx*dx + dy*dy) < 50 && !loc.visited) {
                loc.visited = true;
                std::cout << "Hint unlocked!\n";
            }
        }

        // 🚢 check treasure
        float dx = playerX - treasureX;
        float dy = playerY - treasureY;

        if (std::sqrt(dx*dx + dy*dy) < 50 && !treasureFound) {
            treasureFound = true;
            std::cout << "🎉 YOU FOUND THE TREASURE!\n";
        }

        ocean.update();
        minimap.update(playerX, playerY);

        window.clear(sf::Color(10, 30, 80));

        ocean.draw(window);

        // 🏝️ draw locations (images)
        for (int i = 0; i < locations.size(); i++) {
            locations[i].sprite.setPosition(locations[i].x, locations[i].y);
            window.draw(locations[i].sprite);
        }

        // 🚢 draw treasure
        ship.setPosition(treasureX, treasureY);
        window.draw(ship);



        minimap.draw(window);

        window.display();

    }

    return 0;
}
