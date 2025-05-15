/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/15 14:52:49 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Entry point of the simulation.
 *
 * Initializes the simulation, creates philosopher threads, and waits 
 * for their completion. Handles the case of a single philosopher separately.
 */
int	main(int ac, char **av)
{
	t_data	*data;

	if (ac != 5 && ac != 6)
		return (ft_printf("Use: ./philo <# phs> <die> <eat> <slp> [max]\n"), 1);
	if (ft_atoi(av[1]) <= 0 || ft_atoi(av[2]) <= 0
		|| ft_atoi(av[3]) <= 0 || ft_atoi(av[4]) <= 0
		|| (ac == 6 && ft_atoi(av[5]) <= 0))
		return (ft_printf("These are not the args you were looking for\n"), 1);
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	ft_threads(data, data->philos);
	ft_cleanup(data, data->philos);
	return (0);
}
