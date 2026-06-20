#pragma once
#include "State.hpp"
#include "Game.hpp"
#include "Bird.hpp"
#include "Pipe.hpp"
#include <SFML/Audio.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <memory>
#include <vector>
#include <random>

class Game;

struct SpeedBooster {
    sf::CircleShape shape;
    bool active = true;
};

struct Particle {
    sf::Vector2f pos;
    sf::Vector2f vel;
    float life;
    float maxLife;
    sf::Color color = sf::Color::White;
};

struct GravityZone {
    sf::FloatRect bounds;
    int type; // 0 = Inversion, 1 = Zero-G
    sf::RectangleShape shape;
    bool active = true;
};

class GameState : public State {
public:
    GameState(Game& game);

    void init() override;
    void handleInput() override;
    void update(sf::Time dt) override;
    void draw(sf::RenderTarget& target, float alpha) override;

private:
    void reset();
    void spawnPipe();

    Game& m_game;
    sf::Texture m_birdTexture;
    std::unique_ptr<Bird> m_bird;
    
    std::vector<Pipe> m_pipes;
    float m_pipeSpawnTimer;
    float m_currentPipeSpawnInterval = 1.5f;
    const float m_basePipeSpawnInterval = 1.5f;

    std::mt19937 m_rng;

    sf::Font m_font;
    sf::Font m_titleFont;
    sf::Text m_scoreText;
    
    sf::Texture m_logoTexture;
    sf::Sprite m_logoSprite;

    sf::Text m_titleShadowText;
    sf::Text m_titleText;
    sf::Text m_subtitleShadowText;
    sf::Text m_subtitleText;
    sf::Text m_startPromptText;
    
    sf::Text m_gameOverShadowText;
    sf::Text m_gameOverText;
    
    float m_menuTimer = 0.f;
    
    sf::RenderTexture m_bgRenderTexture;
    sf::Sprite m_bgSprite1;
    sf::Sprite m_bgSprite2;
    float m_bgScrollX = 0.f;
    
    int m_score = 0;
    int m_highScore = 0;
    int m_currentLevel = 1;
    float m_timeAlive = 0.f;
    bool m_isGameOver = false;

    // Advanced Mechanics State
    std::vector<SpeedBooster> m_boosters;
    std::vector<GravityZone> m_gravityZones;
    float m_boosterSpawnTimer = 0.f;
    float m_zoneSpawnTimer = 0.f;
    float m_speedMultiplier = 1.0f;
    float m_speedBoostTimer = 0.f;
    
    // UI Elements
    sf::Font m_uiFont;
    bool m_isDropdownOpen = false;
    int m_selectedStartLevel = 1;
    sf::RectangleShape m_dropdownMainBox;
    sf::Text m_dropdownMainText;
    std::vector<sf::RectangleShape> m_dropdownItemBoxes;
    std::vector<sf::Text> m_dropdownItemTexts;
    
    // Gravity UI
    bool m_isAntiGravityMode = false;
    sf::RectangleShape m_gravityButtonBox;
    sf::Text m_gravityButtonText;
    
    sf::SoundBuffer m_flapBuffer;
    sf::Sound m_flapSound;
    
    sf::SoundBuffer m_crashBuffer;
    sf::Sound m_crashSound;
    
    sf::SoundBuffer m_dingBuffer;
    sf::Sound m_dingSound;

    sf::SoundBuffer m_whooshBuffer;
    sf::Sound m_whooshSound;

    // Juice & Game Feel
    float m_hitStopTimer = 0.f;
    float m_screenShakeTimer = 0.f;
    float m_deathFlashTimer = 0.f;
    float m_trailTimer = 0.f;
    std::vector<Particle> m_particles;
};
