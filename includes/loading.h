#ifndef LOADINGSCREEN_H
#define LOADINGSCREEN_H

#include <raylib.h>
#include <webwise.h>

class LoadingScreenAnim {
private:
    Image image = {
        .data = WEBWISE_DATA,
	    .width = WEBWISE_WIDTH,
	    .height = WEBWISE_HEIGHT,
	    .mipmaps = 1,
	    .format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8
    };
   	Texture2D logo = LoadTextureFromImage(image);
	bool fadeout = false;
    float logoScale = 0.0f;
    Vector2 logoPosition = { screenWidth / 2.0f, screenHeight / 2.0f };
    float alpha = 0.0f;
	float scaleSpeed = 0.005f;
	float alphaSpeed = 0.01f;
public:
	int screenWidth, screenHeight;

    // Update animation data
    int UpdateAnimationData();

    // Draw the animation
    void DrawAnimation();
};

#endif // LOADINGSCREEN_H
