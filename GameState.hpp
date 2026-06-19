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
    const float m_pipeSpawnInterval = 1.5f;

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
    float m_boosterSpawnTimer = 0.f;
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
    
    sf::SoundBuffer m_flapBuffer;
    sf::Sound m_flapSound;
    
    sf::SoundBuffer m_crashBuffer;
    sf::Sound m_crashSound;
};
