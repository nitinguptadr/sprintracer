#pragma once

#include "sprintracer_level.h"

#define LEVEL2_BOUNDS GRect(0, 0, SCREEN_RES_COLS*4, SCREEN_RES_ROWS*4)

#define LEVEL2_CAR_STARTING_ANGLE -90

// Used for sprite collision - drawn in order of index in array
static LevelSpriteLocation level2_sprites[] = {
  { RESOURCE_ID_VERT_SHRUB_96, {334, 248}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {505, 201}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {246, 248}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {246, 248}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {246, 336}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {496, 105}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {408, 105}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {408, 105}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {408, 193}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {165, 100}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {25, 315}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {10, 206}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {205, 100}, NULL },
  { RESOURCE_ID_VERT_BUSH_35, {235, 105}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {196, 106}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {40, 204}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {92, 85}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {288, 87}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_24, {288, 55}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {350, 240}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {392, 240}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {350, 186}, NULL },
  { RESOURCE_ID_VERT_SHRUB_24, {392, 186}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {200, 250}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {150, 310}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {245, 10}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {410, 70}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {506, 165}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {536, 165}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {70, 50}, NULL },
  { RESOURCE_ID_VERT_TREE_35, {95, 10}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {440, 252}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {400, 252}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {342, 248}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {246, 198}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {342, 154}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {342, 106}, NULL },
  { RESOURCE_ID_VERT_SHRUB_48, {400, 153}, NULL },
  { RESOURCE_ID_VERT_SHRUB_96, {400, 57}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {101, 246}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {400, 244}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {348, 47}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_48, {400, 292}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {55, 196}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {4, 173}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {4, 85}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {60, 300}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {252, 95}, NULL },
  { RESOURCE_ID_HORIZ_SHRUB_96, {252, 47}, NULL },
};

static LevelSpriteLocation level2_scenery[] = {
  { RESOURCE_ID_VERT_BUSH_35, {410, 255}, NULL },
  { RESOURCE_ID_WATER_96_96, {4, 85}, NULL },
  { RESOURCE_ID_WATER_96_96, {246, 248}, NULL },
  { RESOURCE_ID_WATER_96_96, {408, 105}, NULL },
};

static LevelSpriteLocation level2_walls[] = {
  { RESOURCE_ID_HORIZ_WALL_144, {0, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {288, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {432, 0}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {0, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {144, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {288, 352}, NULL },
  { RESOURCE_ID_HORIZ_WALL_144, {432, 352}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {0, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_24, {0, 336}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {568, 0}, NULL },
  { RESOURCE_ID_VERT_WALL_168, {568, 168}, NULL },
  { RESOURCE_ID_VERT_WALL_24, {568, 336}, NULL },
};

// Track layout - drawn in order of index in array
static LevelSpriteLocation level2_tracks[] = {
  { RESOURCE_ID_CURVE_ROAD_NW, {100, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {50, 200}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_E, {250, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {150, 150}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {450, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {450, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {250, 150}, NULL },
  { RESOURCE_ID_CROSS_ROAD_BIG, {150, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NE, {350, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {100, 150}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_W, {300, 50}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {250, 200}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {150, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {350, 300}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SW, {50, 250}, NULL },
  { RESOURCE_ID_CURVE_ROAD_NW, {200, 150}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {100, 100}, NULL },
  { RESOURCE_ID_CURVE_ROAD_SE, {200, 200}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_E, {300, 200}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {350, 100}, NULL },
  { RESOURCE_ID_VERT_ROAD_FUNNEL_S, {350, 150}, NULL },
  { RESOURCE_ID_CROSS_ROAD_SMALL, {350, 200}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_FUNNEL_W, {400, 200}, NULL },
  { RESOURCE_ID_VERT_ROAD_FUNNEL_N, {350, 250}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {100, 250}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {150, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {200, 50}, NULL },
  { RESOURCE_ID_HORIZ_ROAD_50, {400, 300}, NULL },
  { RESOURCE_ID_VERT_ROAD_50, {450, 250}, NULL },
  { RESOURCE_ID_ARROW_E, {350, 222}, NULL },
  { RESOURCE_ID_ARROW_S, {172, 201}, NULL },
};

static LevelSpriteLocation level2_checkpoints[] = {
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {100, 130}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {135, 255}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {25, 235}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {240, 125}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {450, 250}, NULL },
  { RESOURCE_ID_CHECKPOINT_VERT_70, {400, 280}, NULL },
  { RESOURCE_ID_CHECKPOINT_HORIZ_70, {350, 129}, NULL },
};

static LevelFinishLine level2_finish_group = {
  { RESOURCE_ID_FINISH_LINE_VERT, {150, 55}, NULL },
  { RESOURCE_ID_FINISH_BOX_1, {165, 30}, NULL },
  { RESOURCE_ID_FINISH_BOX_2, {185, 30}, NULL },
  { RESOURCE_ID_FINISH_BOX_3, {205, 30}, NULL },
  { RESOURCE_ID_FINISH_BOX_4, {225, 30}, NULL },
  { RESOURCE_ID_LIGHT_SIGNAL_ALL, {155, 30}, NULL },
};

static GPoint level2_track_points0[] = {
  {140, 70},
  {125, 75},
  {120, 90},
  {120, 155},
  {125, 170},
  {140, 175},
  {155, 170},
  {170, 175},
  {175, 190},
  {175, 255},
  {170, 270},
  {155, 275},
  {90, 275},
  {75, 270},
  {70, 255},
  {70, 240},
  {75, 225},
  {90, 220},
  {205, 225},
  {220, 220},
  {225, 205},
  {220, 190},
  {225, 175},
  {240, 170},
  {255, 170},
  {270, 175},
  {275, 190},
  {270, 205},
  {275, 220},
  {290, 225},
  {305, 220},
  {340, 220},
  {405, 220},
  {440, 220},
  {455, 220},
  {470, 225},
  {475, 240},
  {475, 305},
  {470, 320},
  {455, 325},
  {390, 325},
  {375, 320},
  {370, 305},
  {370, 290},
  {370, 255},
  {375, 190},
  {375, 155},
  {375, 90},
  {370, 75},
  {355, 70},
  {340, 70},
  {305, 70},
  {290, 70},
  {255, 70},
  {240, 70},
  {205, 70},
  {190, 70},
  {155, 70},
};

static GPoint level2_car_locations[] = {
  {195, 75},
  {170, 75},
  {170, 55},
  {195, 55},
};

static LevelSpriteDetails level2_details = {
  .level = LEVEL2_ID,
  .sprites = level2_sprites,
  .scenery = level2_scenery,
  .walls = level2_walls,
  .tracks = level2_tracks,
  .checkpoints = level2_checkpoints,
  .finish_group = &level2_finish_group,
  .track_points0 = level2_track_points0,
  .car_locations = level2_car_locations,
  .car_starting_angle = LEVEL2_CAR_STARTING_ANGLE,
};
