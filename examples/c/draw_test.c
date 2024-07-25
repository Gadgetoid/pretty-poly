#include "float.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define PP_IMPLEMENTATION
#define PP_DEBUG
#include "pretty-poly.h"
#define PPP_IMPLEMENTATION
#include "pretty-poly-primitives.h"
#include "helpers.h"

const int WIDTH = 1024;
const int HEIGHT = 1024;
colour buffer[1024][1024];

colour pen;
void set_pen(colour c) {
  pen = c;
}

void blend_tile(const pp_tile_t *t) {  
  for(int y = t->y; y < t->y + t->h; y++) {
    for(int x = t->x; x < t->x + t->w; x++) {     
      colour alpha_pen = pen;
      alpha_pen.a = alpha(pen.a, pp_tile_get(t, x, y));
      buffer[y][x] = blend(buffer[y][x], alpha_pen);
    }
  }
}

void _pp_path_transform(pp_path_t *path, pp_mat3_t *transform) {
    for (int i = 0; i < path->count; i++) {
        path->points[i] = pp_point_transform(&path->points[i], transform);
    }
}

void _pp_poly_transform(pp_poly_t *poly, pp_mat3_t *transform) {
    pp_path_t *path = poly->paths;

    while(path) {
        _pp_path_transform(path, transform);
        path = path->next;
    }
}

int main() { 
  pp_tile_callback(blend_tile);
  pp_antialias(PP_AA_X16);
  //pp_clip(50, 50, WIDTH - 400, HEIGHT - 400);
  pp_clip(0, 0, WIDTH, HEIGHT);

  for(int y = 0; y < HEIGHT; y++) {
    for(int x = 0; x < WIDTH; x++) {
      buffer[y][x] = ((x / 8) + (y / 8)) % 2 == 0 ? create_colour(16, 32, 48, 255) : create_colour(0, 0, 0, 255);
    }
  }

  pp_mat3_t t = pp_mat3_identity();


  // A single merged poly

  pp_poly_t *combined = pp_poly_new();

  for(int x = 0; x < 60; x++) {
    if(x % 5 == 0) {
        // hour mark
        pp_poly_merge(combined, ppp_rect((ppp_rect_def){.x=WIDTH / 2 - 5, .y=10, .w=10, .h=HEIGHT / 10}));
    } else {
        // minute mark
        pp_poly_merge(combined, ppp_rect((ppp_rect_def){.x=WIDTH / 2 - 3, .y=10, .w=6, .h=HEIGHT / 48}));
    }
    _pp_poly_transform(combined, &t);
    pp_mat3_translate(&t, WIDTH / 2, HEIGHT / 2);
    pp_mat3_rotate(&t, 360.0f / 60.0f);
    pp_mat3_translate(&t, -WIDTH / 2, -HEIGHT / 2);
  }

  set_pen(create_colour(255, 0, 0, 255));

  t = pp_mat3_identity();
  pp_mat3_translate(&t, 0, 10);
  pp_transform(&t);

  pp_render(combined);
  
  set_pen(create_colour(255, 255, 255, 255));

  t = pp_mat3_identity();

  // Individual elements and draw calls

  pp_poly_t *min_mark = ppp_rect((ppp_rect_def){.x=WIDTH / 2 - 3, .y=10, .w=6, .h=HEIGHT / 48});
  pp_poly_t *hour_mark = ppp_rect((ppp_rect_def){.x=WIDTH / 2 - 5, .y=10, .w=10, .h=HEIGHT / 10});


  for(int x = 0; x < 60; x++) {
    if(x % 5 == 0) {
        pp_render(hour_mark);
    } else {
        pp_render(min_mark);
    }
    pp_mat3_translate(&t, WIDTH / 2, HEIGHT / 2);
    pp_mat3_rotate(&t, 360.0f / 60.0f);
    pp_mat3_translate(&t, -WIDTH / 2, -HEIGHT / 2);
  }

  stbi_write_png("/tmp/out.png", WIDTH, HEIGHT, 4, (void *)buffer, WIDTH * sizeof(uint32_t));
  
  return 0;
}