/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_path.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:26:42 by msennane          #+#    #+#             */
/*   Updated: 2025/04/10 12:26:48 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	check_colors(t_config *config, char *line, char *temp);

void	read_textures_path_aux(t_config *config, char *temp, char *line)
{
	if (ft_strncmp("NO", temp, 2) == 0)
		copy_texture_path(&(config->no_texture_path), temp, "NO", line);
	else if (ft_strncmp("SO", temp, 2) == 0)
		copy_texture_path(&(config->so_texture_path), temp, "SO", line);
	else if (ft_strncmp("WE", temp, 2) == 0)
		copy_texture_path(&(config->we_texture_path), temp, "WE", line);
	else if (ft_strncmp("EA", temp, 2) == 0)
		copy_texture_path(&(config->ea_texture_path), temp, "EA", line);
	else if (ft_strncmp("F", temp, 1) == 0)
		check_rgb(&config->floor_rgb, temp, line, 'F');
	else if (ft_strncmp("C", temp, 1) == 0)
		check_rgb(&config->ceiling_rgb, temp, line, 'C');
	else
	{
		if (temp[0] != '\n' && temp[0] != '\0')
		{
			free(line);
			handle_error("Error: invalid file.\n");
		}
	}
	check_colors(config, line, temp);
	config->loaded_textures++;
}

static void	check_colors(t_config *config, char *line, char *temp)
{
	static int	ceiling;
	static int	floor;

	if (ft_strncmp("F", temp, 1) == 0)
		floor++;
	if (ft_strncmp("C", temp, 1) == 0)
		ceiling++;
	if (ceiling == 1 && floor == 1)
		config->textures_ready = true;
	if (ceiling > 1 || floor > 1)
	{
		free(line);
		handle_error("Error: duplicated color.\n");
	}
}

void	copy_texture_path(char **texture, char *path, char *mode, char *line)
{
	int	size_key;

	size_key = 0;
	if (*texture != NULL)
	{
		free(line);
		handle_error("Error: duplicated texture path.\n");
	}
	check_spaces(path, mode, line);
	while (ft_isspace(*path) || ft_strncmp(mode, path, 2) == 0)
	{
		if (ft_strncmp(mode, path, 2) == 0)
		{
			size_key++;
			path++;
		}
		path++;
	}
	trim_newline(path, line);
	if (!check_path(path) || size_key != 1)
	{
		free(line);
		handle_error("Error: invalid texture path.\n");
	}
	*texture = ft_strdup(path);
}

void	trim_newline(char *str, char *line)
{
	size_t	i;
	size_t	len;

	i = 0;
	len = 0;
	while (str[i] && !ft_isspace(str[i]))
		i++;
	while (str[i] && ft_isspace(str[i]))
	{
		len++;
		i++;
	}
	if (str[i] != '\0' && str[i] != '\n')
	{
		free(line);
		handle_error("Error: invalid texture path.\n");
	}
	str[i - len] = '\0';
}

void	check_spaces(char *temp, char *mode, char *line)
{
	int	i;

	i = 0;
	if (ft_strncmp(mode, temp, 2) == 0)
		i += 2;
	if (ft_isspace(temp[i]))
		return ;
	else
	{
		free(line);
		handle_error("Error: invalid texture path.\n");
	}
}
