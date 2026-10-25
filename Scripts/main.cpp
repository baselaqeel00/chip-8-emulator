#define SDL_MAIN_HANDLED
#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <SDL.h>
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"

extern "C" {
#include "chip.h"
#include "display.h"
}

void open_url(const char* url) {
#ifdef _WIN32
    std::string command = std::string("start ") + url;
    system(command.c_str());
#endif
}

void audio_callback(void* userdata, Uint8* stream, int len) {
    Sint16* buffer = (Sint16*)stream;
    int length = len / sizeof(Sint16);
    static double sample_index = 0.0;

    const double frequency = 440.0;
    const double sample_rate = 44100.0;
    const Sint16 amplitude = 3000; 

    for (int i = 0; i < length; i++) {
        double period = sample_rate / frequency;
        buffer[i] = (fmod(sample_index, period) < (period / 2.0)) ? amplitude : -amplitude;
        sample_index++;
    }
}

void handle_input(Chip8* chip, bool* isRunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL2_ProcessEvent(&event);

        if (event.type == SDL_QUIT) {
            *isRunning = false;
        }

        ImGuiIO& io = ImGui::GetIO();
        if (io.WantCaptureKeyboard) {
            continue;
        }

        if (event.type == SDL_KEYDOWN) {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                *isRunning = false;
            }

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
    if (chip->delay_timer > 0) chip->delay_timer--;
    if (chip->sound_timer > 0) chip->sound_timer--;
}

int main(int argc, char* argv[]) {
    srand((unsigned int)time(NULL));

    Chip8 chip;
    chip8_init(&chip);

    std::vector<const char*> rom_list = {
        "Roms/IBMLogo.ch8",
        "Roms/Tetris.ch8",
        "Roms/Pong [Paul Vervalin, 1990].ch8",
        "Roms/Tank.ch8",
        "Roms/Space Invaders [David Winter] (alt).ch8"
    };
    int current_rom_idx = 0;

    load_rom(&chip, rom_list[current_rom_idx]);

    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* texture = NULL;

    if (!init_display(&window, &renderer, &texture)) {
        return 1;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer2_Init(renderer);

    bool isRunning = true;
    bool isPaused = false;
    int cycles_per_frame = 11;

    if (SDL_InitSubSystem(SDL_INIT_AUDIO) < 0) {
        printf("SDL Audio Initialization Error: %s\n", SDL_GetError());
    }

    SDL_AudioSpec desired_spec, obtained_spec;
    SDL_zero(desired_spec);
    desired_spec.freq = 44100;         
    desired_spec.format = AUDIO_S16SYS; 
    desired_spec.channels = 1;         
    desired_spec.samples = 2048;       
    desired_spec.callback = audio_callback;

    SDL_AudioDeviceID audio_device = SDL_OpenAudioDevice(NULL, 0, &desired_spec, &obtained_spec, 0);

    if (audio_device == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
    }

    while (isRunning) {
        Uint32 frame_start = SDL_GetTicks();

        handle_input(&chip, &isRunning);

        if (!isPaused) {
            for (int i = 0; i < cycles_per_frame; i++) {
                chip8_cycle(&chip);
            }
            update_timers(&chip);
        }

        if (chip.sound_timer > 0) {
            SDL_PauseAudioDevice(audio_device, 0);
        }
        else {
            SDL_PauseAudioDevice(audio_device, 1);
        }

        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Bassel's CHIP-8 Control Panel");

        ImGui::TextColored(ImVec4(0.2f, 0.8f, 1.0f, 1.0f), "Welcome to Bassel's CHIP-8 Emulator!");
        ImGui::Separator();
        ImGui::Spacing();

        if (ImGui::Button("GitHub Profile")) {
            open_url("https://github.com/baselaqeel00");
        }
        ImGui::SameLine();
        if (ImGui::Button("LinkedIn Profile")) {
            open_url("https://www.linkedin.com/in/bassel-aqeel/");
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("ROM Selection:");

        if (ImGui::Combo("Select ROM", &current_rom_idx, rom_list.data(), (int)rom_list.size())) {
            chip8_init(&chip);
            load_rom(&chip, rom_list[current_rom_idx]);
        }

        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Text("Controls & Settings:");

        if (ImGui::Button(isPaused ? "Resume" : "Pause")) {
            isPaused = !isPaused;
        }
        ImGui::SameLine();

        if (ImGui::Button("Reset Game")) {
            chip8_init(&chip);
            load_rom(&chip, rom_list[current_rom_idx]);
        }

        ImGui::SliderInt("Cycles/Frame", &cycles_per_frame, 1, 30);

        ImGui::End();

        ImGui::Render();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        Draw_Screen(&chip, renderer, texture);

        ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer);
        SDL_RenderPresent(renderer);

        Uint32 frame_time = SDL_GetTicks() - frame_start;
        const Uint32 TARGET_FRAME_TIME = 1000 / 60;

        if (frame_time < TARGET_FRAME_TIME) {
            SDL_Delay(TARGET_FRAME_TIME - frame_time);
        }
    }


    if (audio_device != 0) {
        SDL_CloseAudioDevice(audio_device);
    }

    ImGui_ImplSDLRenderer2_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    close_display(window, renderer, texture);
    return 0;
}