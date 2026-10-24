#include <stdio.h>
#include <stdbool.h>
#include "chip.h"
#include "display.h"

void handle_input(Chip8* chip, bool* isRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            *isRunning = false;
        }

        if (event.type == SDL_KEYDOWN) {
            switch (event.key.keysym.sym) {
            case SDLK_1: chip->keypad[0x1] = 1; break;
            case SDLK_2: chip->keypad[0x2] = 1; break;
            case SDLK_3: chip->keypad[0x3] = 1; break;
            case SDLK_4: chip->keypad[0xC] = 1; break;

            case SDLK_q: chip->keypad[0x4] = 1; break;
            case SDLK_w: chip->keypad[0x5] = 1; break;
            case SDLK_e: chip->keypad[0x6] = 1; break;
            case SDLK_r: chip->keypad[0xD] = 1; break;

            case SDLK_a: chip->keypad[0x7] = 1; break;
            case SDLK_s: chip->keypad[0x8] = 1; break;
            case SDLK_d: chip->keypad[0x9] = 1; break;
            case SDLK_f: chip->keypad[0xE] = 1; break;

            case SDLK_z: chip->keypad[0xA] = 1; break;
            case SDLK_x: chip->keypad[0x0] = 1; break;
            case SDLK_c: chip->keypad[0xB] = 1; break;
            case SDLK_v: chip->keypad[0xF] = 1; break;
            }
        }

        if (event.type == SDL_KEYUP) {
            switch (event.key.keysym.sym) {
            case SDLK_1: chip->keypad[0x1] = 0; break;
            case SDLK_2: chip->keypad[0x2] = 0; break;
            case SDLK_3: chip->keypad[0x3] = 0; break;
            case SDLK_4: chip->keypad[0xC] = 0; break;

            case SDLK_q: chip->keypad[0x4] = 0; break;
            case SDLK_w: chip->keypad[0x5] = 0; break;
            case SDLK_e: chip->keypad[0x6] = 0; break;
            case SDLK_r: chip->keypad[0xD] = 0; break;

            case SDLK_a: chip->keypad[0x7] = 0; break;
            case SDLK_s: chip->keypad[0x8] = 0; break;
            case SDLK_d: chip->keypad[0x9] = 0; break;
            case SDLK_f: chip->keypad[0xE] = 0; break;

            case SDLK_z: chip->keypad[0xA] = 0; break;
            case SDLK_x: chip->keypad[0x0] = 0; break;
            case SDLK_c: chip->keypad[0xB] = 0; break;
            case SDLK_v: chip->keypad[0xF] = 0; break;
            }
        }
    }
}

void update_timers(Chip8* chip) {
    if (chip->delay_timer > 0) {
        chip->delay_timer--;
    }
    if (chip->sound_timer > 0) {
        chip->sound_timer--;
    }
}

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
        handle_input(&chip, &isRunning);

        for (int i = 0; i < 10; i++) {
            chip8_cycle(&chip);
        }

        update_timers(&chip);

        Draw_Screen(&chip, renderer, texture);
        SDL_Delay(2);
    }

    close_display(window, renderer, texture);
    return 0;
}