/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/13 14:26:25 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/13 16:40:11 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Simulates a delay in milliseconds by repeatedly calling `usleep` 
 * to avoid excessive CPU usage.
 *
 * If the requested duration is less than or equal to 5ms, a single 
 * `usleep` call is used to minimize overhead. Otherwise, a loop is used 
 * to repeatedly call `usleep` for 500ms until the desired duration is reached.
 * This approach ensures more accurate timing without consuming too much CPU.
 */
void	ft_usleep(t_philo *philo, long long duration_ms)
{
	long long	start;

	start = ft_time();
	if (duration_ms <= 5)
	{
		usleep(duration_ms * 1000);
		return ;
	}
	while (ft_time() - start < duration_ms)
	{
		if (ft_stoplock(philo))
			break ;
		usleep(500);
	}
}

/*
 * Simulates the behavior of a single philosopher.
 *
 * If there is only one philosopher, the simulation locks the fork, 
 * waits for the philosopher to die, and then signals the stop condition.
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
 * Waits for all philosopher threads to finish.
 *
 * Monitors the simulation status and waits for all philosopher threads 
 * to complete by joining each one.
 */
static void	ft_wait(t_data *data, t_philo *philos)
{
	int	i;

	while (!ft_stoplock(&philos[0]))
	{
		if (ft_status(data, philos)
			|| (data->must_eat > 0 && ft_maxmeal(data, philos)))
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
 * Main routine for each philosopher thread.
 *
 * Each philosopher repeatedly eats and sleeps while checking the stop 
 * condition. Once the philosopher's routine finishes, it signals that 
 * the thread is done.
 */
static void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = arg;
	if (philo->id % 2 == 0)
		ft_usleep(philo, philo->data->time_to_eat / 2);
	while (!ft_stoplock(philo))
	{
		ft_eat(philo);
		pthread_mutex_lock(&philo->data->sim_stop_lock);
		if (philo->data->must_eat > 0
			&& philo->meals_eaten >= philo->data->must_eat)
		{
			pthread_mutex_unlock(&philo->data->sim_stop_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		ft_sleepthink(philo);
	}
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->thread_done = 1;
	pthread_cond_signal(&philo->done_cond);
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (NULL);
}

/*
 * Creates and manages philosopher threads for the simulation.
 *
 * Initializes and starts philosopher threads. Handles the special case 
 * of a single philosopher separately.
 */
void	ft_threads(t_data *data, t_philo *philos)
{
	int	i;

	if (data->num_philos == 1)
	{
		ft_solo(&philos[0]);
		pthread_mutex_lock(&philos[0].data->sim_stop_lock);
		philos[0].thread_done = 1;
		pthread_cond_signal(&philos[0].done_cond);
		pthread_mutex_unlock(&philos[0].data->sim_stop_lock);
		return ;
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&philos[i].thread, NULL,
				ft_routine, &philos[i]) != 0)
		{
			ft_printf("Error creating thread for philo %d\n", i);
			while (i-- > 0)
				pthread_join(philos[i].thread, NULL);
			return ;
		}
		i++;
	}
	ft_wait(data, philos);
}
