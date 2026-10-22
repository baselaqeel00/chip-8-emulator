#include <stdbool.h>
#include "chip8.h"
#include "display.h"

int main(int argc, char* argv[]) {
    Chip8 chip;
    chip8_init(&chip);

    // يمكنك تغيير مسار الـ ROM هنا
    load_rom(&chip, "C:/Users/VICTUS/Desktop/uni project/Chip8_Project/IBMLogo.ch8");

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

    if (!init_display(&window, &renderer, &texture)) {
        return 1;
    }

    bool isRunning = true;

    while (isRunning) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = false;
            }
        }

        for (int i = 0; i < 10; i++) {
            chip8_cycle(&chip);
        }

        Draw_Screen(&chip, renderer, texture);
        SDL_Delay(2);
    }

    close_display(window, renderer, texture);
    return 0;
}