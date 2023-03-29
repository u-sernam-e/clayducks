#include "coordinator.h"
#include "rayextended.h"
#include <vector>
#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif

GameData G;

void updateDrawFrame()
{
	//UPDATE
	update(G);
	//DRAW
	BeginDrawing();

	ClearBackground(RED);
	draw(G);
	
	EndDrawing();
}

int main()
{
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);

	InitWindow(1000, 500, "doin ur mom");
    InitAudioDevice();
	initialize(G);

#if defined(PLATFORM_WEB)
	emscripten_set_main_loop(updateDrawFrame, 0, 1);
#else
	while (!WindowShouldClose() || IsKeyPressed(KEY_ESCAPE))
	{
		updateDrawFrame();
	}
#endif
	return 0;
}