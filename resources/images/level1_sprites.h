#pragma once

#include "sprintracer_level.h"

#define LEVEL1_BOUNDS GRect(0, 0, SCREEN_RES_COLS*4, SCREEN_RES_ROWS*4)

#define LEVEL1_CAR_STARTING_ANGLE 0

// Used for sprite collision - drawn in order of index in array
static LevelSpriteLocation level1_sprites[] = {
  { RESOURCE_ID_HORIZ_SHRUB_24, {8, 173}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {8, 81}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {8, 265}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {160, 205}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {160, 170}, NULL },
  { RESOURCE_ID_SHRUB_8, {128, 148}, NULL },
  { RESOURCE_ID_SHRUB_8, {136, 155}, NULL },
  { RESOURCE_ID_SHRUB_8, {143, 162}, NULL },
  { RESOURCE_ID_SHRUB_8, {148, 170}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {196, 98}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {146, 8}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {32, 81}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {32, 177}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {196, 122}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {204, 242}, NULL },
  { RESOURCE_ID_SHRUB_8, {228, 250}, NULL },
  { RESOURCE_ID_SHRUB_8, {236, 259}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {246, 276}, NULL },
  { RESOURCE_ID_SHRUB_8, {243, 267}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {148, 226}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {144, 254}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {104, 270}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {104, 247}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {144, 278}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {120, 247}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {103, 143}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {104, 247}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {120, 294}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {104, 294}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {10, 315}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {270, 65}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {196, 298}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {148, 178}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {96, 98}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {190, 146}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {156, 242}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {55, 196}, NULL },
};

static LevelSpriteLocation level1_scenery[] = {
  { RESOURCE_ID_VERT_WATER_20_80, {10, 90}, NULL },
  { RESOURCE_ID_VERT_WATER_20_40, {125, 255}, NULL },
  { RESOURCE_ID_VERT_WATER_20_40, {111, 255}, NULL },
  { RESOURCE_ID_VERT_WATER_20_80, {10, 185}, NULL },
};

static LevelSpriteLocation level1_walls[] = {
  { RESOURCE_ID_HORIZ_WALL_144, {0, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_24, {288, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {0, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_24, {288, 352}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_24, {0, 336}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {304, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {304, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_24, {304, 336}, NULL },
};

// Track layout - drawn in order of index in array
static LevelSpriteLocation level1_tracks[] = {
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {150, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {150, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {200, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {100, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {250, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {100, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {250, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {200, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {100, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {150, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {100, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {200, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {150, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {200, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 300}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 300}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {50, 100}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {250, 250}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {200, 100}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {200, 150}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {50, 250}, NULL },
};

static LevelSpriteLocation level1_checkpoints[] = {
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {35, 130}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {130, 280}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {185, 180}, NULL },
};

static LevelFinishLine level1_finish_group = {
  { RESOURCE_ID_FINISH_LINE_HORIZ, {205, 100}, NULL },
  { RESOURCE_ID_FINISH_BOX_1, {250, 100}, NULL },
  { RESOURCE_ID_FINISH_BOX_2, {250, 120}, NULL },
  { RESOURCE_ID_FINISH_BOX_3, {250, 140}, NULL },
  { RESOURCE_ID_FINISH_BOX_4, {250, 160}, NULL },
  { RESOURCE_ID_LIGHT_SIGNAL_ALL, {255, 80}, NULL },
};

static GPoint level1_track_points0[] = {
  {225, 90},
  {220, 75},
  {205, 70},
  {190, 70},
  {175, 75},
  {170, 90},
  {175, 105},
  {170, 120},
  {155, 125},
  {140, 125},
  {125, 120},
  {120, 105},
  {125, 90},
  {120, 75},
  {105, 70},
  {90, 70},
  {75, 75},
  {70, 90},
  {70, 105},
  {70, 140},
  {70, 155},
  {75, 170},
  {90, 175},
  {105, 170},
  {120, 175},
  {125, 190},
  {125, 205},
  {120, 220},
  {105, 225},
  {90, 220},
  {75, 225},
  {70, 240},
  {70, 255},
  {70, 290},
  {70, 305},
  {75, 320},
  {90, 325},
  {105, 325},
  {140, 325},
  {155, 325},
  {170, 320},
  {175, 305},
  {170, 290},
  {175, 275},
  {190, 270},
  {205, 270},
  {220, 275},
  {225, 290},
  {225, 305},
  {230, 320},
  {245, 325},
  {255, 325},
  {270, 320},
  {275, 305},
  {275, 290},
  {275, 255},
  {275, 240},
  {270, 225},
  {255, 220},
  {241, 225},
  {226, 220},
  {221, 205},
  {225, 190},
  {225, 155},
  {225, 140},
  {225, 105},
};

static GPoint level1_car_locations[] = {
  {227, 140},
  {207, 140},
  {207, 115},
  {227, 115},
};

static LevelSpriteDetails level1_details = {
  .level = LEVEL1_ID,
  .sprites = level1_sprites,
  .scenery = level1_scenery,
  .walls = level1_walls,
  .tracks = level1_tracks,
  .checkpoints = level1_checkpoints,
  .finish_group = &level1_finish_group,
  .track_points0 = level1_track_points0,
  .car_locations = level1_car_locations,
  .car_starting_angle = LEVEL1_CAR_STARTING_ANGLE,
};
