/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:49:21 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/09 11:38:10 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Attempts to acquire both forks for a philosopher to eat.
 *
 * Locks the left and right forks, ensuring no other philosopher 
 * can access them simultaneously. If the simulation is stopped, 
 * the forks are unlocked and the philosopher cannot proceed.
 */
int	ft_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		ft_printlog(philo, "has taken a fork");
		pthread_mutex_lock(philo->right_fork);
		ft_printlog(philo, "has taken a fork");
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		ft_printlog(philo, "has taken a fork");
		pthread_mutex_lock(philo->left_fork);
		ft_printlog(philo, "has taken a fork");
	}
	return (1);
}

/*
 * Simulates the philosopher eating.
 *
 * After successfully acquiring both forks, the philosopher eats 
 * for a specified duration. The philosopher's meal count is updated, 
 * and the forks are released once the eating phase is complete.
 */
void	ft_eat(t_philo *philo)
{
	if (ft_stoplock(philo))
		return ;
	if (!ft_forks(philo))
		return ;
	pthread_mutex_lock(&philo->data->last_meal_lock);
	philo->last_meal = ft_time();
	pthread_mutex_unlock(&philo->data->last_meal_lock);
	ft_printlog(philo, "is eating");
	ft_usleep(philo->data->time_to_eat);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (!philo->data->sim_stop
		&& (philo->data->must_eat < 0
			|| philo->meals_eaten < philo->data->must_eat))
		philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
}

/*
 * Simulates the philosopher sleeping and thinking.
 *
 * After eating, the philosopher sleeps for the specified duration. 
 * Upon waking, the philosopher thinks, and the state is logged.
 */
void	ft_sleepthink(t_philo *philo)
{
	long long	start_time;

	if (ft_stoplock(philo))
		return ;
	ft_printlog(philo, "is sleeping");
	start_time = ft_time();
	while (ft_time() - start_time < philo->data->time_to_sleep)
	{
		if (ft_stoplock(philo))
			return ;
		usleep(50);
	}
	if (ft_stoplock(philo))
		return ;
	ft_printlog(philo, "is thinking");
}

/*
 * Prints a log message for a philosopher's actions.
 *
 * The log message includes the current time, philosopher's ID, 
 * and the action being performed. A mutex lock is used to ensure 
 * thread-safe printing.
 */
void	ft_printlog(t_philo *philo, char *msg)
{
	int	timestamp;

	if (ft_stoplock(philo))
		return ;
	pthread_mutex_lock(&philo->data->write_lock);
	timestamp = (int)(ft_time() - philo->data->start_time);
	ft_printf("%d %d %s\n", timestamp, philo->id, msg);
	pthread_mutex_unlock(&philo->data->write_lock);
}

/*
 * Returns the current time in milliseconds since the simulation start.
 *
 * This function uses `gettimeofday` to retrieve the current time, 
 * calculates the milliseconds, and returns it.
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
