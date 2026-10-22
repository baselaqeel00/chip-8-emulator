#include "chip.h"
#include <stdio.h>

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

void chip8_init(Chip8* chip) {
    *chip = (Chip8){ 0 };
    chip->PC = 0x200;

    for (int i = 0; i < 80; i++) {
        chip->Memory[0x050 + i] = fontset[i];
    }
}

void load_rom(Chip8* ch, const char* filename) {
    FILE* rom = fopen(filename, "rb");
    if (rom == NULL) {
        printf("Error: Cannot open ROM file %s\n", filename);
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
        else if (opcode == 0x00EE) {
            ch->sp--;
            ch->PC = ch->stack[ch->sp];
        }
        break;

    case 0x1000:
        ch->PC = NNN;
        break;

    case 0x2000:
        ch->stack[ch->sp] = ch->PC;
        ch->sp++;
        ch->PC = NNN;
        break;

    case 0x3000:
        if (ch->V[X] == NN) {
            ch->PC += 2;
        }
        break;

    case 0x4000:
        if (ch->V[X] != NN) {
            ch->PC += 2;
        }
    break;

    case 0x5000:
        if(N == 0x0){
            if (ch->V[X] == ch->V[Y]) {
                ch->PC += 2;
            }
        }
        break;

    case 0x6000:
        ch->V[X] = NN;
        break;

    case 0x7000:
        ch->V[X] = ch->V[X] + NN;
        break;

    case 0x8000:
        if (N == 0x1) {
            ch->V[X] = ch->V[Y] | ch->V[X];
        }
        if (N == 0x2) {
            ch->V[X] = ch->V[Y] & ch->V[X];
        }
        if (N == 0x3) {
            ch->V[X] = ch->V[Y] ^ ch->V[X];
        }
        if (N == 0x4) {
            uint16_t sum = ch->V[X] + ch->V[Y];

            if (sum > 255) {
                ch->V[0xF] = 1;
            }
            else {
                ch->V[0xF] = 0;
            }

            ch->V[X] = sum & 0xFF;
        }
        if (N == 0x5) {
            ch->V[X] = ch->V[X] - ch->V[Y];
        }
        if (N == 0x7) {
            ch->V[X] = ch->V[Y] - ch->V[X];
        }
        if (N == 0x6) {
            uint8_t lsb = ch->V[X] & 0x1;
            ch->V[X] >>= 1;
            ch->V[0xF] = lsb;
        }
        if (N == 0xE) {
            uint8_t msb = (ch->V[X] & 0x80) >> 7;
            ch->V[X] <<= 1;
            ch->V[0xF] = msb;
        }
        break;

    case 0x9000:
        if (N == 0) {
            if (ch->V[X] != ch->V[Y]) {
                ch->PC += 2;
            }
        }
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
        printf("Unimplemented Opcode: 0x%X\n", opcode);
        break;
    }
}