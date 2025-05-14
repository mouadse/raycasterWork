/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:43:24 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 16:03:31 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H

# include "./libft/libft.h"
# include <fcntl.h>
# include <math.h>
# include <mlx.h>
# include <stdbool.h>
# include <stdint.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <unistd.h>

# define ERR_MISSING_MAP "Error: no map file.\n"
# define ERR_TOO_MANY_ARGS "Error: too many args.\n"
# define ERR_INVALID_EXTENSION "Error: invalid .cub file.\n"
# define ERR_INVALID_CHARACTER "Error: bad character in map.\n"
# define ERR_PLAYER_COUNT "Error: one player required.\n"

# define PI 3.141592653589793
# define TEX_WIDTH 64
# define TEX_HEIGHT 64

# define SCREEN_WIDTH 1280
# define SCREEN_HEIGHT 720

# define KEY_ESC 65307
# define KEY_W 119
# define KEY_S 115
# define KEY_A 97
# define KEY_D 100
# define KEY_LEFTAR 65361
# define KEY_RIGHTAR 65363

typedef struct s_keys
{
	bool		w;
	bool		s;
	bool		a;
	bool		d;
	bool		left;
	bool		right;
}				t_keys;

typedef struct s_vec2
{
	float		x;
	float		y;
}				t_vec2;

typedef struct s_point
{
	int			x;
	int			y;
}				t_point;

typedef struct s_validator
{
	int			invalid_char_count;
	int			player_count;
}				t_validator;

typedef struct s_config
{
	char		**grid;
	char		*no_texture_path;
	char		*so_texture_path;
	char		*we_texture_path;
	char		*ea_texture_path;
	uint32_t	floor_rgb;
	uint32_t	ceiling_rgb;
	bool		textures_ready;
	int			loaded_textures;
	int			grid_rows;
	int			grid_cols;
	char		character_orientation;
	int			character_pos_x;
	int			character_pos_y;
}				t_config;

typedef struct s_wall_slice
{
	int			slice_height;
	int			draw_start_y;
	int			draw_end_y;
	int			tex_x;
	int			tex_y;
	float		hit_pos_x;
	float		tex_step;
	float		tex_pos;
}				t_wall_slice;

typedef struct s_dda
{
	float		plane_multiplier;
	t_vec2		dir;
	t_vec2		camera_pixel;
	t_vec2		delta_dist;
	t_vec2		dist_side;
	t_point		map;
	t_point		step;
	int			hit_side;
	float		perp_dist;
	float		wall_hit_x;
}				t_dda;

typedef struct s_img
{
	void		*ptr;
	char		*addr;
	int			bpp;
	int			line_len;
	int			endian;
	int			w;
	int			h;
}				t_img;

typedef struct s_tex
{
	void		*ptr;
	char		*addr;
	int			bpp;
	int			line_len;
	int			endian;
	int			w;
	int			h;
}				t_tex;

typedef struct s_cub3d
{
	void		*mlx;
	void		*win;
	t_tex		*north_texture;
	t_tex		*south_texture;
	t_tex		*west_texture;
	t_tex		*east_texture;
	t_tex		*texture;
	int			*texture_pixels[4];
	t_vec2		player_pos;
	t_vec2		player_dir;
	t_vec2		camera_plane;
	int			last_hit_side;
	float		frame_time;
	t_keys		keys;
	t_img		img;
	t_config	*config;
}				t_cub3d;

/* File handling and validation */
void			validate_map(int argc, char **argv, t_cub3d *game);
void			check_arguments(int argc);
void			check_extension(char *map_file);
int				check_path(char *path);
int				open_file(char *map_file);

/* Data processing */
void			data_processing(char *map_file, t_config *config);
void			read_textures_path(t_config *config, char *temp, int fd);
void			read_textures_path_aux(t_config *config, char *temp,
					char *line);

/* Texture path processing */
void			copy_texture_path(char **texture, char *path, char *mode,
					char *line);
void			trim_newline(char *str, char *line);
void			check_spaces(char *temp, char *mode, char *line);

/* RGB color processing */
void			check_rgb(uint32_t *color, char *rgb_color, char *original,
					char c);
void			split_rgb(char ***rgb, char *rgb_color, char *original, char c);
uint32_t		convert_rgb(int r, int g, int b);

/* Wall validation */
bool			check_diagonals(t_config *config, int line, int col);
bool			check_sides(t_config *config, int line, int col);
void			surrounded_by_walls(t_config *config);

/* Map processing */
void			count_map_size(t_config *config, char *temp, int fd);
void			read_and_copy_map_content(char *temp, int fd);
void			analyze_map_content(t_config *config, t_validator *validator);

/* Map utility functions */
void			get_max_lines(t_config *config);
void			get_max_columns(t_config *config);
void			check_map_content(t_validator *validator);
int				check_invalid_char(char c);

/* Tab handling */
int				count_tabs(char *line);
void			found_tabs(t_config *config);
char			*replace_tabs(char *line, int tabs);

/* Memory and error handling */
void			free_memory(t_cub3d *game);
t_cub3d			*get_game(t_cub3d *game);
void			handle_error(char *message);
void			init_variables_valid(t_validator *validator);

/* File reading utilities */
char			*get_next_line(int fd);
void			ft_free_matrix(char **matrix);

/* String utilities */

int				ft_isspace(int c);

/* Matrix and Vector Operations */

t_vec2			create_vector(float x, float y);
t_vec2			vector_add(t_vec2 a, t_vec2 b);
t_vec2			vector_subtract(t_vec2 a, t_vec2 b);
t_vec2			vector_multiply(t_vec2 a, float scalar);
float			vector_magnitude(t_vec2 a);
t_vec2			rotate_vector(t_vec2 vector, float angle);

/* Raycasting math utilities */
int				sign_of(float v);
t_vec2			calc_delta_dist(t_vec2 dir);
void			init_side_dist(t_dda *ray, t_vec2 pos);

/* Raycasting core functions */
void			perform_dda(t_dda *ray, char **map, int *hit_side);
void			draw_rays(t_cub3d *game);
void			draw_wall(t_dda *ray, t_cub3d *game, int pixel);

/* Movement and player control */
void			update_player_movement(t_cub3d *game);

/* Rendering functions */
void			draw_background(t_cub3d *game);
void			draw_background2(t_cub3d *game);
/* My MLX functions */
double	now_sec(void); // To be removed later
uint32_t		tex_pixel(t_tex *t, int x, int y);
void			init_image(t_cub3d *g, int w, int h);
void			my_mlx_pixel_put(t_img *img, int x, int y, uint32_t col);
void			load_textures(t_cub3d *game);

// Add hooks for close, loop, and input events
int				close_window(t_cub3d *game);
int				game_loop(t_cub3d *game);
int				key_press(int keycode, t_cub3d *game);
int				key_release(int keycode, t_cub3d *game);

#endif
