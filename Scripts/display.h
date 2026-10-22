#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL.h>
#include "chip.h"

bool init_display(SDL_Window** window, SDL_Renderer** renderer, SDL_Texture** texture);
void Draw_Screen(const Chip8* chip, SDL_Renderer* renderer, SDL_Texture* texture);
void close_display(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* texture);

#endif