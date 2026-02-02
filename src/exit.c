/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/23 16:02:16 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * Checks if simulation stop flag is set.
 *
 * Locks sim_stop mutex and returns the stop flag value. Used to 
 * know if the simulation should terminate.
 */
int	ft_stoplock(t_philo *philo)
{
	int	stop;

	pthread_mutex_lock(&philo->data->sim_stop_lock);
	stop = philo->data->sim_stop;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (stop);
}

/*
 * Checks if a philosopher has died and stops simulation.
 *
 * Compares current time with last meal time. If time_to_die exceeded, 
 * sets sim_stop and prints death message with mutex protection.
 */
int	ft_reaper(t_data *data, t_philo *philo)
{
	long long	current_time;
	long long	last_meal;

	if (ft_stoplock(philo))
		return (1);
	pthread_mutex_lock(&data->last_meal_lock);
	last_meal = philo->last_meal;
	pthread_mutex_unlock(&data->last_meal_lock);
	current_time = ft_time();
	if (current_time - last_meal < data->time_to_die)
		return (0);
	pthread_mutex_lock(&data->sim_stop_lock);
	if (data->sim_stop)
		pthread_mutex_unlock(&data->sim_stop_lock);
	else
	{
		data->sim_stop = 1;
		pthread_mutex_unlock(&data->sim_stop_lock);
		pthread_mutex_lock(&data->write_lock);
		printf("%d %d died\n",
			(int)(current_time - data->start_time), philo->id);
		pthread_mutex_unlock(&data->write_lock);
	}
	return (1);
}

/*
 * Checks if all philosophers have eaten required meals.
 *
 * Locks last_meal mutex and checks each philosopher's meal count. 
 * If all have reached must_eat, sets sim_stop to end simulation.
 */
int	ft_maxmeal(t_data *data, t_philo *philos)
{
	int		i;

	if (ft_stoplock(&philos[0]))
		return (1);
	i = 0;
	pthread_mutex_lock(&data->last_meal_lock);
	while (i < data->num_philos)
	{
		if (philos[i].meals_eaten < data->must_eat)
		{
			pthread_mutex_unlock(&data->last_meal_lock);
			return (0);
		}
		i++;
	}
	pthread_mutex_unlock(&data->last_meal_lock);
	pthread_mutex_lock(&data->sim_stop_lock);
	data->sim_stop = 1;
	pthread_mutex_unlock(&data->sim_stop_lock);
	return (1);
}

/*
 * Checks simulation status for death or meal completion.
 *
 * Iterates all philosophers to check death with ft_reaper. Also 
 * checks must_eat condition if set. Returns 1 if simulation should stop.
 */
int	ft_status(t_data *data, t_philo *philos)
{
	int	i;

	if (ft_stoplock(&philos[0]))
		return (1);
	i = 0;
	while (i < data->num_philos)
	{
		if (ft_reaper(data, &philos[i]))
			return (1);
		i++;
	}
	if (data->must_eat > 0 && ft_maxmeal(data, philos))
		return (1);
	return (0);
}

/*
 * Frees memory and destroys all mutexes after simulation.
 *
 * Destroys forks mutexes, frees philosopher array, destroys other 
 * mutexes and frees data structures to clean up all resources.
 */
void	ft_cleanup(t_data *data, t_philo *philos)
{
	int	i;

	if (philos && data->num_philos > 1)
	{
		i = 0;
		while (i < data->num_philos)
			pthread_mutex_destroy(&data->forks[i++]);
	}
	if (philos)
		free(philos);
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
	pthread_mutex_destroy(&data->last_meal_lock);
	free(data->forks);
	free(data);
}
