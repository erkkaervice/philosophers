/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tasks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:49:21 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/16 16:50:17 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_printlog(t_data *data, int id, char *msg)
{
	int	time;

	pthread_mutex_lock(&data->write_lock);  // Lock to avoid concurrent write access
	time = (int)(ft_time() - data->start_time);
	ft_printf("%d %d %s\n", time, id, msg);
	pthread_mutex_unlock(&data->write_lock);  // Unlock after writing log
}

long long	ft_time(void)
{
	struct timeval	tv;
	long long		ms;

	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

int	ft_forks(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);  // Lock left fork
	pthread_mutex_lock(&philo->data->sim_stop_lock);  // Lock simulation stop condition
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);  // Unlock sim stop lock
		pthread_mutex_unlock(philo->left_fork);  // Unlock left fork
		return (0);
	}
	ft_printlog(philo->data, philo->id, "has taken a fork");
	pthread_mutex_unlock(&philo->data->sim_stop_lock);  // Unlock sim stop lock
	pthread_mutex_lock(philo->right_fork);  // Lock right fork
	pthread_mutex_lock(&philo->data->sim_stop_lock);  // Lock sim stop condition
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);  // Unlock sim stop lock
		pthread_mutex_unlock(philo->right_fork);  // Unlock right fork
		pthread_mutex_unlock(philo->left_fork);  // Unlock left fork
		return (0);
	}
	ft_printlog(philo->data, philo->id, "has taken a fork");
	ft_printlog(philo->data, philo->id, "is eating");
	philo->last_meal = ft_time();
	pthread_mutex_unlock(&philo->data->sim_stop_lock);  // Unlock sim stop lock
	return (1);
}

void	ft_eat(t_philo *philo)
{
	long long	start_time;

	// Check stop condition or if unable to take forks
	if (ft_stoplock(philo) || !ft_forks(philo))
		return ;

	// Skip eating if philosopher has already eaten enough (based on must_eat)
	if (philo->data->must_eat > 0 && philo->meals_eaten >= philo->data->must_eat)
	{
		return ;
	}

	start_time = ft_time();
	while (ft_time() - start_time < philo->data->time_to_eat)
		usleep(100); // Simulate eating for the required time

	// Lock to modify shared data (meal count and last_meal)
	pthread_mutex_lock(&philo->data->sim_stop_lock);

	// Update last_meal and increment meals_eaten
	if (!philo->data->sim_stop && philo->meals_eaten < philo->data->must_eat)
	{
		philo->last_meal = ft_time(); // Update last meal timestamp
		philo->meals_eaten++; // Increment meals eaten
	}

	pthread_mutex_unlock(&philo->data->sim_stop_lock);  // Unlock sim stop lock

	// Release forks after eating
	pthread_mutex_unlock(philo->right_fork);  // Unlock right fork
	pthread_mutex_unlock(philo->left_fork);  // Unlock left fork
}

void	ft_sleepthink(t_philo *philo)
{
	long long	start_time;

	if (ft_stoplock(philo))
		return ;
	ft_printlog(philo->data, philo->id, "is sleeping");
	start_time = ft_time();
	while (ft_time() - start_time < philo->data->time_to_sleep)
	{
		if (ft_stoplock(philo))
			return ;
		usleep(50);
	}
	if (ft_stoplock(philo))
		return ;
	ft_printlog(philo->data, philo->id, "is thinking");
}
