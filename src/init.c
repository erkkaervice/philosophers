/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 15:30:10 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/24 12:19:20 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * ft_initforks - Initializes fork mutexes for each philosopher.
 *
 * Allocates memory for fork mutexes and initializes each one. If allocation
 * or initialization fails, already-initialized mutexes are destroyed, memory
 * is freed, and the function returns 1.
 *
 * Parameters:
 * - data: Pointer to the data struct holding philosopher count and fork array.
 *
 * Returns:
 * - 0 on success, 1 on failure.
 */
static int	ft_initforks(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		ft_printf("What forks?\n");
		return (1);
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			ft_printf("Failed to initialize mutex for fork %d\n", i);
			return (1);
		}
		i++;
	}
	return (0);
}

/*
 * ft_initlocks - Initializes simulation synchronization locks.
 *
 * Initializes write_lock, sim_stop_lock, and last_meal_lock. If any
 * initialization fails, previously-initialized mutexes are destroyed and
 * the function returns 1.
 *
 * Parameters:
 * - data: Pointer to the data struct containing the mutexes.
 *
 * Returns:
 * - 0 on success, 1 on failure.
 */
static int	ft_initlocks(t_data *data)
{
	if (pthread_mutex_init(&data->write_lock, NULL) != 0)
	{
		ft_printf("Failed to initialize write_lock mutex\n");
		return (1);
	}
	if (pthread_mutex_init(&data->sim_stop_lock, NULL) != 0)
	{
		ft_printf("Failed to initialize sim_stop_lock mutex\n");
		pthread_mutex_destroy(&data->write_lock);
		return (1);
	}
	if (pthread_mutex_init(&data->last_meal_lock, NULL) != 0)
	{
		ft_printf("Failed to initialize last_meal_lock mutex\n");
		pthread_mutex_destroy(&data->write_lock);
		pthread_mutex_destroy(&data->sim_stop_lock);
		return (1);
	}
	return (0);
}

/*
 * ft_initphilos - Sets default values for each philosopher.
 *
 * Initializes philosopher fields: ID, meal count, last_meal time, and
 * condition variable. Also assigns fork pointers and back-reference to data.
 *
 * Parameters:
 * - data: Pointer to shared simulation data.
 * - philos: Array of philosopher structs to initialize.
 *
 * Returns:
 * - 0 on success, 1 on failure.
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
		philos[i].thread_done = 0;
		if (pthread_cond_init(&philos[i].done_cond, NULL) != 0)
		{
			ft_printf("Philo %d is in bad condition.\n", i + 1);
			return (1);
		}
		philos[i].data = data;
		philos[i].left_fork = &data->forks[i];
		philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
		i++;
	}
	return (0);
}

/*
 * ft_initmemory - Allocates and fills core simulation data.
 *
 * Allocates memory for the data struct and philosopher array. Assigns
 * arguments to simulation parameters. On failure, prints error and returns
 * NULL.
 *
 * Parameters:
 * - av: Command-line argument array.
 *
 * Returns:
 * - Pointer to initialized data struct, or NULL on failure.
 */
static t_data	*ft_initmemory(char **av)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
	{
		ft_printf("What data?\n");
		return (NULL);
	}
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
		ft_printf("What philosophers?\n");
		free(data);
		return (NULL);
	}
	return (data);
}

/*
 * ft_initdata - Full initialization entry point for simulation.
 *
 * Calls ft_initmemory and validates time arguments. If any required argument
 * is non-positive, frees memory and returns NULL. Then initializes mutexes,
 * forks, and philosophers. On any failure, frees all memory and returns NULL.
 *
 * Parameters:
 * - ac: Argument count.
 * - av: Argument vector.
 *
 * Returns:
 * - Pointer to fully initialized data struct, or NULL on failure.
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
		ft_printf("No tree falls in a forest... are you an idiot?\n");
		free(data->philos);
		free(data);
		return (NULL);
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
