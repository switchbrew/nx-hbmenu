#include <SFML/Graphics.hpp>
#include <string.h>
#include <math.h>

extern "C" {

#include "../common/common.h"

}

color_t pixels[720][1280];

int main()
{
    sf::RenderWindow window(sf::VideoMode(1280, 720), "Test");
    window.setFramerateLimit(60);

    menuStartup();

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
                break;
            }
        }

        memset(pixels, 237, sizeof(pixels));

        if (!uiUpdate()) break;
        menuLoop();
        window.clear();

        sf::Image image;
        image.create(1280, 720, (const unsigned char*)pixels);

        sf::Texture texture;
        texture.loadFromImage(image);

        sf::Sprite sprite;
        sprite.setTexture(texture);

        window.draw(sprite);
        window.display();
    }

    return 0;
}

extern "C" bool menuUpdate(void) {
    return 0;
}
