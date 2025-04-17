/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:49:21 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/17 17:31:26 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
  * ft_eat - Simulates the philosopher eating.
  *
  * This function handles the process of eating for the philosopher. It first 
  * checks whether the simulation should stop using `ft_stoplock`. If the 
  * philosopher successfully acquires both forks using `ft_forks`, it then 
  * sleeps for the duration of `time_to_eat`. After eating, it updates the 
  * number of meals eaten and releases the forks.
  *
  * Parameters:
  * - philo: A pointer to the philosopher's data structure, containing fork 
  *          and meal information.
  */
void	ft_eat(t_philo *philo)
{
	long long	start_time;

	if (ft_stoplock(philo) || !ft_forks(philo))
		return ;
	start_time = ft_time();
	while (ft_time() - start_time < philo->data->time_to_eat)
		usleep(100);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (!philo->data->sim_stop && philo->meals_eaten < philo->data->must_eat)
	{
		philo->last_meal = ft_time();
		philo->meals_eaten++;
	}
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}

/*
  * ft_sleepthink - Simulates the philosopher sleeping and thinking.
  *
  * This function handles the sleeping and thinking phases of the philosopher. 
  * After eating, the philosopher sleeps for the duration specified by 
  * `time_to_sleep`. After waking up, the philosopher will think, and this 
  * is logged. The function checks if the simulation should stop by calling 
  * `ft_stoplock` before proceeding with the sleep and think phases.
  *
  * Parameters:
  * - philo: A pointer to the philosopher's data structure, containing time 
  *          and state information.
  */
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

/*
  * ft_forks - Attempts to acquire both forks for a philosopher to eat.
  *
  * This function locks the left and right forks for the philosopher, ensuring 
  * that no other philosopher can access the same forks at the same time. 
  * If the simulation is stopped during the process, it unlocks any acquired 
  * forks and returns 0 to indicate that the philosopher could not proceed.
  *
  * Parameters:
  * - philo: A pointer to the philosopher's data structure, containing 
  *          fork information.
  *
  * Returns:
  * - 1 if the philosopher successfully took both forks and can eat.
  * - 0 if the philosopher could not take both forks and cannot eat.
  */
int	ft_forks(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		pthread_mutex_unlock(philo->left_fork);
		return (0);
	}
	ft_printlog(philo->data, philo->id, "has taken a fork");
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	pthread_mutex_lock(philo->right_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		return (0);
	}
	ft_printlog(philo->data, philo->id, "has taken a fork");
	ft_printlog(philo->data, philo->id, "is eating");
	philo->last_meal = ft_time();
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (1);
}

/*
  * ft_printlog - Prints a log message for a philosopher's actions.
  *
  * This function formats and prints a log message indicating the philosopher's
  * action (e.g., taking a fork, eating, sleeping, thinking) along with the 
  * current time and philosopher's ID. The printing is done in a thread-safe 
  * manner using a mutex lock to ensure proper synchronization between threads.
  *
  * Parameters:
  * - data: The simulation data structure that holds the current start time 
  *         for time calculations.
  * - id: The philosopher's unique ID to include in the log message.
  * - msg: The action/message to log.
  */
void	ft_printlog(t_data *data, int id, char *msg)
{
	int	time;

	pthread_mutex_lock(&data->write_lock);
	time = (int)(ft_time() - data->start_time);
	ft_printf("%d %d %s\n", time, id, msg);
	pthread_mutex_unlock(&data->write_lock);
}

/*
  * ft_time - Returns the current time in milliseconds 
  *           since the simulation start.
  *
  * This function retrieves the current time using `gettimeofday`, 
  * calculates the time in milliseconds, and returns it. The time is 
  * used to track how long the philosophers have been running and to 
  * control their behavior (eating, sleeping, etc.).
  *
  * Returns:
  * - The current time in milliseconds.
  */
long long	ft_time(void)
{
	struct timeval	tv;
	long long		ms;

	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}
