#include <title.h>
#include <common.h>

int TitleAnimation::UpdateAnimationData(){
	elapsedTime += GetFrameTime() * 4;
	topCircleRadius = elapsedTime * (circleGrowthSpeed * 1.5);

	if(!renderTopCircle && renderBottomCircle)
		bottomCircleRadius = elapsedTime * circleGrowthSpeed;
	if(!renderTopCircle && !renderBottomCircle)
		middleCircleRadius = (elapsedTime) * (circleGrowthSpeed / 4);
    if (renderTopCircle && topCircleRadius > maxCircleRadius) {
		elapsedTime = 0;
		renderTopCircle = false; 
		black = topCircleColor;
	}

	if(!renderTopCircle && !renderBottomCircle && middleCircleRadius > 150.0f){
		middleCircleRadius = 150.0f;
		return 1;
	}

    if (!renderTopCircle && renderBottomCircle && bottomCircleRadius > (maxCircleRadius / 1.5)) {
		elapsedTime = 0;
		renderBottomCircle = false;
		bottomCircleRadius = maxCircleRadius;
	}
	
	return 0;
}

void TitleAnimation::DrawAnimation(){
	ClearBackground(black);
	if(!renderTopCircle && !renderBottomCircle) {
		ClearBackground(bottomCircleColor);
        DrawRectangle(screen_width / 2, 0, screen_width / 2, screen_height, topCircleColor);
	} else {
		if(!renderTopCircle && renderBottomCircle){
    	    Vector2 bottomCirclePosition = (Vector2){0, screenHeight};
    		DrawCircleV(bottomCirclePosition, bottomCircleRadius, bottomCircleColor);
		} else if(!renderTopCircle && !renderBottomCircle) {
			DrawRectangle(0 , 0, 500 , 800, bottomCircleColor);
		}
		if(renderTopCircle){
		    Vector2 topCirclePosition = (Vector2){screenWidth, screenHeight};
    	    DrawCircleV(topCirclePosition, topCircleRadius, topCircleColor);
		} else {
			DrawRectangle(screenWidth / 2, 0, 800, 900, topCircleColor);
		}
	}
	if(!renderTopCircle && !renderBottomCircle){
		DrawCircle(screenWidth / 2, screenHeight / 2, middleCircleRadius, middleCircleColor);
	}
    DrawLine(screenWidth / 2, 0, screenWidth / 2, middleCircleRadius * 4, WHITE);
}

void TitleScreen::Update() {
	return;
}

void TitleScreen::Draw() {
	DrawTable(NORMAL);
}
