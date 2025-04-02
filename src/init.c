/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 15:57:27 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

static int	init_mutexes(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
	if (!data->forks)
	{
		printf("Memory allocation failed for forks\n");
		free(data);
		return (1);
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_mutex_init(&data->forks[i], NULL) != 0)
		{
			printf("Failed to initialize mutex for fork %d\n", i);
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

static int	init_mutexes_locks(t_data *data)
{
	if (pthread_mutex_init(&data->write_lock, NULL) != 0)
	{
		printf("Failed to initialize write_lock mutex\n");
		return (1);
	}
	if (pthread_mutex_init(&data->sim_stop_lock, NULL) != 0)
	{
		printf("Failed to initialize sim_stop_lock mutex\n");
		pthread_mutex_destroy(&data->write_lock);
		return (1);
	}
	return (0);
}

static void	init_philos(t_data *data, t_philo *philos)
{
	int	i;

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
}

static t_data	*init_alloc_data(char **argv)
{
	t_data	*data;

	data = malloc(sizeof(t_data));
	if (!data)
	{
		printf("Memory allocation failed for data\n");
		return (NULL);
	}
	data->num_philos = ft_atoi(argv[1]);
	data->philos = malloc(sizeof(t_philo) * data->num_philos);
	if (!data->philos)
	{
		printf("Memory allocation failed for philosophers\n");
		free(data);
		return (NULL);
	}
	data->time_to_die = ft_atoi(argv[2]);
	data->time_to_eat = ft_atoi(argv[3]);
	data->time_to_sleep = ft_atoi(argv[4]);
	data->sim_stop = 0;
	data->start_time = get_time();
	return (data);
}

t_data	*init_data(int argc, char **argv)
{
	t_data	*data;

	data = init_alloc_data(argv);
	if (!data)
		return (NULL);
	if (argc == 6)
		data->must_eat = ft_atoi(argv[5]);
	else
		data->must_eat = -1;
	if (init_mutexes(data) != 0)
	{
		free(data->philos);
		free(data);
		return (NULL);
	}
	if (init_mutexes_locks(data) != 0)
	{
		free(data->forks);
		free(data->philos);
		free(data);
		return (NULL);
	}
	init_philos(data, data->philos);
	return (data);
}
