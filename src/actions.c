/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:49:21 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/23 16:16:15 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Attempts to acquire both forks for a philosopher.
 *
 * Locks left and right forks. If the simulation stopped, unlocks 
 * forks immediately and returns 0 to indicate failure.
 * Logs fork pickup twice on success.
 */
int	ft_forks(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	if (ft_stoplock(philo))
	{
		pthread_mutex_unlock(philo->left_fork);
		return (0);
	}
	pthread_mutex_lock(philo->right_fork);
	if (ft_stoplock(philo))
	{
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		return (0);
	}
	ft_printlog(philo, "has taken a fork");
	ft_printlog(philo, "has taken a fork");
	return (1);
}

/*
 * Simulates eating for a philosopher.
 *
 * Checks stop condition and fork acquisition. Updates last meal time 
 * and meal count with mutex protection. Sleeps for eating duration 
 * and releases forks after eating.
 */
void	ft_eat(t_philo *philo)
{
	if (ft_stoplock(philo) || !ft_forks(philo))
		return ;
	pthread_mutex_lock(&philo->data->last_meal_lock);
	philo->last_meal = ft_time();
	pthread_mutex_unlock(&philo->data->last_meal_lock);
	ft_printlog(philo, "is eating");
	ft_usleep(philo, philo->data->time_to_eat);
	pthread_mutex_lock(&philo->data->last_meal_lock);
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->last_meal_lock);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

/*
 * Simulates sleeping and thinking phases.
 *
 * Logs sleep, sleeps for the configured duration, then logs thinking 
 * unless the simulation is stopped in between.
 */
void	ft_sleepthink(t_philo *philo)
{
	if (ft_stoplock(philo))
		return ;
	ft_printlog(philo, "is sleeping");
	ft_usleep(philo, philo->data->time_to_sleep);
	if (ft_stoplock(philo))
		return ;
	ft_printlog(philo, "is thinking");
}

/*
 * Logs a philosopher's action in a thread-safe manner.
 *
 * Prints the timestamp relative to simulation start, philosopher ID, 
 * and message, using a mutex to prevent output clashes.
 */
void	ft_printlog(t_philo *philo, char *msg)
{
	long long	timestamp;

	pthread_mutex_lock(&philo->data->write_lock);
	if (!ft_stoplock(philo))
	{
		timestamp = ft_time() - philo->data->start_time;
		printf("%lld %d %s\n", timestamp, philo->id, msg);
	}
	pthread_mutex_unlock(&philo->data->write_lock);
}

/*
 * Returns current time in milliseconds.
 *
 * Uses gettimeofday to get the current time, converts it to 
 * milliseconds, and returns the value.
 */
long long	ft_time(void)
{
	struct timeval	tv;
	long long		ms;

	if (gettimeofday(&tv, NULL) == -1)
		return (-1);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}
