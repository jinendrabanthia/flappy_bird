#include "AssetManager.hpp"
#include <iostream>

void AssetManager::loadTexture(const std::string& name, const std::string& fileName) {
    auto texture = std::make_unique<sf::Texture>();
    if (texture->loadFromFile(fileName)) {
        m_textures[name] = std::move(texture);
    } else {
        std::cerr << "Failed to load texture: " << fileName << std::endl;
    }
}

sf::Texture& AssetManager::getTexture(const std::string& name) {
    return *m_textures.at(name);
}

void AssetManager::loadFont(const std::string& name, const std::string& fileName) {
    auto font = std::make_unique<sf::Font>();
    if (font->loadFromFile(fileName)) {
        m_fonts[name] = std::move(font);
    } else {
        std::cerr << "Failed to load font: " << fileName << std::endl;
    }
}

sf::Font& AssetManager::getFont(const std::string& name) {
    return *m_fonts.at(name);
}

void AssetManager::loadSoundBuffer(const std::string& name, const std::string& fileName) {
    auto soundBuffer = std::make_unique<sf::SoundBuffer>();
    if (soundBuffer->loadFromFile(fileName)) {
        m_sounds[name] = std::move(soundBuffer);
    } else {
        std::cerr << "Failed to load sound buffer: " << fileName << std::endl;
    }
}

sf::SoundBuffer& AssetManager::getSoundBuffer(const std::string& name) {
    return *m_sounds.at(name);
}
