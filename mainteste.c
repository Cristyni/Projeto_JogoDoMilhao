#include "raylib.h"

int main(void) {
    InitWindow(800, 450, "The Office - Show do Milhao");

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();

        ClearBackground(DARKGRAY);

        DrawText("THE OFFICE", 260, 140, 40, WHITE);
        DrawText("Show do Milhao", 285, 210, 25, GOLD);
        DrawText("Raylib funcionando!", 275, 300, 20, GREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}