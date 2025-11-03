/* Small set of game constants used across gameplay code */
#pragma once

namespace GameConstants
{
    // Vertical acceleration (pixels per second squared)
    static constexpr float Gravity = 1000.0f;

    // Initial jump impulse (pixels per second)
    static constexpr float JumpVelocity = 600.0f;

    // Y position of the ground (player stands on this Y)
    static constexpr float GroundY = 800.0f;
}
