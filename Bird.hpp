#pragma once
#include <SFML/Graphics.hpp>

class Bird {
public:
    Bird(const sf::Texture& texture);

    void handleInput();
    void update(sf::Time dt);
    void draw(sf::RenderTarget& target, float alpha); // Interpolated draw

    void jump();
    void toggleAntiGravity();
    void setJumpHeld(bool held) { m_isJumpHeld = held; }
    
    void dash();
    bool isDashing() const { return m_isDashing; }
    
    void setScreenWrap(bool enabled) { m_screenWrapEnabled = enabled; }
    void setUnderwater(bool enabled);
    void applyWind(float dx);
    
    sf::FloatRect getGlobalBounds() const { return m_sprite.getGlobalBounds(); }
    bool hasStarted() const { return m_started; }

private:
    sf::Sprite m_sprite;

    // Physics
    sf::Vector2f m_position;
    sf::Vector2f m_previousPosition; // For rendering interpolation
    sf::Vector2f m_velocity;
    
    float m_gravity;
    float m_targetGravity;
    float m_terminalVelocity;
    float m_jumpVelocity;

    // Anti-gravity transition
    bool m_isAntiGravity;
    bool m_isTransitioning;
    float m_transitionTimer;
    const float m_transitionDuration = 0.5f;
    
    float m_startGravity;
    float m_startRotation;
    float m_targetRotation;

    bool m_started = false;
    float m_animationTimer = 0.f;
    int m_currentFrame = 0;
    float m_currentScale = 1.5f;

    // Advanced Mechanics
    bool m_isJumpHeld = false;
    float m_jumpHoldTimer = 0.f;
    const float m_maxJumpHoldTime = 0.15f;

    bool m_isDashing = false;
    float m_dashTimer = 0.f;
    float m_dashCooldown = 0.f;
    const float m_dashDuration = 0.2f;

    bool m_screenWrapEnabled = false;
    bool m_isUnderwater = false;
};
