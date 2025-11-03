#include "Player.h"
#include "ResourceManager.h"
#include "../GameConstants.h"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <cmath>

Player::Player()
{
}

bool Player::init()
{
    const sf::Texture* pTexture = ResourceManager::getOrLoadTexture("player.png");
    if (pTexture == nullptr)
        return false;

    m_pSprite = std::make_unique<sf::Sprite>(*pTexture);
    if (!m_pSprite)
        return false;

    m_rotation = sf::degrees(0);
    sf::FloatRect localBounds = m_pSprite->getLocalBounds();
    m_pSprite->setOrigin({localBounds.size.x / 2.0f, localBounds.size.y / 2.0f});
    m_pSprite->setPosition(m_position);
    m_pSprite->setScale(sf::Vector2f(3.0f, 3.0f));
    m_collisionRadius = collisionRadius;

    // Make sure vertical velocity starts at zero
    m_velocity.y = 0.0f;

    return true;
}

void Player::update(float dt)
{
    // Single-jump on space key press (edge-triggered). Use physics: velocity + gravity.
    const bool isSpacePressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space);

    // Consider player on ground if at or below GameConstants::GroundY
    const bool onGround = m_position.y >= GameConstants::GroundY - 0.1f;

    // If space was just pressed this frame and player is on ground, apply jump impulse
    if (isSpacePressed && !m_wasJumpKeyPressed && onGround)
    {
        m_velocity.y = -GameConstants::JumpVelocity;
    }

    // Save key state for edge detection next frame
    m_wasJumpKeyPressed = isSpacePressed;

    // Apply gravity
    m_velocity.y += GameConstants::Gravity * dt;

    // Integrate position
    m_position.y += m_velocity.y * dt;

    // Clamp to ground
    if (m_position.y > GameConstants::GroundY)
    {
        m_position.y = GameConstants::GroundY;
        m_velocity.y = 0.0f;
    }
}

void Player::render(sf::RenderTarget& target) const
{
    m_pSprite->setRotation(m_rotation);
    m_pSprite->setPosition(m_position);
    target.draw(*m_pSprite);
}
