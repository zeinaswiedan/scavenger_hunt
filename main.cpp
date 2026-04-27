#include <SFML/Graphics.hpp>
#include "Ocean.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 700), "Ocean Scene");

    Ocean ocean;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        ocean.update();

        window.clear(sf::Color(10, 30, 80)); // ocean color
        ocean.draw(window);

        window.display();
    }
}
