#include "Pipe.hpp"

#include <cmath>

Pipe::Pipe(float x, float gapY, float gapSize, Type type)
    : m_type(type)
    , m_position(x, 0.f)
    , m_previousPosition(x, 0.f)
    , m_gapY(gapY)
    , m_gapSize(gapSize)
    , m_baseGapY(gapY)
{
    sf::Color pipeColor = sf::Color::Green;
    sf::Color outlineColor = sf::Color(0, 100, 0);

    if (m_type == Type::Breakable) {
        pipeColor = sf::Color(139, 69, 19); // SaddleBrown
        outlineColor = sf::Color(80, 40, 10);
    } else if (m_type == Type::Moving) {
        pipeColor = sf::Color(255, 165, 0); // Orange
        outlineColor = sf::Color(200, 100, 0);
    } else if (m_type == Type::Crushing) {
        pipeColor = sf::Color::Red;
        outlineColor = sf::Color(139, 0, 0);
    } else if (m_type == Type::Diagonal) {
        pipeColor = sf::Color::Magenta;
        outlineColor = sf::Color(139, 0, 139);
        m_topOffsetX = 50.f; // The top pipe is pushed right
    }

    m_topShape.setFillColor(pipeColor);
    m_topShape.setOutlineColor(outlineColor);
    m_topShape.setOutlineThickness(2.f);
    
    m_bottomShape.setFillColor(pipeColor);
    m_bottomShape.setOutlineColor(outlineColor);
    m_bottomShape.setOutlineThickness(2.f);
}

void Pipe::update(sf::Time dt) {
    m_previousPosition = m_position;
    m_position.x -= m_speed * dt.asSeconds();
    m_timeAlive += dt.asSeconds();

    if (m_type == Type::Moving) {
        m_gapY = m_baseGapY + std::sin(m_timeAlive * 3.f) * 100.f;
    } else if (m_type == Type::Crushing) {
        if (m_gapSize > 60.f) {
            m_gapSize -= 20.f * dt.asSeconds();
        }
    }
}

void Pipe::draw(sf::RenderTarget& target, float alpha) {
    if (m_broken) return;

    float interpX = m_previousPosition.x + alpha * (m_position.x - m_previousPosition.x);
    
    // Configure shapes just before drawing
    m_topShape.setSize({m_width, m_gapY - m_gapSize / 2.f});
    m_topShape.setPosition({interpX + m_topOffsetX, 0.f});
    
    float bottomY = m_gapY + m_gapSize / 2.f;
    m_bottomShape.setSize({m_width, 600.f - bottomY}); // assuming window height is 600
    m_bottomShape.setPosition({interpX + m_bottomOffsetX, bottomY});
    
    target.draw(m_topShape);
    target.draw(m_bottomShape);
}

bool Pipe::isOffScreen() const {
    return m_position.x + m_width < 0.f;
}

sf::FloatRect Pipe::getTopBounds() const {
    if (m_broken) return sf::FloatRect();
    return sf::FloatRect({m_position.x + m_topOffsetX, 0.f}, {m_width, m_gapY - m_gapSize / 2.f});
}

sf::FloatRect Pipe::getBottomBounds() const {
    if (m_broken) return sf::FloatRect();
    float bottomY = m_gapY + m_gapSize / 2.f;
    return sf::FloatRect({m_position.x + m_bottomOffsetX, bottomY}, {m_width, 600.f - bottomY});
}
