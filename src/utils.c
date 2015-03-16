#if 0

void graphics_draw_rotated_bitmap(GContext* ctx, GBitmap *src, GPoint src_ic, int rotation, GPoint dest_ic) {
  if (rotation == 0) {
    graphics_draw_bitmap_in_rect(ctx, src,
        (GRect){ .origin = { dest_ic.x - src_ic.x, dest_ic.y - src_ic.y }, .size = src->bounds.size });
    return;
  }

  const GRect dest_clip = GRect(0,0,144,168);
  dest_ic.x += ctx->draw_state.drawing_box.origin.x;
  dest_ic.y += ctx->draw_state.drawing_box.origin.y;

  GCompOp compositing_mode = ctx->draw_state.compositing_mode;
#if SCREEN_COLOR_DEPTH_BITS == 1
  GColor foreground, background;
  switch (compositing_mode) {
    case GCompOpAssign:
      foreground = GColorWhite;
      background = GColorBlack;
      break;
    case GCompOpAssignInverted:
      foreground = GColorBlack;
      background = GColorWhite;
      break;
    case GCompOpOr:
      foreground = GColorWhite;
      background = GColorClear;
      break;
    case GCompOpAnd:
      foreground = GColorClear;
      background = GColorBlack;
      break;
    case GCompOpClear:
      foreground = GColorBlack;
      background = GColorClear;
      break;
    case GCompOpSet:
      foreground = GColorClear;
      background = GColorWhite;
      break;
    default:
      PBL_ASSERT(0, "unknown coposting mode %d", compositing_mode);
      return;
  }
#endif

  const GColor ctx_color = ctx->draw_state.stroke_color;

  for (int y = dest_clip.origin.y; y < dest_clip.origin.y + dest_clip.size.h; ++y) {
    for (int x = dest_clip.origin.x; x < dest_clip.origin.x + dest_clip.size.w; ++x) {
      const int32_t src_numerator_x = cos_lookup(-rotation) * (x - dest_ic.x) - sin_lookup(-rotation) * (y - dest_ic.y);
      const int32_t src_numerator_y = cos_lookup(-rotation) * (y - dest_ic.y) + sin_lookup(-rotation) * (x - dest_ic.x);

      const DivResult src_vector_x = polar_div(src_numerator_x, TRIG_MAX_RATIO);
      const DivResult src_vector_y = polar_div(src_numerator_y, TRIG_MAX_RATIO);

      const int32_t src_x = src_ic.x + src_vector_x.quot;
      const int32_t src_y = src_ic.y + src_vector_y.quot;

      // only draw if I'm in the src rect
      if (!(src_x >= 0 && src_x < src->bounds.size.w && src_y >= 0 && src_y < src->bounds.size.h)) {
        continue;
      }

#if SCREEN_COLOR_DEPTH_BITS == 1
      // dividing by 8 to avoid overflows of <thresh> in the next loop
      const int32_t horiz_contrib[3] = {
        src_vector_x.rem < 0 ? (-src_vector_x.rem) >> 3 : 0,
        src_vector_x.rem < 0 ? (TRIG_MAX_RATIO + src_vector_x.rem) >> 3 : (TRIG_MAX_RATIO - src_vector_x.rem) >> 3,
        src_vector_x.rem < 0 ? 0 : (src_vector_x.rem) >> 3
      };

      const int32_t vert_contrib[3] = {
        src_vector_y.rem < 0 ? (-src_vector_y.rem) >> 3 : 0,
        src_vector_y.rem < 0 ? (TRIG_MAX_RATIO + src_vector_y.rem) >> 3 : (TRIG_MAX_RATIO - src_vector_y.rem) >> 3,
        src_vector_y.rem < 0 ? 0 : (src_vector_y.rem) >> 3
      };

      int32_t thresh = 0;

      for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
          if (src_x + i >= 0 && src_x + i < src->bounds.size.w
              && src_y + j >= 0 && src_y + j < src->bounds.size.h) {
            // I'm within bounds
            if (get_bitmap_bit(src, src_x + i , src_y + j)) {
              // more color
              thresh += (horiz_contrib[i+1] * vert_contrib[j+1]);
            } else {
              // less color
              thresh -= (horiz_contrib[i+1] * vert_contrib[j+1]);
            }
          }
        }
      }

      if (thresh > 0) {
        ctx->draw_state.stroke_color = foreground;
      } else {
        ctx->draw_state.stroke_color = background;
      }
#elif SCREEN_COLOR_DEPTH_BITS == 8
      GColor src_color = get_bitmap_color(src, src_x, src_y);
      GColor dst_color = get_bitmap_color(&ctx->dest_bitmap, x, y);
      switch (compositing_mode) {
      case GCompOpSet:
          ctx->draw_state.stroke_color = graphics_do_alpha_blend(src_color, dst_color);
          break;
        case GCompOpAssign:
        default:
          // Do assign by default
          ctx->draw_state.stroke_color = src_color;
          break;
      }

#endif
      if (!gcolor_is_transparent(ctx->draw_state.stroke_color)) {
        set_pixel(ctx, GPoint(x, y));
      }

    }
  }
  ctx->draw_state.stroke_color = ctx_color;
}
#endif
