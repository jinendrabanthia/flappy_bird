#include "Game.hpp"
#include <iostream>
#include <optional>

Game::Game(int width, int height, const std::string& title)
    : m_window(sf::VideoMode({static_cast<unsigned int>(width), static_cast<unsigned int>(height)}), title, sf::Style::Close) {
}

void Game::run() {
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;

    while (m_window.isOpen()) {
        processEvents();

        sf::Time dt = clock.restart();
        timeSinceLastUpdate += dt;

        while (timeSinceLastUpdate > m_timePerFrame) {
            timeSinceLastUpdate -= m_timePerFrame;
            processEvents();
            update(m_timePerFrame);
        }

        // Calculate interpolation alpha
        float alpha = timeSinceLastUpdate.asSeconds() / m_timePerFrame.asSeconds();
        
        m_window.clear();
        if (!m_states.empty()) {
            m_states.top()->draw(m_window, alpha);
        }
        m_window.display();
    }
}

void Game::processEvents() {
    while (const std::optional event = m_window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            m_window.close();
        }
        // State input is polled directly in state->handleInput() via sf::Keyboard, 
        // avoiding complex event passing for simplicity.
    }
}

void Game::update(sf::Time dt) {
    if (!m_states.empty()) {
        m_states.top()->handleInput();
        m_states.top()->update(dt);
    }
}

void Game::pushState(std::unique_ptr<State> state) {
    if (!m_states.empty()) {
        m_states.top()->pause();
    }
    m_states.push(std::move(state));
    m_states.top()->init();
}

void Game::popState() {
    if (!m_states.empty()) {
        m_states.pop();
    }
    if (!m_states.empty()) {
        m_states.top()->resume();
    }
}

void Game::changeState(std::unique_ptr<State> state) {
    if (!m_states.empty()) {
        m_states.pop();
    }
    m_states.push(std::move(state));
    m_states.top()->init();
}

sf::RenderWindow& Game::getWindow() { return m_window; }
AssetManager& Game::getAssets() { return m_assets; }
