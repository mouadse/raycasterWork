#include "cub3d.h"

int	game_loop(t_cub3d *game)
{
	double			current_time;
	static double	last_time;

	// Calculate frame time for smooth movement
	current_time = now_sec();
	game->frame_time = current_time - last_time;
	last_time = current_time;
	// Update player position based on key inputs
	update_player_movement(game);
	// Draw the background (floor and ceiling)
	draw_background2(game);
	// Cast rays and render walls
	draw_rays(game);
	// Put the rendered image to the window
	mlx_put_image_to_window(game->mlx, game->win, game->img.ptr, 0, 0);
	return (0);
}
