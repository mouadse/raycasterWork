#ifndef RAYCASTER_H
#define RAYCASTER_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "MLX42/include/MLX42/MLX42.h"

#define WIDTH  1280 // Example: Doubled width
#define HEIGHT 960  // Example: Doubled height

#define TEX_NORTH "assets/textures/north.png"
#define TEX_SOUTH "assets/textures/south.png"
#define TEX_WEST  "assets/textures/west.png"
#define TEX_EAST  "assets/textures/east.png"

typedef struct s_vector { float x; float y; } t_vector;
typedef struct s_coord  { int   x; int   y; } t_coord;
typedef struct s_dda    { float plane_multiplier; t_vector dir; t_vector camera_pixel; t_vector delta_dist; t_vector dist_side; t_coord map; t_coord step; int hit_side; float perp_dist; float wall_hit_x; } t_dda;
typedef struct s_keys   { bool w, s, a, d, left, right; } t_keys;
typedef struct s_wall   { int height; int start_y; int end_y; int tex_x; float tex_step; float tex_pos; } t_wall;
typedef struct s_game   { mlx_t *mlx; mlx_image_t *img; mlx_texture_t *tex[4]; t_vector pos; t_vector dir; t_vector plane; double frame_time; t_keys keys; } t_game;

void    fatal(const char *msg);
size_t  ft_strlen(const char *s);
char   *ft_strdup(const char *s);
char   *ft_strjoin(const char *a, const char *b);
char   *ft_itoa(int n);

static inline t_vector v_new(float x, float y);
static inline t_vector v_add(t_vector a, t_vector b);
static inline t_vector v_mul(t_vector v, float k);
static inline t_vector v_rot(t_vector v, float deg);

uint32_t get_tex_pixel(mlx_texture_t *tex, int y, int x);
uint32_t set_wall_colour(int side, int stepX, int stepY);
void     key_press(mlx_key_data_t k, void *p);
bool     map_is_wall(int x, int y);
void     handle_movement(t_game *g);
int      signf(float v);
void     draw_column(t_game *g, int x, t_dda *r);
void     render(t_game *g);
void     loop_hook(void *param);
void     load_textures(t_game *g);
void     find_player_start(t_game *g);

#endif // RAYCASTER_H
