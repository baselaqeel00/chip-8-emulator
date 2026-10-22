#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>
#include <stdbool.h>

extern const uint8_t fontset[80];

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

void chip8_init(Chip8* chip);
void load_rom(Chip8* ch, const char* filename);
void ClearScreen(Chip8* ch);
void chip8_cycle(Chip8* ch);

#endif