#include "GameState.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <random>

GameState::GameState(Game &game)
    : m_game(game), m_pipeSpawnTimer(0.f), m_scoreText(m_font),
      m_flapSound(m_flapBuffer), m_titleShadowText(m_titleFont),
      m_titleText(m_titleFont), m_subtitleShadowText(m_font),
      m_subtitleText(m_font), m_startPromptText(m_font),
      m_gameOverShadowText(m_titleFont), m_gameOverText(m_titleFont),
      m_crashSound(m_crashBuffer), m_dingSound(m_dingBuffer), m_whooshSound(m_whooshBuffer), 
      m_dropdownMainText(m_uiFont),
      m_gravityButtonText(m_uiFont), m_logoSprite(m_logoTexture),
      m_bgSprite1(m_bgRenderTexture.getTexture()),
      m_bgSprite2(m_bgRenderTexture.getTexture()) {
  std::random_device rd;
  m_rng.seed(rd());
}

void GameState::init() {
  // Generate a 32x16 pixel-art bird (2 frames)
  const char *pixels[] = {
      "....YYYYYYYY........YYYYYYYY....", "...YYYWWWWYY.......YYYWWWWYY....",
      "..YYYYWBBWYYY.....YYYYWBBWYYY...", ".YYYYYWWWWYYYYY..YYYYYWWWWYYYYY.",
      ".YYWYYYYYYYOOOO..YYYYYYYYYYOOOO.", ".YYWWYYYYYOOOOO..YYYWWYYYYOOOOO.",
      "..YWWYYYYYYYYY....YYWWYYYYYYYY..", "...YYYYYYYYYY......YWWYYYYYYY...",
      "....YYYYYYYY........YYYYYYYY....", "................................",
      "................................", "................................",
      "................................", "................................",
      "................................", "................................"};

  sf::Image image;
  image.resize({32, 16}, sf::Color::Transparent);

  for (unsigned int y = 0; y < 16; ++y) {
    for (unsigned int x = 0; x < 32; ++x) {
      char c = pixels[y][x];
      if (c == 'Y')
        image.setPixel({x, y}, sf::Color::Yellow);
      else if (c == 'W')
        image.setPixel({x, y}, sf::Color::White);
      else if (c == 'B')
        image.setPixel({x, y}, sf::Color::Black);
      else if (c == 'O')
        image.setPixel({x, y}, sf::Color(255, 165, 0)); // Orange
    }
  }

  if (!m_birdTexture.loadFromImage(image)) {
    // Ignore for now
  }

  if (m_logoTexture.loadFromFile("logo.png")) {
    std::cerr << "SUCCESS: Loaded logo.png! Size: " << m_logoTexture.getSize().x
              << "x" << m_logoTexture.getSize().y << "\n";
    m_logoTexture.setSmooth(true);
    m_logoSprite.setTexture(m_logoTexture, true);
    m_logoSprite.setOrigin(
        {m_logoTexture.getSize().x / 2.f, m_logoTexture.getSize().y / 2.f});
    m_logoSprite.setPosition({400.f, 250.f});
    float scale = 700.f / m_logoTexture.getSize().x;
    if (scale < 1.f)
      m_logoSprite.setScale({scale, scale});

    sf::Image iconImage = m_logoTexture.copyToImage();
    m_game.getWindow().setIcon(iconImage.getSize(), iconImage.getPixelsPtr());
  } else {
    std::cerr << "FAILED to load logo.png from: "
              << std::filesystem::current_path().string() << "\n";
  }

  if (m_font.openFromFile("C:/Windows/Fonts/consola.ttf")) {
    m_scoreText.setFont(m_font);
    m_scoreText.setCharacterSize(20);
    m_scoreText.setFillColor(sf::Color::White);
    m_scoreText.setOutlineColor(sf::Color::Black);
    m_scoreText.setOutlineThickness(2.f);
    m_scoreText.setPosition({800.f - 250.f, 20.f}); // Roughly top right

    m_titleFont.openFromFile("C:/Windows/Fonts/impact.ttf");

    m_titleText.setFont(m_titleFont);
    m_titleText.setString("FLAPPY BIRD");
    m_titleText.setCharacterSize(100);
    m_titleText.setFillColor(sf::Color(255, 215, 0));   // Gold
    m_titleText.setOutlineColor(sf::Color(200, 50, 0)); // Dark Orange
    m_titleText.setOutlineThickness(6.f);
    m_titleText.setPosition(
        {800.f / 2.f - m_titleText.getGlobalBounds().size.x / 2.f, 100.f});

    m_titleShadowText = m_titleText;
    m_titleShadowText.setFillColor(sf::Color(0, 0, 0, 150));
    m_titleShadowText.setOutlineThickness(0);
    m_titleShadowText.setPosition(
        {m_titleText.getPosition().x + 8.f, m_titleText.getPosition().y + 8.f});

    m_subtitleText.setFont(m_font);
    m_subtitleText.setString("Designed by Jinendra Banthia");
    m_subtitleText.setCharacterSize(28);
    m_subtitleText.setFillColor(sf::Color(50, 255, 100)); // Neon Green
    m_subtitleText.setOutlineColor(sf::Color(0, 100, 0)); // Dark Green
    m_subtitleText.setOutlineThickness(3.f);
    m_subtitleText.setPosition(
        {800.f / 2.f - m_subtitleText.getGlobalBounds().size.x / 2.f, 230.f});

    m_subtitleShadowText = m_subtitleText;
    m_subtitleShadowText.setFillColor(sf::Color(0, 0, 0, 150));
    m_subtitleShadowText.setOutlineThickness(0);
    m_subtitleShadowText.setPosition({m_subtitleText.getPosition().x + 4.f,
                                      m_subtitleText.getPosition().y + 4.f});

    m_startPromptText.setFont(m_font);
    m_startPromptText.setString(">> Press SPACE to start! <<");
    m_startPromptText.setCharacterSize(35);
    m_startPromptText.setFillColor(sf::Color::White);
    m_startPromptText.setOutlineColor(sf::Color(200, 0, 0)); // Red Outline
    m_startPromptText.setOutlineThickness(4.f);
    m_startPromptText.setPosition(
        {800.f / 2.f - m_startPromptText.getGlobalBounds().size.x / 2.f,
         400.f});

    m_gameOverText.setFont(m_titleFont);
    m_gameOverText.setString("GAME OVER");
    m_gameOverText.setCharacterSize(100);
    m_gameOverText.setFillColor(sf::Color(255, 50, 50)); // Red
    m_gameOverText.setOutlineColor(sf::Color::White);
    m_gameOverText.setOutlineThickness(4.f);
    m_gameOverText.setPosition(
        {800.f / 2.f - m_gameOverText.getGlobalBounds().size.x / 2.f, 150.f});

    m_gameOverShadowText = m_gameOverText;
    m_gameOverShadowText.setFillColor(sf::Color(0, 0, 0, 150));
    m_gameOverShadowText.setOutlineThickness(0);
    m_gameOverShadowText.setPosition({m_gameOverText.getPosition().x + 8.f,
                                      m_gameOverText.getPosition().y + 8.f});

    m_uiFont.openFromFile("C:/Windows/Fonts/consola.ttf");

    // Dropdown UI - Sleek Minimalist Style
    m_dropdownMainBox.setSize({160.f, 35.f});
    m_dropdownMainBox.setFillColor(sf::Color::Transparent);
    m_dropdownMainBox.setOutlineColor(sf::Color::Transparent);
    m_dropdownMainBox.setOutlineThickness(0.f);
    m_dropdownMainBox.setPosition({20.f, 20.f});

    m_dropdownMainText.setFont(m_font);
    m_dropdownMainText.setCharacterSize(18);
    m_dropdownMainText.setFillColor(sf::Color::White);
    m_dropdownMainText.setOutlineColor(sf::Color::Black);
    m_dropdownMainText.setOutlineThickness(2.f);
    m_dropdownMainText.setPosition({30.f, 26.f});
    m_dropdownMainText.setString("Start Level: 1  v");

    for (int i = 1; i <= 8; ++i) {
      sf::RectangleShape box({160.f, 30.f});
      box.setFillColor(sf::Color(0, 0, 0, 150));
      box.setOutlineColor(sf::Color(255, 255, 255, 50)); // Very faint outline
      box.setOutlineThickness(1.f);
      box.setPosition({20.f, 20.f + 35.f + (i - 1) * 30.f});
      m_dropdownItemBoxes.push_back(box);

      sf::Text text(m_font);
      text.setCharacterSize(18);
      text.setFillColor(sf::Color::White);
      text.setOutlineColor(sf::Color::Black);
      text.setOutlineThickness(2.f);
      text.setString("Level " + std::to_string(i));
      text.setPosition({30.f, 20.f + 35.f + (i - 1) * 30.f + 5.f});
      m_dropdownItemTexts.push_back(text);
    }

    m_gravityButtonBox.setSize({160.f, 35.f});
    m_gravityButtonBox.setFillColor(sf::Color::Transparent);
    m_gravityButtonBox.setOutlineColor(sf::Color::Transparent);
    m_gravityButtonBox.setOutlineThickness(0.f);
    m_gravityButtonBox.setPosition({200.f, 20.f});

    m_gravityButtonText.setFont(m_font);
    m_gravityButtonText.setCharacterSize(18);
    m_gravityButtonText.setFillColor(sf::Color::White);
    m_gravityButtonText.setOutlineColor(sf::Color::Black);
    m_gravityButtonText.setOutlineThickness(2.f);
    m_gravityButtonText.setPosition({210.f, 26.f});
    m_gravityButtonText.setString("Gravity: Normal");
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
    float wave = std::sin(phase) + 0.5f * std::sin(phase * 2.f) +
                 0.25f * std::sin(phase * 3.f);
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
  m_flapBuffer.loadFromSamples(samples.data(), samples.size(), 1, sampleRate,
                               {sf::SoundChannel::Mono});
  m_flapSound.setBuffer(m_flapBuffer);

    // Procedural Ding sound
    std::vector<std::int16_t> dingSamples;
    for (int i = 0; i < sampleRate * 0.2f; ++i) { // 200ms ding
        float t = (float)i / sampleRate;
        float envelope = std::exp(-t * 15.0f); // Fast decay
        float wave = std::sin(2.0f * 3.14159f * 800.0f * t); // 800Hz sine wave
        dingSamples.push_back(wave * 20000 * envelope);
    }
    m_dingBuffer.loadFromSamples(dingSamples.data(), dingSamples.size(), 1, sampleRate, {sf::SoundChannel::Mono});
    m_dingSound.setBuffer(m_dingBuffer);

    // Procedural Whoosh sound
    std::vector<std::int16_t> whooshSamples;
    for (int i = 0; i < sampleRate * 0.4f; ++i) { // 400ms whoosh
        float t = (float)i / sampleRate;
        float envelope = std::sin(t / 0.4f * 3.14159f); // Smooth sine envelope
        float noise = ((m_rng() % 32767) - 16384) / 32768.f;
        
        // Low pass filter approximation
        static float lastOut = 0;
        float cutoff = 0.1f + envelope * 0.4f; // Filter opens and closes
        lastOut += (noise - lastOut) * cutoff;
        
        whooshSamples.push_back(lastOut * 25000 * envelope);
    }
    m_whooshBuffer.loadFromSamples(whooshSamples.data(), whooshSamples.size(), 1, sampleRate, {sf::SoundChannel::Mono});
    m_whooshSound.setBuffer(m_whooshBuffer);

  // Generate procedural crash sound
  std::vector<std::int16_t> crashSamples;
  for (int i = 0; i < sampleRate * 0.3f; ++i) { // 300ms crash
    float t = (float)i / sampleRate;
    float envelope = 1.0f - (t / 0.3f);
    envelope = envelope * envelope * envelope; // Sharp exponential decay
    short noise = (m_rng() % 32767) - 16384;
    crashSamples.push_back(noise * envelope);
  }
  m_crashBuffer.loadFromSamples(crashSamples.data(), crashSamples.size(), 1,
                                sampleRate, {sf::SoundChannel::Mono});
  m_crashSound.setBuffer(m_crashBuffer);

  std::ifstream inFile("highscore.txt");
  if (inFile.is_open()) {
    inFile >> m_highScore;
    inFile.close();
  }

  if (m_bgRenderTexture.resize({800u, 600u})) {
    m_bgRenderTexture.clear(sf::Color(135, 206, 235)); // Sky Blue

    const char *cloudPixels[] = {
        "..........WWWW..............",   "........WWWWWWWW............",
        "......WWWWWWWWWWWW...WW.....",   "....WWWWWWWWWWWWWWWWWWWW....",
        "...WWWWWWWWWWWWWWWWWWWWWW...",   "..WWWWWWWWWWWWWWWWWWWWWWWW..",
        ".WWWWWWWWWWWWWWWWWWWWWWWWWW.",   "WWWWWWWWWWWWWWWWWWWWWWWWWWWW",
        "WWWWWWWWWWWWWWWWWWWWWWWWWWWWWW", "WLLLLLLLLLLLLLLLLLLLLLLLLLLWW",
        "LLLLLLLLLLLLLLLLLLLLLLLLLLLL",   ".LLLLLLLLLLLLLLLLLLLLLLLLLL."};

    sf::Image cloudImage;
    cloudImage.resize({28, 12}, sf::Color::Transparent);
    for (unsigned int y = 0; y < 12; ++y) {
      for (unsigned int x = 0; x < 28; ++x) {
        if (cloudPixels[y][x] == 'W')
          cloudImage.setPixel({x, y}, sf::Color::White);
        else if (cloudPixels[y][x] == 'L')
          cloudImage.setPixel({x, y}, sf::Color(220, 220, 220)); // Light Gray
      }
    }
    sf::Texture cloudTex;
    cloudTex.loadFromImage(cloudImage);
    sf::Sprite cloudSprite(cloudTex);

    auto drawCloud = [&](float x, float y, float scale) {
      cloudSprite.setPosition({x, y});
      cloudSprite.setScale(
          {scale * 6.f, scale * 6.f}); // Scale up the pixel art
      m_bgRenderTexture.draw(cloudSprite);
    };

    drawCloud(100.f, 100.f, 1.2f);
    drawCloud(450.f, 50.f, 1.5f);
    drawCloud(700.f, 180.f, 0.8f);
    drawCloud(250.f, 250.f, 0.6f);

    sf::RectangleShape ground({800.f, 100.f});
    ground.setPosition({0.f, 500.f});
    ground.setFillColor(sf::Color(34, 139, 34)); // Forest Green
    m_bgRenderTexture.draw(ground);

    sf::RectangleShape groundTrim({800.f, 10.f});
    groundTrim.setPosition({0.f, 500.f});
    groundTrim.setFillColor(sf::Color(124, 252, 0)); // Lawn Green
    m_bgRenderTexture.draw(groundTrim);

    m_bgRenderTexture.display();

    m_bgSprite1.setTexture(m_bgRenderTexture.getTexture(), true);
    m_bgSprite2.setTexture(m_bgRenderTexture.getTexture(), true);
    m_bgSprite2.setPosition({800.f, 0.f});
  }

  reset();
}

void GameState::handleInput() {
  bool isSpace = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);
  if (m_bird)
    m_bird->setJumpHeld(isSpace);

  // Mouse Input for Dropdown
  sf::Vector2i mousePos = sf::Mouse::getPosition(m_game.getWindow());
  static bool mousePressed = false;
  bool isMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

  if (isMouseDown && !mousePressed) {
    mousePressed = true;
    if (!m_bird->hasStarted() || m_isGameOver) {
      if (m_gravityButtonBox.getGlobalBounds().contains(
              static_cast<sf::Vector2f>(mousePos))) {
        m_isAntiGravityMode = !m_isAntiGravityMode;
        m_gravityButtonText.setString(m_isAntiGravityMode ? "Gravity: Anti"
                                                          : "Gravity: Normal");
        if (m_bird) {
          m_bird->setAntiGravityImmediate(m_isAntiGravityMode);
        }
      }

      if (m_dropdownMainBox.getGlobalBounds().contains(
              static_cast<sf::Vector2f>(mousePos))) {
        m_isDropdownOpen = !m_isDropdownOpen;
      } else if (m_isDropdownOpen) {
        bool clickedItem = false;
        for (size_t i = 0; i < m_dropdownItemBoxes.size(); ++i) {
          if (m_dropdownItemBoxes[i].getGlobalBounds().contains(
                  static_cast<sf::Vector2f>(mousePos))) {
            m_selectedStartLevel = i + 1;
            m_dropdownMainText.setString(
                "Start Level: " + std::to_string(m_selectedStartLevel) + "  v");
            m_currentLevel = m_selectedStartLevel + (m_score / 10);
            m_isDropdownOpen = false;
            clickedItem = true;
            break;
          }
        }
        if (!clickedItem)
          m_isDropdownOpen = false;
      }
    }
  } else if (!isMouseDown) {
    mousePressed = false;
  }

  // Hover effects
  if (!m_bird->hasStarted() || m_isGameOver) {
    if (m_dropdownMainBox.getGlobalBounds().contains(
            static_cast<sf::Vector2f>(mousePos))) {
      m_dropdownMainBox.setFillColor(sf::Color::Transparent); // Hover sleek
    } else {
      m_dropdownMainBox.setFillColor(sf::Color::Transparent);
    }

    if (m_isDropdownOpen) {
      for (size_t i = 0; i < m_dropdownItemBoxes.size(); ++i) {
        if (m_dropdownItemBoxes[i].getGlobalBounds().contains(
                static_cast<sf::Vector2f>(mousePos))) {
          m_dropdownItemBoxes[i].setFillColor(
              sf::Color(60, 60, 60, 220)); // Highlight item
          m_dropdownItemTexts[i].setFillColor(sf::Color::White);
        } else {
          m_dropdownItemBoxes[i].setFillColor(sf::Color(0, 0, 0, 150));
          m_dropdownItemTexts[i].setFillColor(sf::Color(200, 200, 200));
        }
      }
    }
  }

  static bool spacePressed = false;

  if (isSpace) {
    if (!spacePressed) {
      if (m_isDropdownOpen) {
        m_isDropdownOpen = false; // Close dropdown instead of starting if open
      } else if (m_isGameOver) {
        reset();
        // Ensure level respects dropdown on reset
        m_currentLevel = m_selectedStartLevel;
      } else if (m_bird) {
        m_bird->jump();
        float pitch = 0.95f + (m_rng() % 10) / 100.f; // Random 0.95 to 1.05
        m_flapSound.setPitch(pitch);
        m_flapSound.play();
        
        // Emit particles
        sf::Vector2f birdPos = m_bird->getPosition();
        for (int i = 0; i < 8; ++i) {
          Particle p;
          p.pos = birdPos;
          p.vel = { ((m_rng() % 100) - 50.f) * 3.f, ((m_rng() % 100) - 50.f) * 3.f };
          p.life = 0.f;
          p.maxLife = 0.3f + (m_rng() % 10) / 50.f;
          m_particles.push_back(p);
        }
      }  spacePressed = true;
    }
  } else {
    spacePressed = false;
  }

  static bool shiftPressed = false;
  if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LShift)) {
    if (!shiftPressed && m_bird && m_currentLevel >= 6) {
      m_bird->dash();
    }
    shiftPressed = true;
  } else {
    shiftPressed = false;
  }

  // Manual Level Changing (Keys 1-9)
  if (!m_bird->hasStarted() || m_isGameOver) {
    for (int i = 0; i < 9; ++i) {
      static bool numPressed[9] = {false};
      if (sf::Keyboard::isKeyPressed(static_cast<sf::Keyboard::Key>(
              static_cast<int>(sf::Keyboard::Key::Num1) + i))) {
        if (!numPressed[i]) {
          m_selectedStartLevel = i + 1;
          m_currentLevel = m_selectedStartLevel + (m_score / 10);
          m_dropdownMainText.setString(
              "Start Level: " + std::to_string(m_selectedStartLevel) + "  v");
          numPressed[i] = true;
        }
      } else {
        numPressed[i] = false;
      }
    }
  }
}

void GameState::update(sf::Time dt) {
    if (m_hitStopTimer > 0.f) {
        m_hitStopTimer -= dt.asSeconds();
        return; // Freeze time!
    }

    if (m_screenShakeTimer > 0.f) {
        m_screenShakeTimer -= dt.asSeconds();
    }
    
    if (m_deathFlashTimer > 0.f) {
        m_deathFlashTimer -= dt.asSeconds();
    }

    // Update Particles
    for (auto& p : m_particles) {
        p.life += dt.asSeconds();
        p.pos += p.vel * dt.asSeconds();
    }
    m_particles.erase(std::remove_if(m_particles.begin(), m_particles.end(), [](const Particle& p) { return p.life >= p.maxLife; }), m_particles.end());

    if (m_bird) {
        if (!m_bird->hasStarted() && !m_isGameOver) {
            // Idle bobbing on menu
            m_menuTimer += dt.asSeconds();
            float floatOffset = std::sin(m_menuTimer * 4.f) * 10.f;
            m_bird->setPosition({m_bird->getPosition().x, 300.f + floatOffset});
        }
        
        m_bird->update(dt);
        
        if (m_bird->hasStarted() && !m_isGameOver) {
      m_timeAlive += dt.asSeconds();

      // Level Progression
      int newLevel = m_selectedStartLevel + (m_score / 10);
      if (newLevel != m_currentLevel) {
        m_currentLevel = newLevel;
        if (m_currentLevel >= 3) {
          m_bird->setScreenWrap(true);
        }
      }

      // Environment: Underwater
      if (m_currentLevel == 4 || m_currentLevel == 7) {
        m_bgSprite1.setColor(sf::Color(100, 150, 255));
        m_bgSprite2.setColor(sf::Color(100, 150, 255));
        m_bird->setUnderwater(true);
      } else {
        m_bgSprite1.setColor(sf::Color::White);
        m_bgSprite2.setColor(sf::Color::White);
        m_bird->setUnderwater(false);
      }

      // Environment: Wind
      if (m_currentLevel == 5 || m_currentLevel == 8) {
        m_bird->applyWind(-50.f * dt.asSeconds());
      }

      // Environment: Speed Boosters
      if (m_currentLevel >= 6) {
        m_boosterSpawnTimer += dt.asSeconds();
        if (m_boosterSpawnTimer > 4.f) {
          m_boosterSpawnTimer = 0.f;
          SpeedBooster b;
          b.shape.setRadius(15.f);
          b.shape.setFillColor(sf::Color::Transparent);
          b.shape.setOutlineColor(sf::Color::Cyan);
          b.shape.setOutlineThickness(4.f);
          b.shape.setOrigin({15.f, 15.f});
          b.shape.setPosition({850.f, 100.f + (m_rng() % 400)});
          m_boosters.push_back(b);
        }
      }

      sf::FloatRect birdBounds = m_bird->getGlobalBounds();

      // Update Boosters
      for (auto &b : m_boosters) {
        b.shape.move({-200.f * m_speedMultiplier * dt.asSeconds(), 0.f});
        if (b.active &&
            b.shape.getGlobalBounds().findIntersection(birdBounds)) {
          b.active = false;
          m_speedMultiplier = 1.5f;
          m_speedBoostTimer = 3.f;
        }
      }
      m_boosters.erase(
          std::remove_if(m_boosters.begin(), m_boosters.end(),
                         [](const SpeedBooster &b) {
                           return b.shape.getPosition().x < -50.f || !b.active;
                         }),
          m_boosters.end());

      // Gravity Zones
      if (m_currentLevel >= 3) {
          m_zoneSpawnTimer += dt.asSeconds();
          if (m_zoneSpawnTimer > 7.f) {
              m_zoneSpawnTimer = 0.f;
              GravityZone z;
              z.type = (m_rng() % 2); // 0 = Inversion, 1 = Zero-G
              z.shape.setSize({150.f, 600.f});
              z.shape.setPosition({850.f, 0.f});
              sf::Color c = (z.type == 0) ? sf::Color(255, 0, 255, 80) : sf::Color(0, 255, 255, 80);
              z.shape.setFillColor(c);
              z.bounds = z.shape.getGlobalBounds();
              m_gravityZones.push_back(z);
          }
      }

      bool inZeroG = false;
      for (auto &z : m_gravityZones) {
          z.shape.move({-200.f * m_speedMultiplier * dt.asSeconds(), 0.f});
          z.bounds = z.shape.getGlobalBounds();
          if (z.bounds.findIntersection(birdBounds)) {
              if (z.type == 0 && z.active) {
                  // Inversion - Only trigger once
                  z.active = false;
                  m_isAntiGravityMode = !m_isAntiGravityMode;
                  m_bird->setAntiGravityImmediate(m_isAntiGravityMode);
                  m_whooshSound.play();
                  m_screenShakeTimer = 0.2f;
              } else if (z.type == 1) {
                  // Zero-G Zone! Continually triggers while inside
                  inZeroG = true;
              }
          }
      }
      m_bird->setZeroGravity(inZeroG);
      m_gravityZones.erase(
          std::remove_if(m_gravityZones.begin(), m_gravityZones.end(),
                         [](const GravityZone &z) {
                           return z.shape.getPosition().x < -200.f;
                         }),
          m_gravityZones.end());

      // Dynamic Trails
      m_trailTimer += dt.asSeconds();
      if (m_trailTimer > 0.03f) {
          m_trailTimer = 0.f;
          Particle p;
          p.pos = m_bird->getPosition();
          p.vel = { -100.f, ((m_rng() % 20) - 10.f) };
          p.life = 0.f;
          p.maxLife = 0.5f;
          p.color = m_bird->isAntiGravity() ? sf::Color(255, 100, 100, 150) : sf::Color(100, 200, 255, 150);
          m_particles.push_back(p);
      }

      if (m_speedBoostTimer > 0.f) {
        m_speedBoostTimer -= dt.asSeconds();
        if (m_speedBoostTimer <= 0.f)
          m_speedMultiplier = 1.0f;
      }

      m_scoreText.setString(
          "Level " + std::to_string(m_currentLevel) + " | High: " +
          std::to_string(m_highScore) + " | Score: " + std::to_string(m_score) +
          " | Time: " + std::to_string(static_cast<int>(m_timeAlive)) + "s");
      m_scoreText.setPosition(
          {800.f - m_scoreText.getGlobalBounds().size.x - 20.f, 20.f});

      // Update background scroll
      m_bgScrollX -= 30.f * m_speedMultiplier * dt.asSeconds();
      if (m_bgScrollX <= -800.f)
        m_bgScrollX += 800.f;

      m_bgSprite1.setPosition({m_bgScrollX, 0.f});
      m_bgSprite2.setPosition({m_bgScrollX + 800.f, 0.f});

      // Update pipes
      m_pipeSpawnTimer += dt.asSeconds() * m_speedMultiplier;
      if (m_pipeSpawnTimer >= m_currentPipeSpawnInterval) {
        spawnPipe();
        m_pipeSpawnTimer = 0.f;
      }
    } else if (!m_bird->hasStarted() || m_isGameOver) {
      // Blinking effect for start/restart text
      m_menuTimer += dt.asSeconds();
      float alpha = (std::sin(m_menuTimer * 5.f) + 1.f) / 2.f * 255.f;
      m_startPromptText.setFillColor(
          sf::Color(255, 255, 255, static_cast<std::uint8_t>(alpha)));
      m_startPromptText.setOutlineColor(
          sf::Color(200, 0, 0, static_cast<std::uint8_t>(alpha)));

      // Bouncing / Floating animation for the title
      float floatOffset = std::sin(m_menuTimer * 3.f) * 15.f;
      m_titleText.setPosition(
          {800.f / 2.f - m_titleText.getGlobalBounds().size.x / 2.f,
           100.f + floatOffset});
      m_titleShadowText.setPosition({m_titleText.getPosition().x + 8.f,
                                     m_titleText.getPosition().y + 8.f});

      if (m_logoTexture.getSize().x > 0) {
        m_logoSprite.setPosition({400.f, 250.f + floatOffset});
      }
    }

    if (!m_isGameOver) {
      // Collision detection
      sf::FloatRect birdBounds = m_bird->getGlobalBounds();
      bool collision = false;

      // Floor and ceiling (disabled if level >= 3 for screen wrap)
      if (m_currentLevel < 3 && !m_bird->isDashing()) {
        if (birdBounds.position.y < 0.f ||
            birdBounds.position.y + birdBounds.size.y > 600.f) {
          collision = true;
        }
      }

      for (auto &pipe : m_pipes) {
        pipe.update(dt * m_speedMultiplier);

        // Fake-Out Logic
        if (pipe.getType() == Pipe::Type::FakeOut && birdBounds.position.x > pipe.getX() - 350.f) {
            if (pipe.getTargetGapY() == pipe.getGapY()) { // Has not triggered yet
                float shift = (m_rng() % 2 == 0) ? -150.f : 150.f;
                // Keep it in screen bounds
                float newGapY = pipe.getGapY() + shift;
                if (newGapY < 150.f) newGapY = 150.f;
                if (newGapY > 450.f) newGapY = 450.f;
                pipe.setTargetGapY(newGapY);
            }
        }

        if (birdBounds.findIntersection(pipe.getTopBounds()) ||
            birdBounds.findIntersection(pipe.getBottomBounds())) {
          if (m_bird->isDashing() && pipe.getType() == Pipe::Type::Breakable) {
            pipe.breakPipe();
            m_score += 2;        // Bonus points
            m_crashSound.play(); // Play sound for breaking wood
          } else if (!m_bird->isDashing()) {
            collision = true;
          }
        }

        if (!pipe.isPassed() && birdBounds.position.x > pipe.getX() + 80.f) {
          pipe.setPassed(true);
          m_score++;
                    
          // Dynamic Musical Scoring
          float dingPitch = 1.0f + (m_score % 10) * 0.05f; 
          m_dingSound.setPitch(dingPitch);
          m_dingSound.play();
        }
      }

      // Remove offscreen pipes
      m_pipes.erase(
          std::remove_if(m_pipes.begin(), m_pipes.end(),
                         [](const Pipe &p) { return p.isOffScreen(); }),
          m_pipes.end());

      if (collision) {
        m_isGameOver = true;
        m_crashSound.play();
        m_hitStopTimer = 0.1f;
        m_screenShakeTimer = 0.3f;
        m_deathFlashTimer = 1.0f;
        if (m_score > m_highScore) {
          m_highScore = m_score;
          std::ofstream outFile("highscore.txt");
          outFile << m_highScore;
        }
        m_startPromptText.setString(">> Press SPACE to restart! <<");
        m_startPromptText.setPosition(
            {800.f / 2.f - m_startPromptText.getGlobalBounds().size.x / 2.f,
             400.f});
      }
    }
  }
}

void GameState::draw(sf::RenderTarget &target, float alpha) {
  sf::View originalView = target.getView();
  if (m_screenShakeTimer > 0.f) {
      sf::View shakeView = originalView;
      float magnitude = m_screenShakeTimer * 30.f; 
      float offsetX = ((m_rng() % 100) / 100.f * 2.f - 1.f) * magnitude;
      float offsetY = ((m_rng() % 100) / 100.f * 2.f - 1.f) * magnitude;
      shakeView.move({offsetX, offsetY});
      target.setView(shakeView);
  }

  target.draw(m_bgSprite1);
  target.draw(m_bgSprite2);

  for (auto &pipe : m_pipes) {
    pipe.draw(target, alpha);
  }

  for (auto &b : m_boosters) {
    if (b.active)
      target.draw(b.shape);
  }

  if (m_bird) {
    m_bird->draw(target, alpha);
  }
  
  // Draw Gravity Zones
  for (auto &z : m_gravityZones) {
    if (z.active)
      target.draw(z.shape);
  }

  // Draw particles
  for (const auto& p : m_particles) {
      sf::RectangleShape pr({4.f, 4.f});
      pr.setOrigin({2.f, 2.f});
      pr.setPosition(p.pos);
      float lifeRatio = 1.0f - (p.life / p.maxLife);
      sf::Color c = p.color;
      c.a = static_cast<std::uint8_t>(255 * lifeRatio);
      pr.setFillColor(c);
      target.draw(pr);
  }

  // Restore view before drawing UI and Fullscreen effects
  target.setView(originalView);

  // Draw Death Flash
  if (m_deathFlashTimer > 0.f) {
      sf::RectangleShape flash({800.f, 600.f});
      float lifeRatio = m_deathFlashTimer / 1.0f; // 1.0 to 0.0
      std::uint8_t r = 255;
      std::uint8_t g = static_cast<std::uint8_t>(255 * lifeRatio); // White fades to Red
      std::uint8_t b = static_cast<std::uint8_t>(255 * lifeRatio);
      std::uint8_t a = static_cast<std::uint8_t>(255 * lifeRatio);
      flash.setFillColor(sf::Color(r, g, b, a));
      target.draw(flash);
  }

  target.draw(m_scoreText);

  if (!m_bird->hasStarted()) {
    if (m_logoTexture.getSize().x > 0) {
      target.draw(m_logoSprite);
    } else {
      target.draw(m_titleShadowText);
      target.draw(m_titleText);
      target.draw(m_subtitleShadowText);
      target.draw(m_subtitleText);
      target.draw(m_startPromptText);
    }
  } else if (m_isGameOver) {
    target.draw(m_gameOverShadowText);
    target.draw(m_gameOverText);
    target.draw(m_startPromptText);
  }

  // Draw Dropdown UI (drawn over everything else so it's clickable)
  if (!m_bird->hasStarted() || m_isGameOver) {
    target.draw(m_gravityButtonBox);
    target.draw(m_gravityButtonText);
    target.draw(m_dropdownMainBox);
    target.draw(m_dropdownMainText);
    if (m_isDropdownOpen) {
      for (size_t i = 0; i < m_dropdownItemBoxes.size(); ++i) {
        target.draw(m_dropdownItemBoxes[i]);
        target.draw(m_dropdownItemTexts[i]);
      }
    }
  }
}

void GameState::reset() {
  m_bird = std::make_unique<Bird>(m_birdTexture);
  m_bird->setAntiGravityImmediate(m_isAntiGravityMode);
  m_pipes.clear();
  m_boosters.clear();
  m_gravityZones.clear();
  m_particles.clear();
  m_pipeSpawnTimer = 0.f;
  m_score = 0;
  m_timeAlive = 0.f;
  m_isGameOver = false;
  m_speedMultiplier = 1.0f;
  m_speedBoostTimer = 0.f;
  m_currentLevel = m_selectedStartLevel;
  m_currentPipeSpawnInterval = m_basePipeSpawnInterval;

  m_startPromptText.setString(">> Press SPACE to start! <<");
  m_startPromptText.setPosition(
      {800.f / 2.f - m_startPromptText.getGlobalBounds().size.x / 2.f, 400.f});
}

void GameState::spawnPipe() {
  float gapY = 150.f + (m_rng() % 300);
  
  // Difficulty scaling: Gap gets narrower as score increases
  float baseGapSize = 150.f;
  float gapSize = std::max(90.f, baseGapSize - (m_score * 1.5f));
  
  Pipe::Type type = Pipe::Type::Normal;
  if (m_currentLevel >= 2) {
    int r = m_rng() % 100;
    if (r < 20) type = Pipe::Type::Moving;
    else if (r < 30) type = Pipe::Type::Crushing;
    else if (r < 40) type = Pipe::Type::Diagonal;
    else if (r < 50) type = Pipe::Type::Breakable;
    else if (r < 60) type = Pipe::Type::FakeOut;
  }
  
  Pipe newPipe(800.f, gapY, gapSize, type);
  m_pipes.push_back(newPipe);
}
