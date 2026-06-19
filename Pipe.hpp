#pragma once
#include <SFML/Graphics.hpp>

class Pipe {
public:
    Pipe(float x, float gapY, float gapSize);

    void update(sf::Time dt);
    void draw(sf::RenderTarget& target, float alpha);

    bool isOffScreen() const;
    float getX() const { return m_position.x; }
    
    // For collision detection
    sf::FloatRect getTopBounds() const;
    sf::FloatRect getBottomBounds() const;

    bool isPassed() const { return m_passed; }
    void setPassed(bool passed) { m_passed = passed; }

private:
    bool m_passed = false;
    sf::Vector2f m_position;
    sf::Vector2f m_previousPosition;
    float m_gapY;
    float m_gapSize;
    float m_width = 80.f;
    float m_speed = 200.f; // Move left at 200 pixels/sec
    
    sf::RectangleShape m_topShape;
    sf::RectangleShape m_bottomShape;
};
