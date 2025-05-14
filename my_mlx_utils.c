/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   my_mlx_utils.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/07 12:44:35 by msennane          #+#    #+#             */
/*   Updated: 2025/05/07 12:48:13 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include <sys/time.h>

uint32_t	tex_pixel(t_tex *t, int x, int y)
{
	char	*p;

	if (x < 0 || x >= t->w || y < 0 || y >= t->h)
		return (0);
	p = t->addr + (y * t->line_len + x * (t->bpp / 8));
	return (*(uint32_t *)p);
}

void	init_image(t_cub3d *g, int w, int h)
{
	g->img.ptr = mlx_new_image(g->mlx, w, h);
	if (!g->img.ptr)
	{
		perror("mlx_new_image");
		exit(EXIT_FAILURE);
	}
	g->img.addr = mlx_get_data_addr(g->img.ptr, &g->img.bpp, &g->img.line_len,
			&g->img.endian);
	g->img.w = w;
	g->img.h = h;
}

void	my_mlx_pixel_put(t_img *img, int x, int y, uint32_t col)
{
	char	*dst;

	if (x < 0 || x >= img->w || y < 0 || y >= img->h)
		return ;
	dst = img->addr + (y * img->line_len + x * (img->bpp / 8));
	*(uint32_t *)dst = col;
}

double	now_sec(void)
{
	// Warning:
	// To be removed later and replaced with a low level function to get the time
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec + tv.tv_usec / 1e6);
}
