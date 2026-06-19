#pragma once
#include "State.hpp"
#include "Game.hpp"
#include "Bird.hpp"
#include "Pipe.hpp"
#include <SFML/Audio.hpp>
#include <memory>
#include <vector>
#include <random>

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
    
    sf::Text m_titleShadowText;
    sf::Text m_titleText;
    sf::Text m_subtitleShadowText;
    sf::Text m_subtitleText;
    sf::Text m_startPromptText;
    float m_menuTimer = 0.f;
    
    int m_score = 0;
    int m_highScore = 0;
    float m_timeAlive = 0.f;
    
    sf::SoundBuffer m_flapBuffer;
    sf::Sound m_flapSound;
};
