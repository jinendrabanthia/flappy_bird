#include "Pipe.hpp"

#include <cmath>
#include <algorithm>

Pipe::Pipe(float x, float gapY, float gapSize, Type type)
    : m_type(type)
    , m_position(x, 0.f)
    , m_previousPosition(x, 0.f)
    , m_gapY(gapY)
    , m_gapSize(gapSize)
    , m_baseGapY(gapY)
    , m_targetGapY(gapY)
{
    m_baseColor = sf::Color(115, 191, 46); // Flappy Bird Green
    m_outlineColor = sf::Color(84, 56, 71); // Dark brown/black outline

    if (m_type == Type::Breakable) {
        m_baseColor = sf::Color(139, 69, 19); // SaddleBrown
        m_outlineColor = sf::Color(80, 40, 10);
    } else if (m_type == Type::Moving) {
        m_baseColor = sf::Color(255, 165, 0); // Orange
        m_outlineColor = sf::Color(200, 100, 0);
    } else if (m_type == Type::Crushing) {
        m_baseColor = sf::Color::Red;
        m_outlineColor = sf::Color(139, 0, 0);
    } else if (m_type == Type::Diagonal) {
        m_baseColor = sf::Color::Magenta;
        m_outlineColor = sf::Color(139, 0, 139);
        m_topOffsetX = 50.f; // The top pipe is pushed right
    } else if (m_type == Type::FakeOut) {
        m_baseColor = sf::Color(100, 100, 255); // Light Blue
        m_outlineColor = sf::Color(0, 0, 139); // Dark Blue
    }
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
    } else if (m_type == Type::FakeOut) {
        // Smoothly move towards target gap
        m_gapY += (m_targetGapY - m_gapY) * 10.f * dt.asSeconds();
    }
}

void Pipe::draw(sf::RenderTarget& target, float alpha) {
    if (m_broken) return;

    float interpX = m_previousPosition.x + alpha * (m_position.x - m_previousPosition.x);
    
    float capHeight = 24.f;
    float outlineThick = 2.f;

    auto adjustColor = [](sf::Color c, int amt) {
        int r = std::clamp(c.r + amt, 0, 255);
        int g = std::clamp(c.g + amt, 0, 255);
        int b = std::clamp(c.b + amt, 0, 255);
        return sf::Color(r, g, b, c.a);
    };

    sf::Color light = adjustColor(m_baseColor, 50);
    sf::Color veryLight = adjustColor(m_baseColor, 100);
    sf::Color dark = adjustColor(m_baseColor, -40);

    // Helper to draw a pipe segment
    auto drawSegment = [&](float x, float y, float w, float h) {
        // Main outline & base
        sf::RectangleShape rect({w, h});
        rect.setPosition({x, y});
        rect.setFillColor(m_baseColor);
        rect.setOutlineColor(m_outlineColor);
        rect.setOutlineThickness(outlineThick);
        target.draw(rect);

        // Highlight 1 (Very Light) - far left
        sf::RectangleShape hl1({2.f, h});
        hl1.setPosition({x + 2.f, y});
        hl1.setFillColor(veryLight);
        target.draw(hl1);

        // Highlight 2 (Light) - mid left
        sf::RectangleShape hl2({6.f, h});
        hl2.setPosition({x + 4.f, y});
        hl2.setFillColor(light);
        target.draw(hl2);

        // Shadow (Dark) - right side
        sf::RectangleShape sh({8.f, h});
        sh.setPosition({x + w - 10.f, y}); // 10px from right (8 width + 2 border spacing)
        sh.setFillColor(dark);
        target.draw(sh);
    };

    // Top Pipe
    float topCapY = m_gapY - m_gapSize / 2.f - capHeight;
    float topBodyHeight = std::max(0.f, topCapY);
    if (topBodyHeight > 0.f) {
        // Body
        drawSegment(interpX + m_topOffsetX, 0.f, m_width, topBodyHeight);
    }
    // Top Cap
    drawSegment(interpX + m_topOffsetX - 2.f, topCapY, m_width + 4.f, capHeight);

    // Bottom Pipe
    float bottomCapY = m_gapY + m_gapSize / 2.f;
    // Bottom Cap
    drawSegment(interpX + m_bottomOffsetX - 2.f, bottomCapY, m_width + 4.f, capHeight);
    
    float bottomBodyHeight = 600.f - (bottomCapY + capHeight);
    if (bottomBodyHeight > 0.f) {
        // Body
        drawSegment(interpX + m_bottomOffsetX, bottomCapY + capHeight, m_width, bottomBodyHeight);
    }
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
