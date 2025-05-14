/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   wall_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 15:06:49 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 16:48:52 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	*get_wall_texture(t_cub3d *game, t_dda *ray)
{
	if (ray->hit_side == 1)
	{
		if (ray->step.y < 0)
			return (game->north_texture);
		else
			return (game->south_texture);
	}
	else
	{
		if (ray->step.x < 0)
			return (game->west_texture);
		else
			return (game->east_texture);
	}
}

void	intersection_point(t_dda *ray, t_cub3d *game, t_wall_slice *wall)
{
	if (ray->hit_side == 0)
		wall->hit_pos_x = game->player_pos.y + ray->perp_dist * ray->dir.y;
	else
		wall->hit_pos_x = game->player_pos.x + ray->perp_dist * ray->dir.x;
	wall->hit_pos_x -= floor(wall->hit_pos_x);
	wall->tex_step = 1.0 * TEX_HEIGHT / wall->slice_height;
}

void	find_texture_position_x(t_dda *ray, t_cub3d *game, t_wall_slice *wall)
{
	wall->tex_x = (int)(wall->hit_pos_x * game->texture->w);
	if ((ray->hit_side == 0 && ray->dir.x < 0) || (ray->hit_side == 1
			&& ray->dir.y > 0))
		wall->tex_x = game->texture->w - wall->tex_x - 1;
	wall->tex_step = 1.0 * game->texture->h / wall->slice_height;
}

static void	render_wall(t_cub3d *game, int pixel, t_wall_slice *wall)
{
	int			tex_y;
	uint32_t	col;
	int			y;

	y = wall->draw_start_y;
	while (y < wall->draw_end_y)
	{
		tex_y = (int)wall->tex_pos;
		if (tex_y < 0)
			tex_y = 0;
		if (tex_y >= game->texture->h)
			tex_y = game->texture->h - 1;
		wall->tex_pos += wall->tex_step;
		col = tex_pixel(game->texture, wall->tex_x, tex_y);
		my_mlx_pixel_put(&game->img, pixel, y, col);
		y++;
	}
}

void	draw_wall(t_dda *ray, t_cub3d *game, int pixel)
{
	t_wall_slice	wall;
	float			safe_dist;

	safe_dist = ray->perp_dist;
	if (safe_dist <= 0.0001f) // never let it be zero/negative
		safe_dist = 0.0001f;
	game->texture = get_wall_texture(game, ray);
		// Get the correct texture
	wall.slice_height = (int)(SCREEN_HEIGHT / safe_dist);
		// Use safe_dist instead of ray->perp_dist
	wall.draw_start_y = (SCREEN_HEIGHT / 2 - wall.slice_height / 2);
	wall.draw_end_y = (SCREEN_HEIGHT / 2 + wall.slice_height / 2);
	if (wall.draw_start_y < 0)
		wall.draw_start_y = 0;
	if (wall.draw_end_y >= SCREEN_HEIGHT)
		wall.draw_end_y = SCREEN_HEIGHT - 1;
	intersection_point(ray, game, &wall);
	find_texture_position_x(ray, game, &wall);
	wall.tex_pos = (wall.draw_start_y - (int)((float)SCREEN_HEIGHT / 2)
			+ (int)((float)wall.slice_height / 2)) * wall.tex_step;
	render_wall(game, pixel, &wall);
}
