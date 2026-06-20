#pragma once
#include <SFML/Graphics.hpp>

class Pipe {
public:
    enum class Type {
        Normal, Moving, Crushing, Diagonal, Breakable, FakeOut
    };
    Pipe(float x, float gapY, float gapSize, Type type = Type::Normal);

    void update(sf::Time dt);
    void draw(sf::RenderTarget& target, float alpha);

    bool isOffScreen() const;
    float getX() const { return m_position.x; }
    
    // For collision detection
    sf::FloatRect getTopBounds() const;
    sf::FloatRect getBottomBounds() const;

    bool isPassed() const { return m_passed; }
    void setPassed(bool passed) { m_passed = passed; }

    Type getType() const { return m_type; }
    void setType(Type type) { m_type = type; }
    bool isBroken() const { return m_broken; }
    void breakPipe() { m_broken = true; }
    
    float getGapY() const { return m_gapY; }
    void setGapY(float y) { m_gapY = y; }
    float getTargetGapY() const { return m_targetGapY; }
    void setTargetGapY(float y) { m_targetGapY = y; }

private:
    Type m_type;
    bool m_broken = false;
    bool m_passed = false;
    sf::Vector2f m_position;
    sf::Vector2f m_previousPosition;
    float m_gapY;
    float m_gapSize;
    float m_width = 50.f;
    float m_speed = 200.f; // Move left at 200 pixels/sec
    
    // For advanced variants
    float m_timeAlive = 0.f;
    float m_baseGapY;
    float m_targetGapY; // For FakeOut
    float m_topOffsetX = 0.f;
    float m_bottomOffsetX = 0.f;
    
    sf::Color m_baseColor;
    sf::Color m_outlineColor;
};
