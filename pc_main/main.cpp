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

    themeStartup(THEME_PRESET_LIGHT);
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
    //This is implemented here due to the hid code.
    menu_s* menu = menuGetCurrent();

    if (menu->nEntries > 0)
    {
        int move = 0;

        static int left_state = 0;
        int new_left_state = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
        if (!new_left_state && left_state)
            move--;
        left_state = new_left_state;

        static int right_state = 0;
        int new_right_state = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
        if (!new_right_state && right_state)
            move++;
        right_state = new_right_state;

        int newEntry = menu->curEntry + move;
        if (newEntry < 0) newEntry = 0;
        if (newEntry >= menu->nEntries) newEntry = menu->nEntries-1;
        menu->curEntry = newEntry;
    }

    return 0;
}
