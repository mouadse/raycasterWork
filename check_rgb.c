/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_rgb.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/10 12:27:04 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 16:45:35 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

static void	handle_error_rgb(char *original, char **rgb);
static void	control_characters(char *rgb_color, char *original, char c);

void	check_rgb(uint32_t *color, char *rgb_color, char *original, char c)
{
	char	**rgb;
	int		i;
	int		j;

	i = 0;
	control_characters(rgb_color, original, c);
	split_rgb(&rgb, rgb_color, original, c);
	while (rgb[i])
	{
		j = 0;
		while (rgb[i][j])
		{
			if (!ft_isdigit(rgb[i][j]))
				handle_error_rgb(original, rgb);
			j++;
		}
		if (ft_atoi(rgb[i]) < 0 || ft_atoi(rgb[i]) > 255)
			handle_error_rgb(original, rgb);
		i++;
	}
	*color = convert_rgb(ft_atoi(rgb[0]), ft_atoi(rgb[1]), ft_atoi(rgb[2]));
	ft_free_matrix(rgb);
}

static void	control_characters(char *rgb_color, char *original, char c)
{
	int	qtd;
	int	space;

	qtd = 0;
	space = 0;
	while (ft_isspace(*rgb_color) || *rgb_color == c)
	{
		if (*rgb_color == c)
			qtd++;
		if (ft_isspace(*rgb_color))
			space++;
		rgb_color++;
	}
	if (qtd != 1 || space < 1)
	{
		free(original);
		handle_error("Error: invalid rgb color.\n");
	}
}

void	split_rgb(char ***rgb, char *rgb_color, char *original, char c)
{
	char	*temp_strim;
	int		i;

	i = 0;
	while (ft_isspace(*rgb_color) || *rgb_color == c)
		rgb_color++;
	*rgb = ft_split(rgb_color, ',');
	if (!*rgb)
	{
		free(original);
		handle_error("Error: invalid rgb color.\n");
	}
	while ((*rgb)[i])
	{
		if (ft_strncmp((*rgb)[i], "\n", 1) == 0 || ft_isspace((*rgb)[i][0]))
			handle_error_rgb(original, *rgb);
		temp_strim = ft_strtrim((*rgb)[i], " \t\v\f\r\n\b");
		free((*rgb)[i]);
		(*rgb)[i] = temp_strim;
		i++;
	}
	if (i != 3)
		handle_error_rgb(original, *rgb);
}

uint32_t	convert_rgb(int r, int g, int b)
{
	// Cast r, g, b to uint32_t before shifting
	return (((uint32_t)r << 24) | ((uint32_t)g << 16) | ((uint32_t)b << 8) | 255);
}

static void	handle_error_rgb(char *original, char **rgb)
{
	free(original);
	ft_free_matrix(rgb);
	handle_error("Error: invalid rgb color.\n");
}
