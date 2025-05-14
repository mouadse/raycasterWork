/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   vector_ops.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: msennane <msennane@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/06 11:43:25 by msennane          #+#    #+#             */
/*   Updated: 2025/05/06 11:47:53 by msennane         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

t_vec2	create_vector(float x, float y)
{
	t_vec2	vector;

	vector.x = x;
	vector.y = y;
	return (vector);
}

t_vec2	vector_add(t_vec2 a, t_vec2 b)
{
	t_vec2	result;

	result.x = a.x + b.x;
	result.y = a.y + b.y;
	return (result);
}

t_vec2	vector_subtract(t_vec2 a, t_vec2 b)
{
	t_vec2	result;

	result.x = a.x - b.x;
	result.y = a.y - b.y;
	return (result);
}

t_vec2	vector_multiply(t_vec2 a, float scalar)
{
	t_vec2	result;

	result.x = a.x * scalar;
	result.y = a.y * scalar;
	return (result);
}

float	vector_magnitude(t_vec2 a)
{
	t_vec2	result;

	result.x = sqrt(a.x * a.x + a.y * a.y);
	result.y = 0;
	return (result.x);
}

/**
 * Rotates a 2D vector using a rotation matrix.
 *
 * The rotation matrix used is:
 *     | cosθ  -sinθ |
 *     | sinθ   cosθ |
 * where θ is the rotation angle in radians.
 * This matrix rotates the vector by θ degrees counterclockwise.
 *
 * @param vector The vector to rotate.
 * @param angle The angle in degrees.
 * @return The rotated vector.
 */

t_vec2	rotate_vector(t_vec2 vector, float angle)
{
	t_vec2	result;
	float	rad;

	rad = angle * (M_PI / 180.0);
	result.x = vector.x * cos(rad) - vector.y * sin(rad);
	result.y = vector.x * sin(rad) + vector.y * cos(rad);
	return (result);
}
