#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <map>
#include <string>
#include <memory>
#include <stdexcept>

class AssetManager {
public:
    AssetManager() = default;

    void loadTexture(const std::string& name, const std::string& fileName);
    sf::Texture& getTexture(const std::string& name);

    void loadFont(const std::string& name, const std::string& fileName);
    sf::Font& getFont(const std::string& name);

    void loadSoundBuffer(const std::string& name, const std::string& fileName);
    sf::SoundBuffer& getSoundBuffer(const std::string& name);

private:
    std::map<std::string, std::unique_ptr<sf::Texture>> m_textures;
    std::map<std::string, std::unique_ptr<sf::Font>> m_fonts;
    std::map<std::string, std::unique_ptr<sf::SoundBuffer>> m_sounds;
};
