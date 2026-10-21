#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

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

}Chip8; 

void chip8_init(Chip8 *chip) {
    *chip = (Chip8){0};
    chip->PC = 0x200; 
}

void chip8_cycle(Chip8 *chip){

}

void Draw_Screen( const Chip8 *chip){

}

int main(void) {
    Chip8 chip;
    chip8_init(&chip);

    bool isRunning = true;
    
    while(isRunning){
        //Read the Key Bord 
        chip8_cycle(&chip);
        Draw_Screen(&chip);
    }

    printf("Hello, World!\n");
    return 0;
}