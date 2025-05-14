/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   event_hooks.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 16:10:19 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 20:04:44 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <stdio.h>

int	key_press(int keycode, t_cub3d *game)
{
	printf("Key pressed: %d\n", keycode);
	if (keycode == KEY_ESC)
		close_window(game); // To handle leaks later
	else if (keycode == KEY_W)
		game->keys.w = true;
	else if (keycode == KEY_S)
		game->keys.s = true;
	else if (keycode == KEY_A)
		game->keys.a = true;
	else if (keycode == KEY_D)
		game->keys.d = true;
	else if (keycode == KEY_LEFTAR)
		game->keys.left = true;
	else if (keycode == KEY_RIGHTAR)
		game->keys.right = true;
	return (0);
}

int	key_release(int keycode, t_cub3d *game)
{
	if (keycode == KEY_W)
		game->keys.w = false;
	else if (keycode == KEY_S)
		game->keys.s = false;
	else if (keycode == KEY_A)
		game->keys.a = false;
	else if (keycode == KEY_D)
		game->keys.d = false;
	else if (keycode == KEY_LEFTAR)
		game->keys.left = false;
	else if (keycode == KEY_RIGHTAR)
		game->keys.right = false;
	return (0);
}

// Add window close function
int	close_window(t_cub3d *game)
{
	(void)game;
	exit(0);
	return (0);
}

// Main game loop - updates and renders each frame
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
