/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   camera.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 17:53:28 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 12:09:03 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	init_player_camera(t_cub3d *game)
{
	if (game->config->character_orientation == 'N')
	{
		game->player_dir = create_vector(0, -1);
		game->camera_plane = create_vector(0.66, 0);
	}
	else if (game->config->character_orientation == 'S')
	{
		game->player_dir = create_vector(0, 1);
		game->camera_plane = create_vector(-0.66, 0);
	}
	else if (game->config->character_orientation == 'W')
	{
		game->player_dir = create_vector(-1, 0);
		game->camera_plane = create_vector(0, -0.66);
	}
	else if (game->config->character_orientation == 'E')
	{
		game->player_dir = create_vector(1, 0);
		game->camera_plane = create_vector(0, 0.66);
	}
}

void	set_player_position_and_plane(t_cub3d *game)
{
	
	game->player_pos = create_vector(game->config->character_pos_x + 0.5,
			game->config->character_pos_y + 0.5);
	init_player_camera(game);
}
