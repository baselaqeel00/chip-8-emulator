#include "display.h"
#include <stdio.h>

bool init_display(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL Init Error: %s\n", SDL_GetError());
        return false;
    }

    *window = SDL_CreateWindow("CHIP-8 Emulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 640, SDL_WINDOW_SHOWN);
    if (!*window) return false;

    *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
    if (!*renderer) return false;

    *texture = SDL_CreateTexture(*renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
    if (!*texture) return false;

    return true;
}

void Draw_Screen(const Chip8* chip, SDL_Renderer* renderer, SDL_Texture* texture) {
    SDL_UpdateTexture(texture, NULL, chip->gfx, 64 * sizeof(uint32_t));
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void close_display(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture) {
    if (texture) SDL_DestroyTexture(texture);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    SDL_Quit();
}