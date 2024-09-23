#pragma once
#include <raylib.h>
#include <common.h>

class TitleAnimation {
	private:
		Color bottomCircleColor = (Color){20, 160, 133, 255};
    	Color topCircleColor = (Color){38, 185, 154, 255};
		Color middleCircleColor = (Color){129, 204, 184, 255};

		bool renderTopCircle = true;
		bool renderBottomCircle = true;
    	float topCircleRadius = 0.0f;
		float middleCircleRadius = 0.0f;
    	float bottomCircleRadius = 0.0f;
    	const float maxCircleRadius = GetRenderWidth() * 1.3f;
    	float elapsedTime = 0.0f;
   	 	const float circleGrowthSpeed = 100.0f;
		Color black = BLACK;
	public:	
		float screenHeight, screenWidth;
		int done;

		int UpdateAnimationData();
		void DrawAnimation();
};

class TitleScreen {
	public:
		bool isntanimating = false;
		float rotation = 0.0f;
	    bool up = true;
	    float radius = 0.0f;
		Color black = BLACK;
		Image img = LoadImage("./exit.png");
	    Texture2D txt = LoadTextureFromImage(img);
	    Image ig = LoadImage("./play.png");
	    Texture2D play = LoadTextureFromImage(ig);
	    float size = 0.25f;
	    float scaledWidth = play.width * size;
	    float scaledHeight = play.height * size;

	    Rectangle dest = { 
	        (screen_width - scaledWidth) / 2.0f,  // x
	        (screen_height - scaledHeight) / 2.0f, // y
	        scaledWidth, 
	        scaledHeight 
	    };

	    Rectangle source = { 0, 0, (float)play.width, (float)play.height };

    	Vector2 origin = { scaledWidth / 2.0f, scaledHeight / 2.0f };
		TitleAnimation anim;

		void Draw();
		void Update();
};