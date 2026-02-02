/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:26:25 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/23 16:02:16 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Simulates a delay in milliseconds by repeatedly calling usleep 
 * to avoid excessive CPU usage.
 *
 * If the requested duration is less than or equal to 5ms, a single 
 * usleep call is used to minimize overhead. Otherwise, a loop calls 
 * usleep for 5ms, 1ms, or 0.1ms intervals until the desired time elapses.
 * This ensures accurate timing without wasting CPU cycles.
 */
void	ft_usleep(t_philo *philo, long long duration_ms)
{
	long long	start;
	long long	elapsed;

	start = ft_time();
	while (1)
	{
		elapsed = ft_time() - start;
		if (elapsed >= duration_ms)
			break ;
		if (ft_stoplock(philo))
			break ;
		if (duration_ms - elapsed > 10)
			usleep(5000);
		else if (duration_ms - elapsed > 2)
			usleep(1000);
		else
			usleep(100);
	}
}

/*
 * Handles the single philosopher case.
 *
 * Locks the only fork, logs pickup, waits until death, then logs death,
 * unlocks the fork, and stops the simulation.
 */
static void	ft_solo(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	ft_printlog(philo, "has taken a fork");
	ft_usleep(philo, philo->data->time_to_die);
	ft_printlog(philo, "died");
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->data->sim_stop = 1;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
}

/*
 * Waits for all philosopher threads to finish and monitors simulation.
 *
 * Continuously checks if the simulation should stop due to death or 
 * completion. When triggered, sets the stop flag and joins all threads.
 */
static void	ft_wait(t_data *data, t_philo *philos)
{
	int	i;

	while (!ft_stoplock(&philos[0]))
	{
		if (ft_status(data, philos))
		{
			pthread_mutex_lock(&data->sim_stop_lock);
			data->sim_stop = 1;
			pthread_mutex_unlock(&data->sim_stop_lock);
			break ;
		}
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (philos[i].thread)
			pthread_join(philos[i].thread, NULL);
		i++;
	}
}

/*
 * Main routine executed by each philosopher thread.
 *
 * Even ID philosophers start by sleeping and thinking to stagger actions.
 * The last philosopher starts thinking immediately.
 * Then loops: eat, check stop, sleep and think, until stop condition.
 */
static void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = arg;
	if (philo->id % 2 == 0 && !ft_stoplock(philo))
		ft_sleepthink(philo);
	else if (philo->id == philo->data->num_philos && !ft_stoplock(philo))
		ft_printlog(philo, "is thinking");
	while (!ft_stoplock(philo))
	{
		ft_eat(philo);
		if (ft_stoplock(philo))
			break ;
		ft_sleepthink(philo);
	}
	return (NULL);
}

/*
 * Starts philosopher threads and manages simulation lifecycle.
 *
 * For a single philosopher, handles the solo case.
 * Otherwise, sets start time and creates threads, initializing last_meal.
 * On thread creation failure, stops simulation and joins created threads.
 * Finally, waits for all threads to finish.
 */
void	ft_threads(t_data *data, t_philo *philos)
{
	int	i;

	if (data->num_philos == 1)
		return (ft_solo(&philos[0]));
	data->start_time = ft_time();
	i = 0;
	while (i < data->num_philos)
	{
		philos[i].last_meal = data->start_time;
		if (pthread_create(&philos[i].thread, NULL,
				ft_routine, &philos[i]) != 0)
		{
			printf("Error creating thread for philo %d\n", i);
			pthread_mutex_lock(&data->sim_stop_lock);
			data->sim_stop = 1;
			pthread_mutex_unlock(&data->sim_stop_lock);
			while (i-- > 0)
				pthread_join(philos[i].thread, NULL);
			return ;
		}
		i++;
	}
	ft_wait(data, philos);
}
