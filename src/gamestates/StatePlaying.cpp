#include "StatePlaying.h"
#include "StatePaused.h"
#include "StateStack.h"
#include "ResourceManager.h"
#include "../GameConstants.h"
#include <fstream>
#include <filesystem>
#include <sstream>
#include <memory>
#include <iostream>
#include <SFML/Graphics/RenderTarget.hpp>
#include <cmath>

StatePlaying::StatePlaying(StateStack& stateStack)
    : m_stateStack(stateStack)
{
}

bool StatePlaying::init()
{
    m_ground.setSize({1024.0f, 256.0f});
    m_ground.setPosition({0.0f, 800.0f});
    m_ground.setFillColor(sf::Color::Green);

    m_pPlayer = std::make_unique<Player>();
    if (!m_pPlayer || !m_pPlayer->init())
        return false;

    m_pPlayer->setPosition(sf::Vector2f(200, 800));

    // Load font for displaying distance/best
    const sf::Font* pFont = ResourceManager::getOrLoadFont("Lavigne.ttf");
    if (pFont != nullptr)
    {
        m_pDistanceText = std::make_unique<sf::Text>(*pFont);
        m_pBestText = std::make_unique<sf::Text>(*pFont);

        m_pDistanceText->setCharacterSize(24);
        m_pDistanceText->setStyle(sf::Text::Bold);
        m_pDistanceText->setFillColor(sf::Color::White);

        m_pBestText->setCharacterSize(18);
        m_pBestText->setStyle(sf::Text::Bold);
        m_pBestText->setFillColor(sf::Color::Yellow);
    }

    // Read personal best from disk (if available)
    {
        std::filesystem::path path = PersonalBestFile;
        if (std::filesystem::exists(path))
        {
            std::ifstream ifs(path);
            if (ifs)
            {
                ifs >> m_personalBestMeters;
            }
        }
    }

    return true;
}

void StatePlaying::update(float dt)
{
    m_timeUntilEnemySpawn -= dt;

    if (m_timeUntilEnemySpawn < 0.0f)
    {
        m_timeUntilEnemySpawn = enemySpawnInterval;
        std::unique_ptr<Enemy> pEnemy = std::make_unique<Enemy>();
        pEnemy->setPosition(sf::Vector2f(1000, 800));
        if (pEnemy->init())
            m_enemies.push_back(std::move(pEnemy));
    }

    // Update distance: assume world scroll speed corresponds to forward movement
    m_distanceMeters += (GameConstants::RunSpeed * dt) / GameConstants::PixelsPerMeter;

    // Update UI text strings
    if (m_pDistanceText)
    {
        std::ostringstream ss;
        ss << "Distance: " << static_cast<int>(std::floor(m_distanceMeters)) << " m";
        m_pDistanceText->setString(ss.str());
        m_pDistanceText->setPosition({10.0f, 10.0f});
    }
    if (m_pBestText)
    {
        std::ostringstream ss;
        ss << "Best: " << static_cast<int>(std::floor(std::max(m_personalBestMeters, m_distanceMeters))) << " m";
        m_pBestText->setString(ss.str());
        m_pBestText->setPosition({10.0f, 40.0f});
    }

    bool isPauseKeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
    m_hasPauseKeyBeenReleased |= !isPauseKeyPressed;
    if (m_hasPauseKeyBeenReleased && isPauseKeyPressed)
    {
        m_hasPauseKeyBeenReleased = false;
        m_stateStack.push<StatePaused>();
    }

    m_pPlayer->update(dt);

    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        pEnemy->update(dt);
    }

    // Detect collisions
    bool playerDied = false;
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
    {
        float distance = (m_pPlayer->getPosition() - pEnemy->getPosition()).lengthSquared();
        float minDistance = std::pow(Player::collisionRadius + pEnemy->getCollisionRadius(), 2.0f);
        const sf::Vector2f playerPosition = m_pPlayer->getPosition();

        if (distance <= minDistance)
        {
            playerDied = true;
            break;
        }
    }

    // End Playing State on player death
    if (playerDied)
    {
        // Update personal best if necessary and persist
        if (m_distanceMeters > m_personalBestMeters)
        {
            m_personalBestMeters = m_distanceMeters;
            std::filesystem::path path = PersonalBestFile;
            std::ofstream ofs(path);
            if (ofs)
            {
                ofs << static_cast<int>(std::floor(m_personalBestMeters));
            }
        }

        m_stateStack.popDeferred();
    }
}

void StatePlaying::render(sf::RenderTarget& target) const
{
    target.draw(m_ground);
    for (const std::unique_ptr<Enemy>& pEnemy : m_enemies)
        pEnemy->render(target);
    m_pPlayer->render(target);

    if (m_pDistanceText)
        target.draw(*m_pDistanceText);
    if (m_pBestText)
        target.draw(*m_pBestText);
}
