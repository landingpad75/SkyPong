#include <raylib.h>

/// @brief 
/// @param alpha 
/// @return 
Color CalculateAlpha(float alpha) {
    Color color = WHITE;
    int transparency = alpha * 255;
    color.r = (color.r * transparency) / 255;
    color.g = (color.g * transparency) / 255;
    color.b = (color.b * transparency) / 255;
    color.a = transparency;
    return color;
}