/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   textures.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:17:23 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 12:44:48 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_tex	*set_wall(t_cub3d *game, t_dda *ray)
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
	return (NULL);
}

t_tex	*load_xpm(void *mlx, char *file)
{
	t_tex	*t;

	t = malloc(sizeof(t_tex));
	if (!t)
	{
		fprintf(stderr, "Error: cannot allocate memory for texture\n");
		exit(EXIT_FAILURE);
	}
	t->ptr = NULL;
	t->addr = NULL;
	t->ptr = mlx_xpm_file_to_image(mlx, file, &t->w, &t->h);
	if (!t->ptr)
	{
		fprintf(stderr, "Error: cannot load texture %s\n", file);
		free(t);
		exit(EXIT_FAILURE);
	}
	t->addr = mlx_get_data_addr(t->ptr, &t->bpp, &t->line_len, &t->endian);
	return (t);
}

void	load_textures(t_cub3d *game)
{
	game->north_texture = load_xpm(game->mlx, game->config->no_texture_path);
	game->south_texture = load_xpm(game->mlx, game->config->so_texture_path);
	game->west_texture = load_xpm(game->mlx, game->config->we_texture_path);
	game->east_texture = load_xpm(game->mlx, game->config->ea_texture_path);
}
