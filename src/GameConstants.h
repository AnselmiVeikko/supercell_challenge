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

    // Horizontal world scroll speed (pixels per second) used to compute distance
    static constexpr float RunSpeed = 200.0f;

    // How many pixels correspond to one meter (for distance display)
    static constexpr float PixelsPerMeter = 50.0f;
}
