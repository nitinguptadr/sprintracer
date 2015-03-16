

#define SCREEN_RES_ROWS 168
#define SCREEN_RES_COLS 144

// Mask to determine which direction the car can travel
#define DIRECTION_NONE  0
#define DIRECTION_UP    1
#define DIRECTION_LEFT  2
#define DIRECTION_RIGHT 4
#define DIRECTION_DOWN  8
#define DIRECTION_ALL   (DIRECTION_UP | DIRECTION_LEFT | DIRECTION_RIGHT | DIRECTION_DOWN)

typedef enum {
  LEVEL_ID0,
} LevelNumId;
