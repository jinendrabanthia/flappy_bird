#pragma once
#include <SFML/Graphics.hpp>
#include <memory>
#include <stack>
#include <string>
#include "State.hpp"
#include "AssetManager.hpp"

class Game {
public:
    Game(int width, int height, const std::string& title);
    void run();

    // State Machine methods
    void pushState(std::unique_ptr<State> state);
    void popState();
    void changeState(std::unique_ptr<State> state);

    sf::RenderWindow& getWindow();
    AssetManager& getAssets();

private:
    void processEvents();
    void update(sf::Time dt);
    void render();

    sf::RenderWindow m_window;
    AssetManager m_assets;
    std::stack<std::unique_ptr<State>> m_states;

    // Fixed timestep settings
    const sf::Time m_timePerFrame = sf::seconds(1.f / 60.f);
};
