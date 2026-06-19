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
};
