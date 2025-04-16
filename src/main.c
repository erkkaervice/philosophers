/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/16 16:50:17 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_single_philo(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	ft_printlog(philo->data, philo->id, "has taken a fork");
	usleep(philo->data->time_to_die * 1000);
	ft_printlog(philo->data, philo->id, "died");
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->data->sim_stop = 1;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
}

int all_philosophers_done_eating(t_data *data, t_philo *philos)
{
	int i;

	i = 0;
	while (i < data->num_philos)
	{
		if (philos[i].meals_eaten < data->must_eat)  // Check if the philosopher hasn't eaten enough
			return (0);  // Not all philosophers are done
		i++;
	}
	return (1);  // All philosophers have eaten enough
}


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
					pthread_cond_wait(&philos[i].done_cond, lock);  // Wait for thread to finish
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



int	main(int ac, char **av)
{
	t_data	*data;
	t_philo	*philos;

	if (ac != 5 && ac != 6)
	{
		ft_printf("Use: ./philo <# philos> <die> <eat> <sleep> [max meals]\n");
		return (1);
	}
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	philos = data->philos;
	ft_threads(data, philos);
	if (data->num_philos == 1)
	{
		ft_cleanup(data, philos);
		return (0);
	}

	// Main simulation loop
	while (!data->sim_stop)  // Stop when simulation is over
	{
		pthread_mutex_lock(&data->sim_stop_lock);
		// You can add any additional checks here, like checking if all philosophers are done eating
		if (data->must_eat > 0 && all_philosophers_done_eating(data, philos))
		{
			data->sim_stop = 1;  // Stop the simulation when all philosophers are done eating
		}
		pthread_mutex_unlock(&data->sim_stop_lock);

		usleep(100);  // Prevent tight looping
	}

	// Cleanup after simulation finishes
	ft_cleanup(data, philos);
	return (0);
}
