#include "coordinator.h"
#include "rayextended.h"
#include <vector>
#if defined(PLATFORM_WEB)
	#include <emscripten/emscripten.h>
#endif
// make it so that the ball doesn't get launched when you press resume from the pause menu
GameData G;

void updateDrawFrame()
{
	//UPDATE
	update(G);
	//DRAW
	BeginDrawing();

	ClearBackground(WHITE);
	draw(G);
	
	EndDrawing();
}

int main()
{
	InitWindow(1000, 500, "ClayDucks");
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