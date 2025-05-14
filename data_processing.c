/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   data_processing.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:31:51 by msennane          #+#    #+#             */
/*   Updated: 2025/04/10 12:31:52 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	data_processing(char *map_file, t_config *config)
{
	char	*temp;
	int		fd;

	temp = NULL;
	fd = open_file(map_file);
	read_textures_path(config, temp, fd);
	count_map_size(config, temp, fd);
	fd = open_file(map_file);
	read_and_copy_map_content(temp, fd);
	if (config->grid == NULL)
		handle_error("Error: invalid map.\n");
	get_max_columns(config);
	get_max_lines(config);
}

void	read_textures_path(t_config *config, char *temp, int fd)
{
	char	*line;

	temp = get_next_line(fd);
	while (temp)
	{
		line = temp;
		while (ft_isspace(*temp))
			temp++;
		read_textures_path_aux(config, temp, line);
		free(line);
		if (config->no_texture_path && config->so_texture_path
			&& config->we_texture_path && config->ea_texture_path
			&& config->textures_ready)
			break ;
		temp = get_next_line(fd);
	}
	if (!temp)
		handle_error("Error: invalid file.\n");
}
