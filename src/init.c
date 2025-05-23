/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 15:30:10 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/23 16:02:16 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Initializes fork mutexes for each philosopher.
 *
 * Allocates memory for forks and initializes one mutex per fork.
 * On failure, destroys initialized mutexes and frees the fork array.
 */
static int	ft_initforks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
		return (printf("What forks?\n"), 1);
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			return (printf("Failed mutex for fork %d\n", i), 1);
		}
		i++;
	}
	return (0);
}

/*
 * Initializes core simulation mutexes.
 *
 * Sets up write_lock, sim_stop_lock, and last_meal_lock in order.
 * On failure, any previously initialized mutexes are cleaned up.
 */
static int	ft_initlocks(t_data *data)
{
	if (pthread_mutex_init(&data->write_lock, NULL) != 0)
		return (printf("Failed mutex for write_lock\n"), 1);
	if (pthread_mutex_init(&data->sim_stop_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->write_lock);
		return (printf("Failed mutex for sim_stop_lock\n"), 1);
	}
	if (pthread_mutex_init(&data->last_meal_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->write_lock);
		pthread_mutex_destroy(&data->sim_stop_lock);
		return (printf("Failed mutex for last_meal_lock\n"), 1);
	}
	return (0);
}

/*
 * Assigns fork pointers and default values to each philosopher.
 *
 * Each philosopher receives left and right fork pointers in a circular
 * arrangement. Also sets ID, meal counter, thread, and timestamp.
 */
static int	ft_initphilos(t_data *data, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		philos[i].last_meal = ft_time();
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].thread = 0;
		philos[i].left_fork = &data->forks[i];
		philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
		philos[i].data = data;
		i++;
	}
	return (0);
}

/*
 * Allocates and configures main simulation structures.
 *
 * Sets timing and configuration values from input arguments.
 * Allocates memory for data and philosopher array. On failure,
 * prints a descriptive error and returns NULL.
 */
static t_data	*ft_initmemory(int ac, char **av)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
		return (printf("What data?\n"), NULL);
	data->start_time = ft_time();
	data->num_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		data->must_eat = ft_atoi(av[5]);
	else
		data->must_eat = -1;
	data->sim_stop = 0;
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
	{
		free(data);
		return (printf("What philosophers?\n"), NULL);
	}
	return (data);
}

/*
 * Full initialization routine for the simulation.
 *
 * Runs memory allocation, mutex and fork initialization, and 
 * philosopher setup in order. On failure at any step, cleans up
 * everything and returns NULL.
 */
t_data	*ft_initdata(int ac, char **av)
{
	t_data	*data;

	data = ft_initmemory(ac, av);
	if (!data)
		return (NULL);
	if (ft_initforks(data))
		return (free(data), NULL);
	if (ft_initlocks(data))
	{
		free(data->forks);
		return (free(data), NULL);
	}
	if (ft_initphilos(data, data->philos))
	{
		pthread_mutex_destroy(&data->write_lock);
		pthread_mutex_destroy(&data->sim_stop_lock);
		pthread_mutex_destroy(&data->last_meal_lock);
		free(data->forks);
		free(data->philos);
		return (free(data), NULL);
	}
	return (data);
}
