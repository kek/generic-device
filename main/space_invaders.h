#ifndef SPACE_INVADERS_H
#define SPACE_INVADERS_H

#include <stdint.h>
#include "st7735.h"

// Game dimensions
#define GAME_WIDTH   160
#define GAME_HEIGHT  128

// Player
#define PLAYER_WIDTH  8
#define PLAYER_HEIGHT 6
#define PLAYER_Y      (GAME_HEIGHT - 12)

// Aliens
#define ALIEN_ROWS    3
#define ALIEN_COLS    8
#define ALIEN_WIDTH   8
#define ALIEN_HEIGHT  6
#define ALIEN_SPACING 4

// Bullet
#define BULLET_WIDTH  2
#define BULLET_HEIGHT 4

/**
 * Run the Space Invaders demo
 * This is a simple animated demo showing game graphics
 */
void space_invaders_demo(void);

#endif // SPACE_INVADERS_H
