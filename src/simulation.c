/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/04 15:17:54 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	start_threads(t_data *data, t_philo *philos)
{
	int	i;

	if (data->num_philos == 1)
		return ;
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
		{
			ft_printf("Error creating thread for philosopher %d\n", philos[i].id);
			return ;
		}
		philos[i].thread_done = 0;
		i++;
	}
	while (1)
	{
		pthread_mutex_lock(&data->sim_stop_lock);
		if (data->sim_stop)
		{
			pthread_mutex_unlock(&data->sim_stop_lock);
			break ;
		}
		pthread_mutex_unlock(&data->sim_stop_lock);
		ft_usleep(10);
	}
	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_lock(&philos[i].data->sim_stop_lock);
		while (!philos[i].thread_done)
			pthread_cond_wait(&philos[i].done_cond, &philos[i].data->sim_stop_lock);
		pthread_mutex_unlock(&philos[i].data->sim_stop_lock);

		if (pthread_join(philos[i].thread, NULL) != 0)
			ft_printf("Error joining thread for philosopher %d\n", philos[i].id);
		i++;
	}
}

void	*routine(void *arg)
{
	t_philo		*philo;
	long long	current_time;

	philo = (t_philo *)arg;
	while (1)
	{
		pthread_mutex_lock(&philo->data->sim_stop_lock);
		if (philo->data->sim_stop)
		{
			pthread_mutex_unlock(&philo->data->sim_stop_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		current_time = get_time();
		if (current_time - philo->last_meal >= philo->data->time_to_die)
		{
			pthread_mutex_lock(&philo->data->sim_stop_lock);
			philo->data->sim_stop = 1;
			pthread_mutex_unlock(&philo->data->sim_stop_lock);

			pthread_mutex_lock(&philo->data->write_lock);
			ft_printf("Philosopher %d has died\n", philo->id);
			pthread_mutex_unlock(&philo->data->write_lock);
			break ;
		}
		eat(philo);
		pthread_mutex_lock(&philo->data->sim_stop_lock);
		if (philo->data->sim_stop)
		{
			pthread_mutex_unlock(&philo->data->sim_stop_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->sim_stop_lock);

		sleep_think(philo);
		pthread_mutex_lock(&philo->data->sim_stop_lock);
		if (philo->data->sim_stop)
		{
			pthread_mutex_unlock(&philo->data->sim_stop_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
	}
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->thread_done = 1;
	pthread_cond_signal(&philo->done_cond);
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (NULL);
}

int	check_simulation_status(t_data *data, t_philo *philos)
{
	int			i;
	long long	current_time;

	pthread_mutex_lock(&data->sim_stop_lock);
	if (data->sim_stop)
	{
		pthread_mutex_unlock(&data->sim_stop_lock);
		return (1);
	}
	pthread_mutex_unlock(&data->sim_stop_lock);
	i = 0;
	while (i < data->num_philos)
	{
		current_time = get_time();
		if (current_time - philos[i].last_meal >= data->time_to_die)
		{
			pthread_mutex_lock(&data->sim_stop_lock);
			data->sim_stop = 1;
			pthread_mutex_unlock(&data->sim_stop_lock);

			pthread_mutex_lock(&data->write_lock);
			ft_printf("%d has died\n", philos[i].id);
			pthread_mutex_unlock(&data->write_lock);
			return (1);
		}
		i++;
	}
	return (0);
}

void	eat(t_philo *philo)
{
	t_fork	*first;
	t_fork	*second;

	first = philo->left_fork;
	second = philo->right_fork;
	pthread_mutex_lock(first);
	print_log(philo->data, philo->id, "has taken a fork");
	pthread_mutex_lock(second);
	print_log(philo->data, philo->id, "has taken a fork");
	print_log(philo->data, philo->id, "is eating");
	ft_usleep(philo->data->time_to_eat);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->last_meal = get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	pthread_mutex_unlock(second);
	pthread_mutex_unlock(first);
}

void	sleep_think(t_philo *philo)
{
	print_log(philo->data, philo->id, "is sleeping");
	ft_usleep(philo->data->time_to_sleep);
	print_log(philo->data, philo->id, "is thinking");
}
