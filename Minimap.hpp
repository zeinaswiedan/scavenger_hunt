#pragma once
#include <SFML/Graphics.hpp>

class Minimap {
public:
    Minimap();

    void update(float playerX, float playerY);
    void draw(sf::RenderWindow& window);

private:
    sf::Texture mapTexture;
    sf::Sprite mapSprite;

    sf::CircleShape playerDot;
};
