#include <title.h>
#include <common.h>
#define midwidth GetRenderWidth() / 2
#define midheight GetRenderHeight() / 2

int TitleAnimation::UpdateAnimationData(){
	elapsedTime += GetFrameTime() * 4;
	topCircleRadius = elapsedTime * (circleGrowthSpeed * 2);

	if(!renderTopCircle && renderBottomCircle)
		bottomCircleRadius = elapsedTime * (circleGrowthSpeed * 2);
	if(!renderTopCircle && !renderBottomCircle)
		middleCircleRadius = (elapsedTime) * (circleGrowthSpeed / 2);
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
	DrawLine(screenWidth / 2, 0, screenWidth / 2, middleCircleRadius * 4, WHITE);
	if(!renderTopCircle && !renderBottomCircle){
		DrawCircle(screenWidth / 2, screenHeight / 2, middleCircleRadius + 1, WHITE);
		DrawCircle(screenWidth / 2, screenHeight / 2, middleCircleRadius, middleCircleColor);

		DrawCircle(0, 0, middleCircleRadius >= 151 ? 151 : middleCircleRadius + 1 , WHITE);
        DrawCircle(0, 0, middleCircleRadius >= 150 ? 150 : middleCircleRadius + 1, (Color){129, 204, 184, 255});
        DrawCircle(GetRenderWidth(), GetRenderHeight(), middleCircleRadius >= 101 ? 101 : middleCircleRadius + 1, WHITE);
        DrawCircle(GetRenderWidth(), GetRenderHeight(), middleCircleRadius >= 100 ? 100 : middleCircleRadius, (Color){129, 204, 184, 255});
	}
}

void TitleScreen::Update() {
	Vector2 pos = GetMousePosition();
	bool coll = CheckCollisionPointCircle(pos, { screen_width, screen_height }, 100);
	if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && coll){
		CloseWindow();
	}
    bool coll2 = CheckCollisionPointCircle(pos, { screen_width / 2, screen_height / 2 }, 150);
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && coll2 && !isntanimating){
        isntanimating = true;
    }
    if(isntanimating){
        if(radius != 150 && up)
            radius += 1;
        else if(radius == 150 && up)
            up = false;
        if(up == false && radius != 30){
            radius -= 1;
            if(black.g < 255){
                black.g += 4;
                black.r += 4;
            }else {
                black.g = 255;
                black.r = 255;
            }
            
        }
    }
    if(IsKeyDown(KEY_R)){
        size = 0.25f;
        rotation = 0.0f;
        up = true;
        radius = 0.0f;
    }
	return;
}

void TitleScreen::Draw() {
	DrawTable(NORMAL);

	DrawCircle(0, 0, 151, WHITE);
    DrawCircle(0, 0, 150, (Color){129, 204, 184, 255});
    DrawCircle(GetRenderWidth(), GetRenderHeight(), 101, WHITE);
    DrawCircle(GetRenderWidth(), GetRenderHeight(), 100, (Color){129, 204, 184, 255});
                    
	DrawTextureEx(txt, {1220, 740}, 0.0f, 0.07f, WHITE);
    if(radius == 0.0f)
        DrawTextureEx(play, { 597, 335 }, rotation, size, WHITE);
	//DrawTexturePro(play, source, { 597, 335 }, { midwidth, midheight }, rotation, WHITE);
	else 
        DrawCircle(midwidth, midheight, radius, black);
}
