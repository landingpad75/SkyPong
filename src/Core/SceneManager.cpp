#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOUSER
#define NOKERNEL
#define NOSOUND
#define NOMINMAX
#include <raylib.h>
#include <Core/SceneManager.hpp>

SceneManager::SceneManager()
    : transitionState(TransitionState::NONE)
    , fadeAlpha(0.0f)
    , fadeDuration(0.5f)
    , fadeTimer(0.0f)
    , isPopping(false) {
}

SceneManager::SceneManager(SceneManager&& other) noexcept
    : scenes(std::move(other.scenes))
    , sceneStack(std::move(other.sceneStack))
    , currentScene(std::move(other.currentScene))
    , transitionState(other.transitionState)
    , fadeAlpha(other.fadeAlpha)
    , fadeDuration(other.fadeDuration)
    , fadeTimer(other.fadeTimer)
    , nextSceneName(std::move(other.nextSceneName))
    , isPopping(other.isPopping) {}

SceneManager& SceneManager::operator=(SceneManager&& other) noexcept {
    if (this != &other) {
        scenes = std::move(other.scenes);
        sceneStack = std::move(other.sceneStack);
        currentScene = std::move(other.currentScene);
        transitionState = other.transitionState;
        fadeAlpha = other.fadeAlpha;
        fadeDuration = other.fadeDuration;
        fadeTimer = other.fadeTimer;
        nextSceneName = std::move(other.nextSceneName);
        isPopping = other.isPopping;
    }
    return *this;
}

void SceneManager::AddScene(std::unique_ptr<Scene> scene, const std::string_view &name) {
    if (scene) {
        scenes[name] = std::move(scene);
    }
}

void SceneManager::PushSceneInternal(const std::string_view &name) {
    auto it = scenes.find(name);
    if (it != scenes.end()) {
        it->second->Init();
        sceneStack.push(it->second.get());
        currentScene = name;
    }
}

void SceneManager::AddTopScene(std::unique_ptr<Scene> scene, const std::string_view &name) {
    if (scene) {
        Scene* rawPtr = scene.get();
        scenes[name] = std::move(scene);
        rawPtr->Init();
        sceneStack.push(rawPtr);
        currentScene = name;
    }
}

void SceneManager::PopSceneInternal() {
    if (!sceneStack.empty()) {
        sceneStack.top()->Deinit();
        sceneStack.pop();
        if (!sceneStack.empty()) {
            for (const auto& pair : scenes) {
                if (pair.second.get() == sceneStack.top()) {
                    currentScene = pair.first;
                    break;
                }
            }
        } else {
            currentScene = "";
        }
    }
}

void SceneManager::ReplaceSceneInternal(const std::string_view &name) {
    if (!sceneStack.empty()) {
        sceneStack.top()->Deinit();
        sceneStack.pop();
    }
    PushSceneInternal(name);
}

// New fade transition methods
void SceneManager::PushScene(const std::string_view &name, float fadeDuration) {
    this->fadeDuration = fadeDuration;
    StartFadeOut(name, false);
}

void SceneManager::PopScene(float fadeDuration) {
    this->fadeDuration = fadeDuration;
    StartFadeOut("", true);
}

void SceneManager::ReplaceScene(const std::string_view &name, float fadeDuration) {
    this->fadeDuration = fadeDuration;
    StartFadeOut(name, false);
}

void SceneManager::StartFadeOut(const std::string_view& sceneName, bool pop) {
    if (transitionState != TransitionState::NONE) return;

    nextSceneName = sceneName;
    isPopping = pop;
    transitionState = TransitionState::FADE_OUT;
    fadeTimer = 0.0f;
    fadeAlpha = 0.0f;
}

void SceneManager::UpdateFadeTransition(float deltaTime) {
    if (transitionState == TransitionState::NONE) return;

    fadeTimer += deltaTime;
    float progress = fadeTimer / fadeDuration;

    if (transitionState == TransitionState::FADE_OUT) {
        fadeAlpha = progress;
        if (fadeTimer >= fadeDuration) {
            if (isPopping) {
                PopSceneInternal();
            } else {
                ReplaceSceneInternal(nextSceneName);
            }
            transitionState = TransitionState::FADE_IN;
            fadeTimer = 0.0f;
        }
    } else if (transitionState == TransitionState::FADE_IN) {
        fadeAlpha = 1.0f - progress;
        if (fadeTimer >= fadeDuration) {
            transitionState = TransitionState::NONE;
            fadeAlpha = 0.0f;
        }
    }
}

void SceneManager::DrawFadeOverlay() {
    if (transitionState != TransitionState::NONE) {
        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                     Color{0, 0, 0, static_cast<unsigned char>(fadeAlpha * 255)});
    }
}

void SceneManager::Update() {
    float deltaTime = GetFrameTime();
    if (transitionState != TransitionState::NONE) {
        UpdateFadeTransition(deltaTime);
    }

    if (!sceneStack.empty() && transitionState != TransitionState::FADE_IN) {
        sceneStack.top()->Update(deltaTime);
    }
}

void SceneManager::Draw() {
    if (!sceneStack.empty()) {
        sceneStack.top()->Draw();
    }

    DrawFadeOverlay();
}