// some common helper functions for the pretty poly examples
#pragma once

#include <stdint.h>
#include <sys/time.h>

typedef union {
  struct __attribute__((__packed__)) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
  } rgba;
  uint32_t c;
} colour;

__attribute__((always_inline)) uint32_t alpha(uint32_t sa, uint32_t da) {
  return ((sa + 1) * (da)) >> 8;
}

__attribute__((always_inline)) uint8_t blend_channel(uint8_t s, uint8_t d, uint8_t a) {
  return d + ((a * (s - d) + 127) >> 8);
}

colour blend(colour dest, colour src) {

  uint16_t a = alpha(src.rgba.a, dest.rgba.a);

  if(src.rgba.a == 0) return dest;
  if(src.rgba.a == 255) return src;

  colour result;
  result.rgba.r = blend_channel(src.rgba.r, dest.rgba.r, src.rgba.a);
  result.rgba.g = blend_channel(src.rgba.g, dest.rgba.g, src.rgba.a);
  result.rgba.b = blend_channel(src.rgba.b, dest.rgba.b, src.rgba.a);
  result.rgba.a = dest.rgba.a > src.rgba.a ? dest.rgba.a : src.rgba.a;

  return result;
}

colour create_colour(uint8_t r, uint8_t g, uint8_t b, uint8_t a) {
  return (colour){ .rgba.r = r, .rgba.g = g, .rgba.b = b, .rgba.a = a };
}

colour create_colour_hsv(float h, float s, float v, float a) {
    int i = (int)(h * 6.0f);
    float f = h * 6 - i;

    v = v * 255.0f;

    float sv = s * v;
    float fsv = f * sv;

    uint8_t p = (uint8_t)(-sv + v);
    uint8_t q = (uint8_t)(-fsv + v);
    uint8_t t = (uint8_t)(fsv - sv + v);

    uint8_t bv = (uint8_t)v;

    switch (i % 6) {
    default:
    case 0: return create_colour(bv, t, p, a * 255);
    case 1: return create_colour(q, bv, p, a * 255);
    case 2: return create_colour(p, bv, t, a * 255);
    case 3: return create_colour(p, q, bv, a * 255);
    case 4: return create_colour(t, p, bv, a * 255);
    case 5: return create_colour(bv, p, q, a * 255);
    }
}

uint64_t time_ms() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return (((uint64_t)tv.tv_sec) * 1000) + (tv.tv_usec / 1000);
}