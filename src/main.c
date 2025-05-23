/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/23 16:09:59 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Checks if the given character is a numeric digit (0–9).
 */
int	ft_isdigit(int c)
{
	return (c >= '0' && c <= '9');
}

/*
 * Checks if the given character is a whitespace (space, tab, newline, etc).
 */
int	ft_isspace(int c)
{
	return ((c > 8 && c < 14)
		|| (c == 32));
}

/*
 * Converts a string to an integer with strict validation.
 * Skips leading whitespace, accepts optional + or - sign,
 * and returns 0 if the string contains non-numeric characters.
 */
int	ft_atoi(char const *str)
{
	long long int	n;
	int				sign;
	long long int	dig;

	n = 0;
	sign = 1;
	while (*str && ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign *= -1;
		str++;
	}
	if (!ft_isdigit(*str))
		return (0);
	while (*str && ft_isdigit(*str))
	{
		dig = *str - '0';
		n = n * 10 + sign * dig;
		str++;
	}
	if (*str != '\0')
		return (0);
	return ((int)n);
}

/*
 * Entry point of the program.
 *
 * Validates input arguments, initializes simulation data, launches
 * philosopher threads, and performs cleanup after the simulation.
 */
int	main(int ac, char **av)
{
	t_data	*data;
	int		i;

	if (ac != 5 && ac != 6)
		return (printf("Usage: ./philo nbr die eat sleep [must_eat]\n"), 1);
	i = 1;
	while (i < ac)
	{
		if (ft_atoi(av[i++]) <= 0)
			return (printf("These are not the args you were looking for\n"), 1);
	}
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	ft_threads(data, data->philos);
	ft_cleanup(data, data->philos);
	return (0);
}
