#include "space_invaders.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <stdlib.h>
#include <stdio.h>

// Game state
static int16_t player_x = GAME_WIDTH / 2 - PLAYER_WIDTH / 2;
static int16_t player_x_old = GAME_WIDTH / 2 - PLAYER_WIDTH / 2;

static int16_t aliens_x[ALIEN_ROWS][ALIEN_COLS];
static int16_t aliens_y[ALIEN_ROWS][ALIEN_COLS];
static uint8_t aliens_alive[ALIEN_ROWS][ALIEN_COLS];
static int16_t alien_formation_x = 10;
static int16_t alien_formation_x_old = 10;
static int8_t alien_direction = 1;
static uint8_t alien_frame = 0;
static uint8_t alien_frame_old = 0;

static int16_t bullet_x = -1;
static int16_t bullet_y = -1;
static int16_t bullet_x_old = -1;
static int16_t bullet_y_old = -1;
static uint8_t bullet_active = 0;
static uint8_t bullet_active_old = 0;

static uint16_t score = 0;
static uint16_t score_old = 0;

static uint8_t first_frame = 1;

// Draw player ship
static void draw_player(int16_t x, int16_t y, uint16_t color) {
    // Simple ship shape
    st7735_fill_rect(x + 3, y, 2, 2, color);           // Top center
    st7735_fill_rect(x + 1, y + 2, 6, 2, color);       // Middle
    st7735_fill_rect(x, y + 4, 8, 2, color);           // Bottom
}

// Draw alien (two frames for animation)
static void draw_alien(int16_t x, int16_t y, uint8_t frame, uint16_t color) {
    if (frame == 0) {
        // Frame 0 - arms down
        st7735_fill_rect(x + 2, y, 4, 2, color);       // Head
        st7735_fill_rect(x, y + 2, 8, 2, color);       // Body
        st7735_fill_rect(x, y + 4, 2, 2, color);       // Left leg
        st7735_fill_rect(x + 6, y + 4, 2, 2, color);   // Right leg
    } else {
        // Frame 1 - arms up
        st7735_fill_rect(x + 2, y, 4, 2, color);       // Head
        st7735_fill_rect(x, y + 2, 8, 2, color);       // Body
        st7735_fill_rect(x + 1, y + 4, 2, 2, color);   // Left leg
        st7735_fill_rect(x + 5, y + 4, 2, 2, color);   // Right leg
    }
}

// Initialize game
static void init_game(void) {
    // Initialize aliens in formation
    for (int row = 0; row < ALIEN_ROWS; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            aliens_x[row][col] = col * (ALIEN_WIDTH + ALIEN_SPACING);
            aliens_y[row][col] = 16 + row * (ALIEN_HEIGHT + ALIEN_SPACING);
            aliens_alive[row][col] = 1;
        }
    }

    player_x = GAME_WIDTH / 2 - PLAYER_WIDTH / 2;
    player_x_old = player_x;
    alien_formation_x = 10;
    alien_formation_x_old = 10;
    alien_direction = 1;
    alien_frame = 0;
    alien_frame_old = 0;
    bullet_active = 0;
    bullet_active_old = 0;
    score = 0;
    score_old = 0;
    first_frame = 1;
}

// Update game state
static void update_game(void) {
    // Move alien formation
    alien_formation_x += alien_direction;

    // Check if aliens need to change direction and move down
    if (alien_formation_x <= 0 || alien_formation_x >= 40) {
        alien_direction = -alien_direction;
        for (int row = 0; row < ALIEN_ROWS; row++) {
            for (int col = 0; col < ALIEN_COLS; col++) {
                aliens_y[row][col] += 4;
            }
        }
    }

    // Toggle alien animation frame
    alien_frame = 1 - alien_frame;

    // Move player (simple left-right oscillation for demo)
    static int8_t player_dir = 1;
    player_x += player_dir * 2;
    if (player_x <= 0 || player_x >= GAME_WIDTH - PLAYER_WIDTH) {
        player_dir = -player_dir;
    }

    // Update bullet
    if (bullet_active) {
        bullet_y -= 4;
        if (bullet_y < 0) {
            bullet_active = 0;
        }

        // Check collision with aliens
        for (int row = 0; row < ALIEN_ROWS && bullet_active; row++) {
            for (int col = 0; col < ALIEN_COLS && bullet_active; col++) {
                if (aliens_alive[row][col]) {
                    int16_t ax = alien_formation_x + aliens_x[row][col];
                    int16_t ay = aliens_y[row][col];

                    if (bullet_x >= ax && bullet_x < ax + ALIEN_WIDTH &&
                        bullet_y >= ay && bullet_y < ay + ALIEN_HEIGHT) {
                        // Erase the alien immediately
                        draw_alien(ax, ay, alien_frame, ST77XX_BLACK);
                        aliens_alive[row][col] = 0;
                        bullet_active = 0;
                        score += 10;
                    }
                }
            }
        }
    } else {
        // Randomly shoot
        if ((rand() % 20) == 0) {
            bullet_x = player_x + PLAYER_WIDTH / 2;
            bullet_y = PLAYER_Y - 2;
            bullet_active = 1;
        }
    }

    // Check if all aliens destroyed (respawn)
    uint8_t any_alive = 0;
    for (int row = 0; row < ALIEN_ROWS; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            if (aliens_alive[row][col]) {
                any_alive = 1;
                break;
            }
        }
    }

    if (!any_alive) {
        // Reset aliens
        init_game();
    }
}

// Render game
static void render_game(void) {
    // Clear screen only on first frame
    if (first_frame) {
        st7735_fill_screen(ST77XX_BLACK);
        first_frame = 0;
    }

    // Update score if changed
    if (score != score_old) {
        // Clear old score area
        st7735_fill_rect(0, 0, GAME_WIDTH, 10, ST77XX_BLACK);

        // Draw new score
        char score_str[16];
        snprintf(score_str, sizeof(score_str), "SCORE:%04d", score);
        st7735_draw_string(2, 2, score_str, ST77XX_WHITE, ST77XX_BLACK, 1);

        score_old = score;
    }

    // Erase aliens at old positions if formation moved or frame changed
    if (alien_formation_x != alien_formation_x_old || alien_frame != alien_frame_old) {
        for (int row = 0; row < ALIEN_ROWS; row++) {
            for (int col = 0; col < ALIEN_COLS; col++) {
                if (aliens_alive[row][col]) {
                    int16_t ax_old = alien_formation_x_old + aliens_x[row][col];
                    int16_t ay = aliens_y[row][col];
                    draw_alien(ax_old, ay, alien_frame_old, ST77XX_BLACK);
                }
            }
        }
    }

    // Draw aliens at new positions
    for (int row = 0; row < ALIEN_ROWS; row++) {
        for (int col = 0; col < ALIEN_COLS; col++) {
            if (aliens_alive[row][col]) {
                int16_t ax = alien_formation_x + aliens_x[row][col];
                int16_t ay = aliens_y[row][col];

                // Different colors per row
                uint16_t color = ST77XX_GREEN;
                if (row == 1) color = ST77XX_YELLOW;
                if (row == 2) color = ST77XX_RED;

                draw_alien(ax, ay, alien_frame, color);
            }
        }
    }

    // Update old alien position tracking
    alien_formation_x_old = alien_formation_x;
    alien_frame_old = alien_frame;

    // Erase player at old position if moved
    if (player_x != player_x_old) {
        draw_player(player_x_old, PLAYER_Y, ST77XX_BLACK);
    }

    // Draw player at new position
    draw_player(player_x, PLAYER_Y, ST77XX_CYAN);
    player_x_old = player_x;

    // Erase old bullet if it moved or deactivated
    if (bullet_active_old && (bullet_x != bullet_x_old || bullet_y != bullet_y_old || !bullet_active)) {
        st7735_fill_rect(bullet_x_old, bullet_y_old, BULLET_WIDTH, BULLET_HEIGHT, ST77XX_BLACK);
    }

    // Draw new bullet if active
    if (bullet_active) {
        st7735_fill_rect(bullet_x, bullet_y, BULLET_WIDTH, BULLET_HEIGHT, ST77XX_WHITE);
    }

    // Update old bullet tracking
    bullet_x_old = bullet_x;
    bullet_y_old = bullet_y;
    bullet_active_old = bullet_active;
}

void space_invaders_demo(void) {
    init_game();

    while (1) {
        update_game();
        render_game();
        vTaskDelay(50 / portTICK_PERIOD_MS);  // ~20 FPS with partial updates
    }
}
