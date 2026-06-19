#include "GameState.hpp"
#include <SFML/Window/Keyboard.hpp>
#include <random>
#include <algorithm>
#include <cmath>
#include <cstdint>

GameState::GameState(Game& game) 
    : m_game(game), m_pipeSpawnTimer(0.f), m_scoreText(m_font), m_flapSound(m_flapBuffer) {
    std::random_device rd;
    m_rng.seed(rd());
}

void GameState::init() {
    // Generate a 32x16 pixel-art bird (2 frames)
    const char* pixels[] = {
        "....YYYYYYYY........YYYYYYYY....",
        "...YYYWWWWYY.......YYYWWWWYY....",
        "..YYYYWBBWYYY.....YYYYWBBWYYY...",
        ".YYYYYWWWWYYYYY..YYYYYWWWWYYYYY.",
        ".YYWYYYYYYYOOOO..YYYYYYYYYYOOOO.",
        ".YYWWYYYYYOOOOO..YYYWWYYYYOOOOO.",
        "..YWWYYYYYYYYY....YYWWYYYYYYYY..",
        "...YYYYYYYYYY......YWWYYYYYYY...",
        "....YYYYYYYY........YYYYYYYY....",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................",
        "................................"
    };

    sf::Image image;
    image.resize({32, 16}, sf::Color::Transparent);

    for (unsigned int y = 0; y < 16; ++y) {
        for (unsigned int x = 0; x < 32; ++x) {
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

    if (m_font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
        m_scoreText.setFont(m_font);
        m_scoreText.setCharacterSize(20);
        m_scoreText.setFillColor(sf::Color::White);
        m_scoreText.setOutlineColor(sf::Color::Black);
        m_scoreText.setOutlineThickness(2.f);
        m_scoreText.setPosition({800.f - 250.f, 20.f}); // Roughly top right
    }

    // Generate procedural flap sound
    std::vector<std::int16_t> samples;
    unsigned int sampleRate = 44100;
    float duration = 0.1f;
    for (int i = 0; i < sampleRate * duration; ++i) {
        float t = (float)i / sampleRate;
        
        // Pitch sweeps up quickly from 300 Hz to 800 Hz
        float currentFreq = 300.f + 5000.f * t; 
        float phase = 2.f * 3.14159265f * currentFreq * t;
        
        // Complex waveform for a more pleasant "chime" or "chirp" sound
        float wave = std::sin(phase) + 0.5f * std::sin(phase * 2.f) + 0.25f * std::sin(phase * 3.f);
        wave /= 1.75f; // Normalize
        
        // Attack-decay envelope for a soft 'pop'
        float envelope = 0.f;
        if (t < 0.02f) { // 20ms attack
            envelope = t / 0.02f;
        } else { // 80ms decay
            envelope = 1.0f - ((t - 0.02f) / 0.08f);
        }
        envelope = envelope * envelope; // Quadratic decay
        
        short sample = 15000 * wave * envelope;
        samples.push_back(sample);
    }
    m_flapBuffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate, {sf::SoundChannel::Mono});
    m_flapSound.setBuffer(m_flapBuffer);

    reset();
}

void GameState::reset() {
    m_bird = std::make_unique<Bird>(m_birdTexture);
    m_pipes.clear();
    m_pipeSpawnTimer = 0.f;
    m_score = 0;
    m_timeAlive = 0.f;
    m_scoreText.setString("Score: 0 | Time: 0s");
    m_scoreText.setPosition({800.f - m_scoreText.getGlobalBounds().size.x - 20.f, 20.f});
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
            m_flapSound.play();
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
        
        if (m_bird->hasStarted()) {
            m_timeAlive += dt.asSeconds();
            m_scoreText.setString("Score: " + std::to_string(m_score) + " | Time: " + std::to_string(static_cast<int>(m_timeAlive)) + "s");
            m_scoreText.setPosition({800.f - m_scoreText.getGlobalBounds().size.x - 20.f, 20.f});

            // Update pipes
            m_pipeSpawnTimer += dt.asSeconds();
            if (m_pipeSpawnTimer >= m_pipeSpawnInterval) {
                spawnPipe();
                m_pipeSpawnTimer = 0.f;
            }
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

        for (auto& pipe : m_pipes) {
            if (birdBounds.findIntersection(pipe.getTopBounds()) || 
                birdBounds.findIntersection(pipe.getBottomBounds())) {
                collision = true;
                break;
            }
            
            if (!pipe.isPassed() && birdBounds.position.x > pipe.getX() + 80.f) {
                pipe.setPassed(true);
                m_score++;
                // String is updated every frame in the time loop above anyway, 
                // but we can update it immediately here too to avoid 1 frame delay.
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
    
    target.draw(m_scoreText);
}
