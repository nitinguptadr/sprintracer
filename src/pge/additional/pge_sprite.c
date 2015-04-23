#include "pge_sprite.h"
#include "pge_collision.h"

GBitmap *s_bitmaps[60] = {0};

PGESprite* pge_sprite_create(GPoint position, int initial_resource_id) {
  PGESprite *this = malloc(sizeof(PGESprite));

  if (s_bitmaps[initial_resource_id] == NULL) {
    s_bitmaps[initial_resource_id] = gbitmap_create_with_resource(initial_resource_id);
  }

  this->bitmap = s_bitmaps[initial_resource_id];
  this->resource_id = initial_resource_id;
  // Allocate
  this->position = position;
  this->angle = 0;

  // Finally
  return this;
}

void pge_sprite_destroy(PGESprite *this) {
  if (this) {
    if (s_bitmaps[this->resource_id]) {
      gbitmap_destroy(s_bitmaps[this->resource_id]);
    }
    s_bitmaps[this->resource_id] = NULL;
    this->bitmap = NULL;
    free(this);
  }
}

void pge_sprite_set_anim_frame(PGESprite *this, int resource_id) {
  if (this) {
    //gbitmap_destroy(this->bitmap);
    if (s_bitmaps[resource_id] == NULL) {
      s_bitmaps[resource_id] = gbitmap_create_with_resource(resource_id);
    }

    this->bitmap = s_bitmaps[resource_id];
    this->resource_id = resource_id;
  }
}

void pge_sprite_draw(PGESprite *this, GContext *ctx) {
#ifdef PBL_PLATFORM_APLITE
  GRect bounds = this->bitmap->bounds;
#elif PBL_PLATFORM_BASALT
  GRect bounds = gbitmap_get_bounds(this->bitmap);
#endif

  if (this->angle == 0) {
    graphics_draw_bitmap_in_rect(ctx, this->bitmap, GRect(this->position.x, this->position.y, bounds.size.w, bounds.size.h));
  } else {
    GPoint rotation_center = GPoint(bounds.origin.x + (bounds.size.w / 2), bounds.origin.y + (bounds.size.h / 2));
    //void graphics_draw_rotated_bitmap(GContext *ctx, GBitmap *src, GPoint src_ic, int rotation, GPoint dest_ic) {
    //graphics_draw_rotated_bitmap(ctx, this->bitmap, GPoint(this->position.x + rotation_center.x, this->position.y + rotation_center.y), this->angle, GPointZero);
    graphics_draw_rotated_bitmap(ctx, this->bitmap, rotation_center, this->angle, GPoint(this->position.x + rotation_center.x, this->position.y + rotation_center.y));
  }
}

void pge_sprite_set_position(PGESprite *this, GPoint new_position) {
  if (this) {
    this->position = new_position;
  }
}

GPoint pge_sprite_get_position(PGESprite *this) {
  if (!this) {
    return GPointZero;
  }
  return this->position;
}

void pge_sprite_move(PGESprite *this, int dx, int dy) {
  if (this) {
    GPoint pos = pge_sprite_get_position(this);
    pos.x += dx;
    pos.y += dy;
    pge_sprite_set_position(this, pos);
  }
}

bool pge_check_collision(PGESprite* sprite1, PGESprite *sprite2) {
  if ((!sprite1) || (!sprite2)) {
    return false;
  }
#ifdef PBL_PLATFORM_APLITE
  GRect bounds1 = sprite1->bitmap->bounds;
  GRect bounds2 = sprite2->bitmap->bounds;
#elif PBL_PLATFORM_BASALT
  GRect bounds1 = gbitmap_get_bounds(sprite1->bitmap);
  GRect bounds2 = gbitmap_get_bounds(sprite2->bitmap);
#endif

  GRect rect_a = (GRect){
    {sprite1->position.x, sprite1->position.y}, 
    {bounds1.size.w, bounds1.size.h}};

  GRect rect_b = (GRect){
    {sprite2->position.x, sprite2->position.y}, 
    {bounds2.size.w, bounds2.size.h}};

  // Test each corner of the other rect
  return pge_collision_rectangle_rectangle(&rect_a, &rect_b);
}

GRect pge_sprite_get_bounds(PGESprite *this) {
  if (!this) {
    return GRectZero;
  }
#ifdef PBL_PLATFORM_APLITE
  GRect bounds = this->bitmap->bounds;
#elif PBL_PLATFORM_BASALT
  GRect bounds = gbitmap_get_bounds(this->bitmap);
#endif
  return GRect(this->position.x, this->position.y, bounds.size.w, bounds.size.h);
}

void pge_sprite_set_rotation(PGESprite *this, int32_t angle) {
  this->angle = angle;
}
