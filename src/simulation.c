/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 15:18:29 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	start_threads(t_data *data, t_philo *philos)
{
	int i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
		{
			printf("Error creating thread for philosopher %d\n", philos[i].id);
			return;
		}
		i++;
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_join(philos[i].thread, NULL) != 0)
		{
			printf("Error joining thread for philosopher %d\n", philos[i].id);
			return;
		}
		i++;
	}
}

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		eat(philo);
		sleep_think(philo);
		if (check_simulation_status(philo->data, philo->data->philos))
		{
			print_log(philo->data, philo->id, "stopped simulation");
			break;
		}
	}
	return (NULL);
}

int	check_simulation_status(t_data *data, t_philo *philos)
{
	int i;

	if (!data || !philos)
	{
		printf("Error: Invalid data or philosopher pointers\n");
		return (1);
	}
	i = 0;
	pthread_mutex_lock(&data->sim_stop_lock);
	if (data->sim_stop)
	{
		pthread_mutex_unlock(&data->sim_stop_lock);
		return (1);
	}
	while (i < data->num_philos)
	{
		if (get_time() - philos[i].last_meal >= data->time_to_die)
		{
			data->sim_stop = 1;
			pthread_mutex_unlock(&data->sim_stop_lock);
			print_log(data, philos[i].id, "has died");
			return (1);
		}
		i++;
	}
	pthread_mutex_unlock(&data->sim_stop_lock);
	return (0);
}

void	eat(t_philo *philo)
{
	t_fork	*first;
	t_fork	*second;

	first = philo->left_fork;
	second = philo->right_fork;
	pthread_mutex_lock(first);
	pthread_mutex_lock(second);
	print_log(philo->data, philo->id, "is eating");
	ft_usleep(philo->data->time_to_eat);
	philo->last_meal = get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(second);
	pthread_mutex_unlock(first);
}

void	sleep_think(t_philo *philo)
{
	print_log(philo->data, philo->id, "is sleeping");
	ft_usleep(philo->data->time_to_sleep);
	print_log(philo->data, philo->id, "is thinking");
}
