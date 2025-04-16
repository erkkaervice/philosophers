/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/04 15:30:10 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/16 16:00:00 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"


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
	data->must_eat = -1;
	return (data);
}


t_data	*ft_initdata(int ac, char **av)
{
	t_data	*data;

	data = ft_initmemory(av);
	if (!data)
		return (NULL);
	if (data->time_to_die <= 0 || data->time_to_eat <= 0
		|| data->time_to_sleep <= 0)
	{
		ft_printf("If a tree falls in a forest... are you an idiot?\n");
		free(data);
		return (NULL);
	}
	if (ac == 6)
		data->must_eat = ft_atoi(av[5]);
	if (ft_initforks(data) || ft_initlocks(data)
		|| ft_initphilos(data, data->philos))
	{
		free(data);
		return (NULL);
	}
	return (data);
}
