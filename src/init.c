/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 14:41:11 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	init_mutexes(t_data *data)
{
	int	i;

	// Initialize forks mutexes
	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		printf("Memory allocation failed for forks\n");
		free(data); // Free data if forks allocation fails
		return (1);
	}

	// Initialize each fork mutex
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			printf("Failed to initialize mutex for fork %d\n", i);
			while (i-- > 0) // Free already initialized forks
				pthread_mutex_destroy(&data->forks[i]);
			free(data->forks); // Free forks if initialization fails
			free(data);
			return (1);
		}
		i++;
	}

	// Initialize the write lock and simulation stop lock
	if (pthread_mutex_init(&data->write_lock, NULL) != 0)
	{
		printf("Failed to initialize write_lock mutex\n");
		while (i-- > 0) // Free already initialized forks
			pthread_mutex_destroy(&data->forks[i]);
		free(data->forks); // Free forks if initialization fails
		free(data);
		return (1);
	}

	if (pthread_mutex_init(&data->sim_stop_lock, NULL) != 0)
	{
		printf("Failed to initialize sim_stop_lock mutex\n");
		pthread_mutex_destroy(&data->write_lock); // Destroy the write lock
		while (i-- > 0) // Free already initialized forks
			pthread_mutex_destroy(&data->forks[i]);
		free(data->forks); // Free forks if initialization fails
		free(data);
		return (1);
	}

	return (0);
}

int	init_philos(t_data *data, t_philo *philos)
{
	int	i;

	// Initialize philosophers
	i = 0;
	while (i < data->num_philos)
	{
		philos[i].id = i + 1;
		philos[i].meals_eaten = 0;
		philos[i].last_meal = get_time();
		philos[i].data = data;
		philos[i].left_fork = &data->forks[i];
		philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
		i++;
	}

	return (0);
}

t_data	*init_data(int argc, char **argv)
{
	t_data	*data;

	// Allocate memory for data
	data = malloc(sizeof(t_data));
	if (!data)
	{
		printf("Memory allocation failed for data\n");
		return (NULL);
	}

	// Initialize the number of philosophers before allocating memory for them
	data->num_philos = ft_atoi(argv[1]);

	// Allocate memory for philosophers
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
	{
		printf("Memory allocation failed for philosophers\n");
		free(data);  // Clean up data if philosopher allocation fails
		return (NULL);
	}

	// Initialize data fields
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	data->must_eat = (argc == 6) ? ft_atoi(argv[5]) : -1;
	data->sim_stop = 0;
	data->start_time = get_time();

	// Initialize mutexes
	if (init_mutexes(data) != 0)
	{
		printf("Mutex initialization failed\n");
		free(data->philos); // Clean up philosophers
		free(data); // Clean up memory if mutex initialization fails
		return (NULL);
	}

	// Initialize philosophers
	if (init_philos(data, data->philos) != 0)
	{
		free(data->forks); // Clean up forks if philosophers initialization fails
		free(data->philos); // Clean up philosophers
		free(data); // Clean up data
		return (NULL);
	}

	return (data);
}
