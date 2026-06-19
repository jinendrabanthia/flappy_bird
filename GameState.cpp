#include "GameState.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <random>
#include <algorithm>

GameState::GameState(Game& game) : m_game(game), m_pipeSpawnTimer(0.f) {
    std::random_device rd;
    m_rng.seed(rd());
}

void GameState::init() {
    // Generate a 16x16 pixel-art bird
    const char* pixels[] = {
        "....YYYYYYYY....",
        "...YYYWWWWYY....",
        "..YYYYWBBWYYY...",
        ".YYYYYWWWWYYYYY.",
        ".YYYYYYYYYYOOOO.",
        ".YYYYYYYYYOOOOO.",
        "..YYYYYYYYYYYY..",
        "...YYYYYYYYYY...",
        "....YYYYYYYY....",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................",
        "................"
    };

    sf::Image image;
    image.resize({16, 16}, sf::Color::Transparent);

    for (unsigned int y = 0; y < 16; ++y) {
        for (unsigned int x = 0; x < 16; ++x) {
            char c = pixels[y][x];
            if (c == 'Y') image.setPixel({x, y}, sf::Color::Yellow);
            else if (c == 'W') image.setPixel({x, y}, sf::Color::White);
            else if (c == 'B') image.setPixel({x, y}, sf::Color::Black);
            else if (c == 'O') image.setPixel({x, y}, sf::Color(255, 165, 0)); // Orange
        }
    }

    if (!m_birdTexture.loadFromImage(image)) {
        // Ignore for now
    }

    reset();
}

void GameState::reset() {
    m_bird = std::make_unique<Bird>(m_birdTexture);
    m_pipes.clear();
    m_pipeSpawnTimer = 0.f;
}

void GameState::spawnPipe() {
    std::uniform_real_distribution<float> dist(200.f, 400.f);
    float gapY = dist(m_rng);
    m_pipes.emplace_back(800.f, gapY, 150.f); // 800 is window width
}

void GameState::handleInput() {
    static bool spacePressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)) {
        if (!spacePressed) {
            if (m_bird) m_bird->jump();
            spacePressed = true;
        }
    } else {
        spacePressed = false;
    }

    static bool shiftPressed = false;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
        if (!shiftPressed) {
            if (m_bird) m_bird->toggleAntiGravity();
            shiftPressed = true;
        }
    } else {
        shiftPressed = false;
    }
}

void GameState::update(sf::Time dt) {
    if (m_bird) {
        m_bird->update(dt);
        
        // Update pipes
        m_pipeSpawnTimer += dt.asSeconds();
        if (m_pipeSpawnTimer >= m_pipeSpawnInterval) {
            spawnPipe();
            m_pipeSpawnTimer = 0.f;
        }

        for (auto& pipe : m_pipes) {
            pipe.update(dt);
        }

        // Remove offscreen pipes
        m_pipes.erase(std::remove_if(m_pipes.begin(), m_pipes.end(), 
            [](const Pipe& p) { return p.isOffScreen(); }), m_pipes.end());

        // Collision detection
        sf::FloatRect birdBounds = m_bird->getGlobalBounds();
        
        bool collision = false;
        
        // Floor and ceiling
        if (birdBounds.position.y < 0.f || birdBounds.position.y + birdBounds.size.y > 600.f) {
            collision = true;
        }

        for (const auto& pipe : m_pipes) {
            if (birdBounds.findIntersection(pipe.getTopBounds()) || 
                birdBounds.findIntersection(pipe.getBottomBounds())) {
                collision = true;
                break;
            }
        }

        if (collision) {
            reset();
        }
    }
}

void GameState::draw(sf::RenderTarget& target, float alpha) {
    for (auto& pipe : m_pipes) {
        pipe.draw(target, alpha);
    }

    if (m_bird) {
        m_bird->draw(target, alpha);
    }
}
