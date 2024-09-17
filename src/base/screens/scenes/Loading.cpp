#include <loading.h>
#include <colors.h>

// Define the UpdateAnimationData method
int LoadingScreenAnim::UpdateAnimationData() {
    if (alpha < 1.0f && !fadeout) alpha += (alphaSpeed /2);
    else if(alpha < 0.1f && fadeout) return 1;
    else alpha -= (alphaSpeed /2);

    if (logoScale < 0.6f) logoScale += (scaleSpeed /2);
    else {
        fadeout = true;
        logoScale += scaleSpeed * 2;
    }

    logoPosition.x = screenWidth / 2.0f - (logo.width * logoScale) / 2;
    logoPosition.y = screenHeight / 2.0f - (logo.height * logoScale) / 2;
    return 0;
}

// Define the DrawAnimation method
void LoadingScreenAnim::DrawAnimation() {
    ClearBackground(BLACK);
    DrawTextureEx(logo, logoPosition, 0.0f, logoScale, CalculateAlpha(alpha));
}
