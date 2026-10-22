#include <stdio.h>
#include <stdbool.h>
#include "chip.h"
#include "display.h"

int main(int argc, char* argv[]) {
    printf("=== Starting CHIP-8 Emulator ===\n");

    Chip8 chip;
    chip8_init(&chip);

    const char* rom_path = "C:/Users/VICTUS/Desktop/uni project/Chip8_Project/IBMLogo.ch8";
    load_rom(&chip, rom_path);

    printf("First Opcode in Memory: 0x%02X%02X\n", chip.Memory[0x200], chip.Memory[0x201]);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

    if (!init_display(&window, &renderer, &texture)) {
        printf("Failed to initialize display!\n");
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