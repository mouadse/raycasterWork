/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   render_background.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 18:04:44 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 16:03:29 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	draw_background(t_cub3d *game)
{
	int	x;
	int	y;

	y = 0;
	while (y < game->img.h / 2)
	{
		x = 0;
		while (x < game->img.w)
		{
			mlx_pixel_put(game->mlx, game->win, x, y,
				game->config->ceiling_rgb);
			x++;
		}
		y++;
	}
	while (y < game->img.h)
	{
		x = 0;
		while (x < game->img.w)
		{
			mlx_pixel_put(game->mlx, game->win, x, y, game->config->floor_rgb);
			x++;
		}
		y++;
	}
}

void	draw_background2(t_cub3d *game)
{
	int x, y;

	y = 0;
	while (y < game->img.h)
	{
		x = 0;
		while (x < game->img.w)
		{
			if (y < game->img.h / 2)
				my_mlx_pixel_put(&game->img, x, y, game->config->ceiling_rgb);
			else
				my_mlx_pixel_put(&game->img, x, y, game->config->floor_rgb);
			x++;
		}
		y++;
	}
}
