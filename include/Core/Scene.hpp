#pragma once

class Scene {
public:
    Scene() = default;
    virtual ~Scene() {};

    virtual void Init() = 0;
    virtual void Deinit() = 0;

    virtual void Update(float deltaTime) = 0;
    virtual void Draw() = 0;

};