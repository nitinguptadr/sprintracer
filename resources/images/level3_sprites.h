#pragma once

#include "sprintracer_level.h"

#define LEVEL3_BOUNDS GRect(0, 0, SCREEN_RES_COLS*4, SCREEN_RES_ROWS*4)

#define LEVEL3_CAR_STARTING_ANGLE 0

// Used for sprite collision - drawn in order of index in array
static LevelSpriteLocation level3_sprites[] = {
  { RESOURCE_ID_VERT_SHRUB_96, {336, 4}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {248, 4}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {248, 100}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {296, 255}, NULL },
  { RESOURCE_ID_VERT_BUSH_4, {304, 250}, NULL },
  { RESOURCE_ID_VERT_BUSH_4, {0, 5}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {8, 195}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {15, 160}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {20, 125}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {96, 201}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {103, 196}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {248, 92}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {55, 146}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {100, 96}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {101, 296}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {189, 304}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {188, 342}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {336, 92}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {245, 248}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {196, 102}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {253, 240}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {254, 189}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {288, 239}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {288, 189}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {205, 296}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {304, 189}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {288, 197}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {200, 250}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {320, 150}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {300, 154}, NULL },
  { RESOURCE_ID_SHRUB_8, {146, 246}, NULL },
};

static LevelSpriteLocation level3_scenery[] = {
  { RESOURCE_ID_WATER_96_96, {335, 4}, NULL },
  { RESOURCE_ID_WATER_96_96, {250, 4}, NULL },
};

static LevelSpriteLocation level3_walls[] = {
  { RESOURCE_ID_HORIZ_WALL_144, {0, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {288, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {0, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {288, 352}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_24, {0, 336}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {424, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {424, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_24, {424, 336}, NULL },
};

// Track layout - drawn in order of index in array
static LevelSpriteLocation level3_tracks[] = {
  { RESOURCE_ID_CROSS_ROAD_BIG, {150, 150}, NULL },
  { RESOURCE_ID_CROSS_ROAD_BIG, {150, 200}, NULL },
  { RESOURCE_ID_CROSS_ROAD_SMALL, {250, 200}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {150, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 100}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 150}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 300}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {300, 100}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_W, {300, 200}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_E, {200, 200}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_W, {200, 300}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_E, {150, 300}, NULL },
  { RESOURCE_ID_VERT_ROAD_FUNNEL_S, {250, 150}, NULL },
  { RESOURCE_ID_VERT_ROAD_FUNNEL_N, {250, 250}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {350, 150}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {50, 250}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {50, 200}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {200, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {100, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {250, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {350, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {150, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {200, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {200, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {150, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {350, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {100, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {250, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 100}, NULL },
  { RESOURCE_ID_ARROW_E, {249, 221}, NULL },
  { RESOURCE_ID_ARROW_E, {149, 171}, NULL },
  { RESOURCE_ID_ARROW_S, {171, 199}, NULL },
};

static LevelSpriteLocation level3_checkpoints[] = {
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {200, 125}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {100, 100}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {145, 255}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {75, 235}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {350, 180}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {300, 80}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {230, 280}, NULL },
};

static LevelFinishLine level3_finish_group = {
  { RESOURCE_ID_FINISH_LINE_HORIZ, {54, 250}, NULL },
  { RESOURCE_ID_FINISH_BOX_1, {30, 250}, NULL },
  { RESOURCE_ID_FINISH_BOX_2, {30, 270}, NULL },
  { RESOURCE_ID_FINISH_BOX_3, {30, 290}, NULL },
  { RESOURCE_ID_FINISH_BOX_4, {30, 310}, NULL },
  { RESOURCE_ID_LIGHT_SIGNAL_ALL, {35, 230}, NULL },
};

static GPoint level3_track_points0[] = {
  {70, 240},
  {70, 205},
  {70, 190},
  {75, 175},
  {90, 170},
  {205, 175},
  {220, 170},
  {225, 155},
  {225, 90},
  {220, 75},
  {205, 70},
  {90, 70},
  {75, 75},
  {70, 90},
  {70, 105},
  {75, 120},
  {90, 125},
  {155, 120},
  {170, 125},
  {175, 140},
  {175, 255},
  {170, 270},
  {155, 275},
  {140, 275},
  {125, 270},
  {120, 255},
  {120, 240},
  {125, 225},
  {140, 220},
  {205, 220},
  {240, 220},
  {355, 225},
  {370, 220},
  {375, 205},
  {375, 140},
  {370, 125},
  {355, 120},
  {290, 120},
  {275, 125},
  {270, 140},
  {275, 305},
  {270, 320},
  {255, 325},
  {90, 325},
  {75, 320},
  {70, 305},
  {70, 290},
  {70, 255},
};

static GPoint level3_car_locations[] = {
  {77, 290},
  {57, 290},
  {57, 265},
  {77, 265},
};

static LevelSpriteDetails level3_details = {
  .level = LEVEL3_ID,
  .sprites = level3_sprites,
  .scenery = level3_scenery,
  .walls = level3_walls,
  .tracks = level3_tracks,
  .checkpoints = level3_checkpoints,
  .finish_group = &level3_finish_group,
  .track_points0 = level3_track_points0,
  .car_locations = level3_car_locations,
  .car_starting_angle = LEVEL3_CAR_STARTING_ANGLE,
};

