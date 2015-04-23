#pragma once

#include <pebble.h>
#include "pge/pge.h"

#define TITLE_LENGTH_MAX 32
#define TITLE_ACTION_MAX 16

/**
 * Show a pre-built title page with your game title and the background resource
 * Use the button ID in the click handler to navigate the title screen
 */
void title_push(PGEClickHandler *click_handler);

/**
 * Hide and destroy the title page
 */
void title_pop();

