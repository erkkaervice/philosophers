/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 15:30:10 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/09 13:35:01 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * ft_initforks - Initializes the mutexes for the forks.
 *
 * This function allocates memory for the forks mutexes and initializes each
 * mutex. If memory allocation or mutex initialization fails, the function 
 * returns 1, cleans up any allocated memory, and prints an error message.
 * If everything succeeds, it returns 0.
 *
 * Parameters:
 * - data: The data structure containing the number of philosophers and 
 *         the array of forks mutexes.
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
		ft_printf("Memory allocation failed for forks\n");
		free(data);
		return (1);
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			ft_printf("Failed to initialize mutex for fork %d\n", i);
			while (i-- > 0)
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks);
			free(data);
			return (1);
		}
		i++;
	}
	return (0);
}

/*
 * ft_initlocks - Initializes the necessary locks for synchronization.
 *
 * This function initializes the mutexes used for synchronizing writes, 
 * stopping the simulation, and tracking the last meal time. If any mutex 
 * initialization fails, the function returns 1, cleans up previously 
 * initialized mutexes, and prints an error message.
 *
 * Parameters:
 * - data: The data structure containing the mutexes for synchronization.
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
 * ft_initphilos - Initializes the philosophers with default values.
 *
 * This function sets up each philosopher with an ID, meal count, and initial 
 * last meal time. It also sets up the mutexes for synchronization and assigns 
 * the left and right forks for each philosopher.
 *
 * Parameters:
 * - data: The data structure containing information about the simulation.
 * - philos: The array of philosophers to initialize.
 *
 * Returns:
 * - 0 on success.
 */
int	ft_initphilos(t_data *data, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		philos[i].thread_done = 0;
		pthread_cond_init(&philos[i].done_cond, NULL);
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].last_meal = ft_time();
		philos[i].data = data;
		philos[i].left_fork = &data->forks[i];
		philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
		i++;
	}
	return (0);
}

/*
 * ft_initmemory - Allocates memory for the data structure.
 *
 * This function allocates memory for the main simulation data structure 
 * and the philosophers array. It initializes the parameters using values 
 * passed through `av`. If memory allocation fails, an error message is printed, 
 * and the function returns NULL.
 *
 * Parameters:
 * - av: The argument vector containing command-line arguments.
 *
 * Returns:
 * - A pointer to the initialized `t_data` structure, or NULL on failure.
 */
static t_data	*ft_initmemory(char **av)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
	{
		ft_printf("Memory allocation failed for data\n");
		return (NULL);
	}
	data->num_philos = ft_atoi(av[1]);
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
	{
		ft_printf("Memory allocation failed for philosophers\n");
		free(data);
		return (NULL);
	}
	data->time_to_die = ft_atoi(av[2]);
	data->time_to_eat = ft_atoi(av[3]);
	data->time_to_sleep = ft_atoi(av[4]);
	data->sim_stop = 0;
	data->start_time = ft_time();
	return (data);
}

/*
 * ft_initdata - Initializes the data structure for the simulation.
 *
 * This function checks the validity of the input parameters and initializes 
 * the main data structure, mutexes, and philosophers. If any initialization 
 * fails, the function frees allocated memory and returns NULL.
 *
 * Parameters:
 * - ac: The argument count (unused in this function).
 * - av: The argument vector containing command-line arguments.
 *
 * Returns:
 * - A pointer to the initialized `t_data` structure, or NULL on failure.
 */
t_data	*ft_initdata(int ac, char **av)
{
	t_data	*data;

	(void)ac;
	data = ft_initmemory(av);
	if (!data)
		return (NULL);
	if (data->time_to_die <= 0 || data->time_to_eat <= 0
		|| data->time_to_sleep <= 0 || data->num_philos == 1)
	{
		ft_printf("If a tree falls in a forest... are you an idiot?\n");
		free(data);
		return (NULL);
	}
	if (ft_initforks(data) || ft_initlocks(data)
		|| ft_initphilos(data, data->philos))
	{
		free(data);
		return (NULL);
	}
	return (data);
}
