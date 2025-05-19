/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/19 13:18:13 by eala-lah         ###   ########.fr       */
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
	int		i;

	if (ac != 5 && ac != 6)
		ft_error("Usage: ./philo nbr die eat sleep [must_eat]");
	i = 1;
	while (i < ac)
	{
		if (ft_atoi(av[i++]) <= 0)
			ft_error("These are not the args you were looking for");
	}
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	ft_threads(data, data->philos);
	ft_cleanup(data, data->philos);
	return (0);
}
