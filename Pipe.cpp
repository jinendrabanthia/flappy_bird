#include "Pipe.hpp"

Pipe::Pipe(float x, float gapY, float gapSize)
    : m_position(x, 0.f)
    , m_previousPosition(x, 0.f)
    , m_gapY(gapY)
    , m_gapSize(gapSize)
{
    m_topShape.setFillColor(sf::Color::Green);
    m_topShape.setOutlineColor(sf::Color(0, 100, 0));
    m_topShape.setOutlineThickness(2.f);
    
    m_bottomShape.setFillColor(sf::Color::Green);
    m_bottomShape.setOutlineColor(sf::Color(0, 100, 0));
    m_bottomShape.setOutlineThickness(2.f);
}

void Pipe::update(sf::Time dt) {
    m_previousPosition = m_position;
    m_position.x -= m_speed * dt.asSeconds();
}

void Pipe::draw(sf::RenderTarget& target, float alpha) {
    float interpX = m_previousPosition.x + alpha * (m_position.x - m_previousPosition.x);
    
    // Configure shapes just before drawing
    m_topShape.setSize({m_width, m_gapY - m_gapSize / 2.f});
    m_topShape.setPosition({interpX, 0.f});
    
    float bottomY = m_gapY + m_gapSize / 2.f;
    m_bottomShape.setSize({m_width, 600.f - bottomY}); // assuming window height is 600
    m_bottomShape.setPosition({interpX, bottomY});
    
    target.draw(m_topShape);
    target.draw(m_bottomShape);
}

bool Pipe::isOffScreen() const {
    return m_position.x + m_width < 0.f;
}

sf::FloatRect Pipe::getTopBounds() const {
    return sf::FloatRect({m_position.x, 0.f}, {m_width, m_gapY - m_gapSize / 2.f});
}

sf::FloatRect Pipe::getBottomBounds() const {
    float bottomY = m_gapY + m_gapSize / 2.f;
    return sf::FloatRect({m_position.x, bottomY}, {m_width, 600.f - bottomY});
}
