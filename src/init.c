/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 15:30:10 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/24 15:56:56 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Initializes fork mutexes for each philosopher.
 *
 * Allocates memory for fork mutexes and initializes each one. If initialization
 * fails, already-initialized mutexes are destroyed, and memory is freed.
 */
static int	ft_initforks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * (data->num_philos + 2));
	if (!data->forks)
		return (ft_printf("What forks?\n"), 1);
	i = 0;
	while (i < data->num_philos + 2)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			return (ft_printf("Failed mutex for fork %d\n", i), 1);
		}
		i++;
	}
	return (0);
}

/*
 * Initializes simulation synchronization locks.
 *
 * Initializes write_lock, sim_stop_lock, and last_meal_lock. If any
 * initialization fails, previously-initialized mutexes are destroyed.
 */
static int	ft_initlocks(t_data *data)
{
	if (pthread_mutex_init(&data->write_lock, NULL) != 0)
		return (ft_printf("Failed mutex for write_lock\n"), 1);
	if (pthread_mutex_init(&data->sim_stop_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->write_lock);
		return (ft_printf("Failed mutex for sim_stop_lock\n"), 1);
	}
	if (pthread_mutex_init(&data->last_meal_lock, NULL) != 0)
	{
		pthread_mutex_destroy(&data->write_lock);
		pthread_mutex_destroy(&data->sim_stop_lock);
		return (ft_printf("Failed mutex for last_meal_lock\n"), 1);
	}
	return (0);
}

/*
 * Sets default values for each philosopher.
 *
 * Initializes philo fields, back-reference to data and assigns fork pointers.
 */
static int	ft_initphilos(t_data *data, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		philos[i].last_meal = ft_time();
		philos[i].start_eating = 0;
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].thread_done = 0;
		if (pthread_cond_init(&philos[i].done_cond, NULL) != 0)
			return (ft_printf("Philo %d is in no condition.\n", i + 1), 1);
		philos[i].data = data;
		philos[i].left_fork = &data->forks[i + 1];
		philos[i].right_fork = &data->forks[(i + 2) % data->num_philos];
		i++;
	}
	return (0);
}

/*
 * Allocates and fills core simulation data.
 *
 * Allocates memory for the data struct and philosopher array. Assigns
 * arguments to simulation parameters. On failure, prints error.
 */
static t_data	*ft_initmemory(char **av)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
		return (ft_printf("What data?\n"), NULL);
	data->start_time = ft_time();
	data->num_philos = ft_atoi(av[1]);
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	data->sim_stop = 0;
	data->must_eat = -1;
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
	{
		free(data);
		return (ft_printf("What philosophers?\n"), NULL);
	}
	return (data);
}

/*
 * Full initialization entry point for simulation.
 *
 * Calls ft_initmemory and validates time arguments. If any required argument
 * is non-positive, frees memory and returns NULL. Then initializes mutexes,
 * forks, and philosophers. On any failure, frees all memory and returns NULL.
 */
t_data	*ft_initdata(int ac, char **av)
{
	t_data	*data;

	data = ft_initmemory(av);
	if (!data)
		return (NULL);
	if (data->time_to_die <= 0 || data->time_to_eat <= 0
		|| data->time_to_sleep <= 0)
	{
		free(data->philos);
		free(data);
		return (ft_printf("Brain much with the values?\n"), NULL);
	}
	if (ac == 6)
		data->must_eat = ft_atoi(av[5]);
	if (ft_initforks(data)
		|| ft_initlocks(data)
		|| ft_initphilos(data, data->philos))
	{
		free(data->philos);
		free(data->forks);
		free(data);
		return (NULL);
	}
	return (data);
}
