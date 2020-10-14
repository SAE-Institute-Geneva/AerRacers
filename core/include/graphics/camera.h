#pragma once
#include "engine/engine.h"

#include "mathematics/vector.h"
#include "mathematics/matrix.h"
#include "mathematics/transform.h"
#include "utilities/service_locator.h"

namespace neko
{
struct Camera
{
    Vec3f position = Vec3f::back;
    Vec3f reverseDirection = Vec3f::back;

    float nearPlane = 0.1f;
    float farPlane = 10'000.0f;

    void WorldLookAt(const Vec3f& target);

    [[nodiscard]] Vec3f GetRight() const;
    [[nodiscard]] Vec3f GetUp() const;

    [[nodiscard]] EulerAngles GetRotation() const;
    [[nodiscard]] Mat4f GetRotationMat() const;

    [[nodiscard]] Mat4f GenerateViewMatrix() const;

    void SetDirectionFromEuler(const EulerAngles& angles);

    void Rotate(const EulerAngles& angles);

    [[nodiscard]] float GetAspect() const { return aspect_; }

    void SetAspect(const float aspect) { aspect_ = aspect; }

    void SetAspect(const int width, const int height)
    { aspect_ = static_cast<float>(width) / static_cast<float>(height); }

    [[nodiscard]] virtual Mat4f GenerateProjectionMatrix() const = 0;

protected:
    float aspect_ = 1.0f;
};

struct Camera2D : Camera
{
    float size = 5.0f;
    [[nodiscard]] Mat4f GenerateProjectionMatrix() const override;
};

struct Camera3D : Camera
{
    degree_t fovY = degree_t(45.0f);

    [[nodiscard]] Mat4f GenerateProjectionMatrix() const override;
    [[nodiscard]] radian_t GetFovX() const;
};

struct MovableCamera : sdl::SdlEventSystemInterface, SystemInterface
{
    float moveSpeed = 5.0f;
    float mouseSpeed = 1.0f;
    float mouseSensitivity = 0.5f;

protected:
    Vec2f mouseMotion_;
};

struct MovableCamera2D final : Camera2D, MovableCamera
{
    void Init() override
    {
    }

    void Update(const seconds dt) override
    {
        //Checking if keys are down
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            position += GetRight() * moveSpeed * dt.count();
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            position -= GetRight() * moveSpeed * dt.count();
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
            size -= moveSpeed * dt.count();
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
            size += moveSpeed * dt.count();
        if (keys[SDL_SCANCODE_SPACE])
            position.y += moveSpeed * dt.count();
        if (keys[SDL_SCANCODE_LSHIFT])
            position.y -= moveSpeed * dt.count();

        const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
        if (mouseState & SDL_BUTTON(3))
        {
            Rotate(EulerAngles(
                    degree_t(mouseMotion_.y),
                    degree_t(mouseMotion_.x),
                    degree_t(0.0f)
            ));
            mouseMotion_ = Vec2f::zero;
        }
    }

    void OnEvent(const SDL_Event& event) override
    {
        if(event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            SetAspect(event.window.data1, event.window.data2);
        }

        if (event.type == SDL_MOUSEMOTION)
            mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) / moveSpeed;
    }

    void Destroy() override
    {
    }
};

struct MovableCamera3D : Camera3D, public MovableCamera
{
    void Init() override
    {
        const auto& config = BasicEngine::GetInstance()->config;
        SetAspect(config.windowSize.x, config.windowSize.y);
    }

    void Update(const seconds dt) override
    {
        //Checking if keys are down
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        Vec3f move{};
        //Checking if keys are down
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            move += GetRight() * dt.count();
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            move -= GetRight() * dt.count();
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
            move -= reverseDirection * dt.count();
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
            move += reverseDirection * dt.count();
        if (keys[SDL_SCANCODE_SPACE])
            move.y += dt.count();
        if (keys[SDL_SCANCODE_LSHIFT])
            move.y -= dt.count();

        if (keys[SDL_SCANCODE_R])
            position += move * moveSpeed * 5.0f;
        else
            position += move * moveSpeed;

        const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
        if (mouseState & SDL_BUTTON(3))
        {
            Rotate(EulerAngles(
                    degree_t(mouseMotion_.y),
                    degree_t(mouseMotion_.x),
                    degree_t(0.0f)
            ));
            mouseMotion_ = Vec2f::zero;
        }
    }

    void OnEvent(const SDL_Event& event) override
    {
        if (event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            SetAspect(event.window.data1, event.window.data2);
        }

        if (event.type == SDL_MOUSEMOTION)
            mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;
    }

    void Destroy() override
    {
    }
};

struct FpsCamera final : public MovableCamera3D
{
    bool freezeCam = false;

    FpsCamera& operator=(const FpsCamera& other)
    {
        position = other.position;
        reverseDirection = other.reverseDirection;

        farPlane = other.farPlane;
        nearPlane = other.nearPlane;
        fovY = other.fovY;

        mouseSpeed = other.mouseSpeed;
        moveSpeed = other.moveSpeed;
        mouseMotion_ = other.mouseMotion_;

        freezeCam = other.freezeCam;

        return *this;
    }

    void Init() override
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
    }

    void Update(const seconds dt) override
    {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (!freezeCam)
        {
            Vec3f move{};
            //Checking if keys are down
            if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
                move += GetRight() * dt.count();
            if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
                move -= GetRight() * dt.count();
            if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
                move -= reverseDirection * dt.count();
            if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
                move += reverseDirection * dt.count();
            if (keys[SDL_SCANCODE_SPACE])
                move.y += dt.count();
            if (keys[SDL_SCANCODE_LSHIFT])
                move.y -= dt.count();

            if (keys[SDL_SCANCODE_R])
                position += move * moveSpeed * 5.0f;
            else
                position += move * moveSpeed;

            Rotate(EulerAngles(
                    degree_t(mouseMotion_.y),
                    degree_t(mouseMotion_.x),
                    degree_t(0.0f)
            ));
            mouseMotion_ = Vec2f::zero;
        }
    }

    void OnEvent(const SDL_Event& event) override
    {
        const Uint8* keys = SDL_GetKeyboardState(nullptr);
        if (keys[SDL_SCANCODE_ESCAPE] && event.type == SDL_KEYDOWN)
        {
            freezeCam = !freezeCam;
            SDL_SetRelativeMouseMode(static_cast<SDL_bool>(!freezeCam));
        }

        if(event.window.event == SDL_WINDOWEVENT_RESIZED)
        {
            SetAspect(event.window.data1, event.window.data2);
        }

        if (!freezeCam)
        {
            if (event.type == SDL_MOUSEMOTION)
                mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;

            SDL_WarpMouseGlobal(event.window.data1 / 2, event.window.data2 / 2);
        }
    }

    void Destroy() override
    {
    }
};
/**
 * \brief Exclusively used for the CameraLocator, please do not use!
 */
struct NullCamera : Camera
{
    [[nodiscard]] Mat4f GenerateProjectionMatrix() const override;

};

using CameraLocator = Locator<Camera, NullCamera>;

}  