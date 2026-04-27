#include "Ocean.hpp"
#include <cstdlib>

Ocean::Ocean() {
    for (int i = 0; i < 50; i++) {
        bubbles.push_back({
            float(rand() % 1000),
            float(rand() % 700),
            float(0.5 + rand() % 2)
        });
    }
}

void Ocean::update() {
    for (auto &b : bubbles) {
        b.y -= b.speed;
        if (b.y < 0) b.y = 700;
    }
}

void Ocean::draw(sf::RenderWindow& window) {
    for (auto &b : bubbles) {
        sf::CircleShape shape(3);
        shape.setPosition(b.x, b.y);
        shape.setFillColor(sf::Color(200, 200, 255, 150));
        window.draw(shape);
    }
}
