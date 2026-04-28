#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class Ocean {
public:
    Ocean();

    void update();
    void draw(sf::RenderWindow& window);

private:
    struct Bubble {
        float x, y, speed;
    };

    std::vector<Bubble> bubbles;
};
