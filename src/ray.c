/*******************************************************************************************
*
*   raylib [core] example - Basic window
*
*   Welcome to raylib!
*
*   To test examples, just press F6 and execute 'raylib_compile_execute' script
*   Note that compiled executable is placed in the same folder as .c file
*
*   To test the examples on Web, press F6 and execute 'raylib_compile_execute_web' script
*   Web version of the program is generated in the same folder as .c file
*
*   You can find all basic examples on C:\raylib\raylib\examples folder or
*   raylib official webpage: www.raylib.com
*
*   Enjoy using raylib. :)
*
*   Example originally created with raylib 1.0, last time updated with raylib 1.0
*
*   Example licensed under an unmodified zlib/libpng license, which is an OSI-certified,
*   BSD-like license that allows static linking with closed source software
*
*   Copyright (c) 2013-2024 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dlfcn.h>


typedef struct ColorStruct {
    char r;
    char g;
    char b;
    char a;
} ColorStruct;


void (*BeginDrawing) (void);
void (*EndDrawing) (void);
void (*CloseWindow) (void);
void (*InitWindow) (int, int, char*);
bool (*WindowShouldClose) (void);
void (*ClearBackground) (ColorStruct);
void (*DrawText) (char*, int, int, int, ColorStruct);


int main(void){

    // Ref: https://linux.die.net/man/3/dlopen 
    char *error;
    char *plugin_name;
    char* file_name = "raylib-5.5_linux_amd64/lib/libraylib.so";
    void *plugin;
    plugin = dlopen(file_name, RTLD_NOW);
    if (!plugin)
    {
        fprintf(stderr, "Cannot load %s: %s", plugin_name, dlerror());
        exit(EXIT_FAILURE);
    }
    dlerror();

    *(void **) (&BeginDrawing) = dlsym(plugin, "BeginDrawing");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    *(void **) (&EndDrawing) = dlsym(plugin, "EndDrawing");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    *(void **) (&CloseWindow) = dlsym(plugin, "CloseWindow");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    *(void **) (&InitWindow) = dlsym(plugin, "InitWindow");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    *(void **) (&WindowShouldClose) = dlsym(plugin, "WindowShouldClose");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    *(void **) (&ClearBackground) = dlsym(plugin, "ClearBackground");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    *(void **) (&DrawText) = dlsym(plugin, "DrawText");
    if ((error = dlerror()) != NULL)  {
        fprintf(stderr, "%s\n", error);
        exit(EXIT_FAILURE);
    }

    const int screen_width = 800;
    const int screen_height = 450;

    InitWindow(screen_width, screen_height, "test");

    while (!WindowShouldClose())
    {
        BeginDrawing();

            ClearBackground((ColorStruct){0xF1, 0x09, 0x09, 0xFF});

            char *msg = "Hello from PickleLisp";
            size_t msg_len = strlen(msg);
            int font_sz = 20;
            DrawText(msg, (screen_width - msg_len * font_sz / 2) / 2, 200, font_sz, (ColorStruct){0xF2, 0xF2, 0xF2, 0xFF});

        EndDrawing();
    }

    CloseWindow();

    dlclose(plugin);
    return 0;
}