#include <raylib.h>
#include <a.h>
#include <string>

void StartWindow(Image logo, std::string name){
	init();
	InitWindow(1280, 800, name.c_str());
	SetTargetFPS(144);
	//SetWindowIcon(logo);
	InitAudioDevice();
	SetExitKey(KEY_NULL);
}