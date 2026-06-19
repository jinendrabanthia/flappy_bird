#include "Bird.hpp"
#include <algorithm>

Bird::Bird(const sf::Texture& texture)
    : m_position(100.f, 300.f)
    , m_previousPosition(100.f, 300.f)
    , m_velocity(0.f, 0.f)
    , m_gravity(1500.f)
    , m_targetGravity(1500.f)
    , m_terminalVelocity(800.f)
    , m_jumpVelocity(-600.f)
    , m_isAntiGravity(false)
    , m_isTransitioning(false)
    , m_transitionTimer(0.f)
    , m_startGravity(1500.f)
    , m_startRotation(0.f)
    , m_targetRotation(0.f)
    , m_sprite(texture)
{
    // Set origin to center for proper rotation
    m_sprite.setOrigin(sf::Vector2f(texture.getSize().x / 2.f, texture.getSize().y / 2.f));
    m_sprite.setPosition(m_position);
}

void Bird::handleInput() {
    // Input is usually handled by the GameState calling jump() or toggleAntiGravity()
}

void Bird::update(sf::Time dt) {
    m_previousPosition = m_position;

    if (m_isTransitioning) {
        m_transitionTimer += dt.asSeconds();
        float t = std::min(m_transitionTimer / m_transitionDuration, 1.0f);
        
        // Linear interpolation for gravity and rotation
        m_gravity = m_startGravity + t * (m_targetGravity - m_startGravity);
        float currentRotation = m_startRotation + t * (m_targetRotation - m_startRotation);
        m_sprite.setRotation(sf::degrees(currentRotation));

        if (t >= 1.0f) {
            m_isTransitioning = false;
        }
    }

    // Apply gravity
    m_velocity.y += m_gravity * dt.asSeconds();

    // Terminal velocity limit
    if (m_velocity.y > m_terminalVelocity) {
        m_velocity.y = m_terminalVelocity;
    } else if (m_velocity.y < -m_terminalVelocity) {
        m_velocity.y = -m_terminalVelocity;
    }

    // Update position
    m_position += m_velocity * dt.asSeconds();
}

void Bird::draw(sf::RenderTarget& target, float alpha) {
    // Interpolate position for smooth rendering
    sf::Vector2f interpolatedPosition = m_previousPosition + alpha * (m_position - m_previousPosition);
    m_sprite.setPosition(interpolatedPosition);
    target.draw(m_sprite);
}

void Bird::jump() {
    // In anti-gravity, jump velocity is reversed
    m_velocity.y = m_isAntiGravity ? -m_jumpVelocity : m_jumpVelocity;
}

void Bird::toggleAntiGravity() {
    m_isAntiGravity = !m_isAntiGravity;
    m_isTransitioning = true;
    m_transitionTimer = 0.f;
    
    m_startGravity = m_gravity;
    m_targetGravity = m_isAntiGravity ? -1500.f : 1500.f;
    
    m_startRotation = m_sprite.getRotation().asDegrees();
    m_targetRotation = m_isAntiGravity ? 180.f : 0.f;
    
    // Normalize start rotation to prevent spinning multiple times
    if (m_startRotation > 180.f) m_startRotation -= 360.f;
}
