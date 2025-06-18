#include <SDL.h>
#include <string.h>
#include <math.h>
#include <physfs.h>

extern "C" {
#include "../common/common.h"
}

color_t pixels[720][1280];

SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;
SDL_Texture* texture = NULL;

bool initSDL() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return false;

    window = SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_SHOWN);
    if (!window) return false;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) return false;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, 1280, 720);
    if (!texture) return false;

    return true;
}

void cleanupSDL() {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char **argv)
{
    if (!initSDL()) return 1;

    menuStartupPath();
    PHYSFS_init(argv[0]);
    assetsInit();
    themeStartup(THEME_PRESET_LIGHT);
    textInit();
    fontInitialize();
    netloaderInit();
    workerInit();
    statusInit();
    menuStartup();

    bool running = true;
    SDL_Event event;

    while (running)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                running = false;
                break;
            }
        }

        memset(pixels, 237, sizeof(pixels));

        if (!uiUpdate()) break;
        menuLoop();

        void* pixels_out;
        int pitch;
        SDL_LockTexture(texture, NULL, &pixels_out, &pitch);
        memcpy(pixels_out, pixels, sizeof(pixels));
        SDL_UnlockTexture(texture);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        SDL_Delay(1000 / 60); // ~60 FPS
    }

    netloaderSignalExit();
    statusExit();
    workerExit();
    netloaderExit();
    fontExit();
    assetsExit();
    PHYSFS_deinit();
    cleanupSDL();

    return 0;
}

extern "C" bool menuUpdate(void) {
    menu_s* menu = menuGetCurrent();

    const Uint8* keystate = SDL_GetKeyboardState(NULL);

    static int esc_state = 0;
    int new_esc_state = keystate[SDL_SCANCODE_ESCAPE];

    static int return_state = 0;
    int new_return_state = keystate[SDL_SCANCODE_RETURN];

    static int x_state = 0;
    int new_x_state = keystate[SDL_SCANCODE_X];

    static int y_state = 0;
    int new_y_state = keystate[SDL_SCANCODE_Y];

    static int t_state = 0;
    int new_t_state = keystate[SDL_SCANCODE_T];

    if (!new_y_state && y_state)
        launchMenuNetloaderTask();

    if (!new_x_state && x_state)
        menuHandleXButton();

    if (!new_esc_state && esc_state)
        launchMenuBackTask();
    else if (!new_t_state && t_state)
        themeMenuStartup();
    else if (!new_return_state && return_state)
        menuHandleAButton();
    else if (menu->nEntries > 0)
    {
        int move = 0;

        static int left_state = 0;
        int new_left_state = keystate[SDL_SCANCODE_LEFT];
        if (!new_left_state && left_state)
            move--;

        static int right_state = 0;
        int new_right_state = keystate[SDL_SCANCODE_RIGHT];
        if (!new_right_state && right_state)
            move++;

        left_state = new_left_state;
        right_state = new_right_state;

        int newEntry = menu->curEntry + move;
        if (newEntry < 0) newEntry = 0;
        if (newEntry >= menu->nEntries) newEntry = menu->nEntries - 1;
        menu->curEntry = newEntry;
    }

    esc_state = new_esc_state;
    return_state = new_return_state;
    x_state = new_x_state;
    y_state = new_y_state;
    t_state = new_t_state;

    return 0;
}
