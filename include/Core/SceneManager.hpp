#pragma once
#include "Scene.hpp"
#include <memory>
#include <unordered_map>
#include <string_view>
#include <stack>

class SceneManager {
public:
    SceneManager();
    SceneManager(SceneManager&& other) noexcept;
    SceneManager& operator=(SceneManager&& other) noexcept;

    void AddScene(std::unique_ptr<Scene> scene, const std::string_view &name);
    void AddTopScene(std::unique_ptr<Scene> scene, const std::string_view &name);

    void PushScene(const std::string_view &name, float fadeDuration = 0.5f);
    void PopScene(float fadeDuration = 0.5f);
    void ReplaceScene(const std::string_view &name, float fadeDuration = 0.5f);

    void Update();
    void Draw();
    int level;
    std::string_view currentScene;

private:
    enum class TransitionState {
        NONE,
        FADE_OUT,
        FADE_IN
    };
    void PushSceneInternal(const std::string_view &name);
    void PopSceneInternal();
    void ReplaceSceneInternal(const std::string_view &name);

    std::unordered_map<std::string_view, std::unique_ptr<Scene>> scenes;
    std::stack<Scene*> sceneStack;

    TransitionState transitionState;
    float fadeAlpha;
    float fadeDuration;
    float fadeTimer;
    std::string nextSceneName;
    bool isPopping;

    void StartFadeOut(const std::string_view& sceneName = "", bool pop = false);
    void UpdateFadeTransition(float deltaTime);
    void DrawFadeOverlay();
};