#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL.h>

const uint8_t fontset[80] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

typedef struct {
    uint8_t Memory[4096];
    uint8_t V[16];
    uint8_t sp;
    uint8_t delay_timer;
    uint8_t sound_timer;
    uint8_t keypad[16];

    uint16_t I;
    uint16_t PC;
    uint16_t stack[16];

    uint32_t gfx[64 * 32];
} Chip8;

void chip8_init(Chip8* chip) {
    *chip = (Chip8){ 0 };
    chip->PC = 0x200;

    for (int i = 0; i < 80; i++) {
        chip->Memory[0x050 + i] = fontset[i];
    }
}

void load_rom(Chip8* ch) {
    FILE* rom = fopen("C:/Users/VICTUS/Desktop/uni project/Chip8_Project/IBMLogo.ch8", "rb");
    if (rom == NULL) {
        printf("There is no rom \n");
        return;
    }

    fread(&ch->Memory[0x200], 1, sizeof(ch->Memory) - 0x200, rom);
    fclose(rom);
}

void ClearScreen(Chip8* ch) {
    for (int i = 0; i < 64 * 32; i++) ch->gfx[i] = 0;
}

void chip8_cycle(Chip8* ch) {
    uint16_t opcode = (ch->Memory[ch->PC] << 8) | ch->Memory[ch->PC + 1];
    ch->PC += 2;

    uint16_t NNN = opcode & 0x0FFF;
    uint8_t  NN = opcode & 0x00FF;
    uint8_t  N = opcode & 0x000F;

    uint8_t  X = (opcode >> 8) & 0x000F;
    uint8_t  Y = (opcode >> 4) & 0x000F;

    switch (opcode & 0xF000) {
    case 0x0000:
        if (opcode == 0x00E0) {
            ClearScreen(ch);
        }
        break;

    case 0x1000:
        ch->PC = NNN;
        break;

    case 0x6000:
        ch->V[X] = NN;
        break;

    case 0x7000:
        ch->V[X] = ch->V[X] + NN;
        break;

    case 0xA000:
        ch->I = NNN;
        break;

    case 0xD000: {
        uint8_t x_pos = ch->V[X] % 64;
        uint8_t y_pos = ch->V[Y] % 32;

        ch->V[0xF] = 0;

        for (int row = 0; row < N; row++) {
            uint8_t sprite_byte = ch->Memory[ch->I + row];

            for (int col = 0; col < 8; col++) {
                if ((sprite_byte & (0x80 >> col)) != 0) {
                    int px = (x_pos + col) % 64;
                    int py = (y_pos + row) % 32;
                    int index = px + (py * 64);

                    if (ch->gfx[index] == 0xFFFFFFFF) {
                        ch->V[0xF] = 1;
                    }

                    ch->gfx[index] ^= 0xFFFFFFFF;
                }
            }
        }
        break;
    }

    default:
        printf("working on it: 0x%X\n", opcode);
        break;
    }
}

void Draw_Screen(const Chip8* chip, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_UpdateTexture(texture, NULL, chip->gfx, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

int main(int argc, char* argv[]) {
    Chip8 chip;
    chip8_init(&chip);

    load_rom(&chip);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 320, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);

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

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}