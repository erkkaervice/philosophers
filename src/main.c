/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/09 12:32:15 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * ft_cleanup - Frees memory and destroys mutexes after the simulation ends.
 *
 * This function ensures that all dynamically allocated memory is freed, and 
 * all mutexes used throughout the simulation are properly destroyed. This 
 * includes cleaning up philosopher threads, mutexes used for forks, and 
 * shared locks used for simulation control.
 *
 * Parameters:
 * - data: A pointer to the data structure containing simulation details.
 * - philos: A pointer to the array of philosopher structures.
 */
void	ft_cleanup(t_data *data, t_philo *philos)
{
	int				i;
	pthread_mutex_t	*lock;

	if (philos)
	{
		i = 0;
		while (i < data->num_philos)
		{
			if (data->num_philos > 1)
			{
				lock = &philos[i].data->sim_stop_lock;
				pthread_mutex_lock(lock);
				while (!philos[i].thread_done)
					pthread_cond_wait(&philos[i].done_cond, lock);
				pthread_mutex_unlock(lock);
				pthread_mutex_destroy(&data->forks[i]);
			}
			i++;
		}
		free(philos);
	}
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
	free(data->forks);
	free(data);
}

/*
 * main - The entry point of the simulation. Initializes data and philosophers,
 *        starts threads, and manages the simulation loop.
 *
 * The function handles initialization of the simulation data, sets up 
 * philosopher threads, and enters the main loop, which continuously checks 
 * for the simulation's stop condition. Once the condition is met, it cleans 
 * up the resources and exits the program.
 *
 * Parameters:
 * - ac: The argument count (should be 5).
 * - av: The argument values (parameters for simulation).
 *
 * Returns:
 * - 0 if the simulation completes successfully.
 * - 1 if the arguments are incorrect or an error occurs.
 */
int	main(int ac, char **av)
{
	t_data	*data;
	t_philo	*philos;

	if (ac != 5)
	{
		ft_printf("Usage: ./philo <# of philos> <die> <eat> <sleep>\n");
		return (1);
	}
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	philos = data->philos;
	ft_threads(data, philos);
	while (1)
	{
		pthread_mutex_lock(&data->sim_stop_lock);
		if (data->sim_stop)
		{
			pthread_mutex_unlock(&data->sim_stop_lock);
			ft_cleanup(data, philos);
			return (0);
		}
		pthread_mutex_unlock(&data->sim_stop_lock);
	}
	return (0);
}
