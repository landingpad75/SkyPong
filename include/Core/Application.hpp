#pragma once
#include "SceneManager.hpp"
#include <string>
struct IVec2 {
    int x;
    int y;
};


class Application {
public:

    Application() {}
    ~Application() {}

    SceneManager mgr;
    Application& setResizable(bool value){ resizable = value; return *this; }
    Application& setName(const std::string& name){ appName = name; return *this; }
    Application& setFullscreen(bool value){ fullscreen = value; return *this; }
    Application& setDims(IVec2 dims){ defaultDimensions = dims; return *this; }
    Application& Init();
    Application& Deinit();

    bool running();
private:
    bool resizable;
    bool fullscreen;
    IVec2 defaultDimensions;
    std::string appName;
};