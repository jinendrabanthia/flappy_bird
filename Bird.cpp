#include "Bird.hpp"
#include <algorithm>

Bird::Bird(const sf::Texture& texture)
    : m_position(100.f, 300.f)
    , m_previousPosition(100.f, 300.f)
    , m_velocity(0.f, 0.f)
    , m_gravity(1500.f)
    , m_targetGravity(1500.f)
    , m_terminalVelocity(800.f)
    , m_jumpVelocity(-450.f)
    , m_isAntiGravity(false)
    , m_isTransitioning(false)
    , m_transitionTimer(0.f)
    , m_startGravity(1500.f)
    , m_startRotation(0.f)
    , m_targetRotation(0.f)
    , m_sprite(texture)
{
    // Set up initial frame
    m_sprite.setTextureRect(sf::IntRect({0, 0}, {16, 16}));
    // Set origin to center for proper rotation
    m_sprite.setOrigin(sf::Vector2f(8.f, 8.f));
    m_sprite.setScale({m_currentScale, m_currentScale});
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

    if (m_started) {
        if (m_dashCooldown > 0.f) m_dashCooldown -= dt.asSeconds();

        if (m_isDashing) {
            m_dashTimer -= dt.asSeconds();
            m_velocity.y = 0.f; // Float straight during dash
            // Visual Haptics for dash
            m_currentScale = 1.0f + (m_dashTimer / m_dashDuration);
            if (m_dashTimer <= 0.f) m_isDashing = false;
        } else {
            // Apply gravity
            float currentGravity = m_gravity;
            if (m_isJumpHeld && m_jumpHoldTimer > 0.f) {
                m_jumpHoldTimer -= dt.asSeconds();
                currentGravity *= 0.2f; // Weak gravity while holding space
            }
            m_velocity.y += currentGravity * dt.asSeconds();

            // Terminal velocity limit
            if (m_velocity.y > m_terminalVelocity) {
                m_velocity.y = m_terminalVelocity;
            } else if (m_velocity.y < -m_terminalVelocity) {
                m_velocity.y = -m_terminalVelocity;
            }
        }

        // Update position
        m_position += m_velocity * dt.asSeconds();
        
        // Screen Wrap Portal
        if (m_screenWrapEnabled) {
            if (m_position.y < -20.f) {
                m_position.y = 620.f;
                m_previousPosition.y = 620.f; // Avoid interpolation artifact
            } else if (m_position.y > 620.f) {
                m_position.y = -20.f;
                m_previousPosition.y = -20.f;
            }
        }
        
        // Visual Haptics: Smoothly scale back to normal size
        m_currentScale += (1.5f - m_currentScale) * 15.f * dt.asSeconds();
        m_sprite.setScale({m_currentScale, m_currentScale});

        // Animation
        m_animationTimer += dt.asSeconds();
        if (m_animationTimer > 0.15f) {
            m_animationTimer = 0.f;
            m_currentFrame = (m_currentFrame + 1) % 2;
            m_sprite.setTextureRect(sf::IntRect({m_currentFrame * 16, 0}, {16, 16}));
        }
    }
}

void Bird::draw(sf::RenderTarget& target, float alpha) {
    // Interpolate position for smooth rendering
    sf::Vector2f interpolatedPosition = m_previousPosition + alpha * (m_position - m_previousPosition);
    m_sprite.setPosition(interpolatedPosition);
    target.draw(m_sprite);
}

void Bird::jump() {
    m_started = true;
    m_velocity.y = m_isAntiGravity ? -m_jumpVelocity : m_jumpVelocity;
    m_jumpHoldTimer = m_maxJumpHoldTime; // Reset jump hold limit
    
    // Visual haptic: squash and stretch pop!
    m_currentScale = 2.0f; // Exaggerate the pop
    m_sprite.setScale({m_currentScale, m_currentScale});
}

void Bird::dash() {
    if (m_started && m_dashCooldown <= 0.f) {
        m_isDashing = true;
        m_dashTimer = m_dashDuration;
        m_dashCooldown = 2.0f;
    }
}

void Bird::toggleAntiGravity() {
    m_isAntiGravity = !m_isAntiGravity;
    m_isTransitioning = true;
    m_transitionTimer = 0.f;
    
    m_startGravity = m_gravity;
    m_targetGravity = m_isAntiGravity ? -1500.f : 1500.f;
    if (m_isUnderwater) m_targetGravity *= 0.5f;
    
    m_startRotation = m_sprite.getRotation().asDegrees();
    m_targetRotation = m_isAntiGravity ? 180.f : 0.f;
    
    if (m_startRotation > 180.f) m_startRotation -= 360.f;
}

void Bird::setAntiGravityImmediate(bool enabled) {
    if (m_isAntiGravity == enabled) return;
    m_isAntiGravity = enabled;
    m_isTransitioning = false;
    
    m_gravity = m_isAntiGravity ? -1500.f : 1500.f;
    m_targetGravity = m_gravity;
    if (m_isUnderwater) {
        m_gravity *= 0.5f;
        m_targetGravity *= 0.5f;
    }
    
    m_sprite.setRotation(sf::degrees(m_isAntiGravity ? 180.f : 0.f));
}

void Bird::setUnderwater(bool enabled) {
    if (m_isUnderwater == enabled) return;
    m_isUnderwater = enabled;
    m_jumpVelocity = m_isUnderwater ? -300.f : -450.f;
    m_gravity = m_isAntiGravity ? (m_isUnderwater ? -750.f : -1500.f) : (m_isUnderwater ? 750.f : 1500.f);
}

void Bird::applyWind(float dx) {
    m_position.x += dx;
    if (m_position.x < 30.f) m_position.x = 30.f;
    if (m_position.x > 770.f) m_position.x = 770.f;
}
