#pragma once
#include <raylib.h>

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
		int screenHeight, screenWidth;
		int done;

		int UpdateAnimationData();
		void DrawAnimation();
};

class TitleScreen {
	public:
		TitleAnimation anim;

		void Draw();
		void Update();
};