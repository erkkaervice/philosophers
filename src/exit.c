/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/28 14:39:14 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * ft_stoplock - Checks if the simulation should stop based on the stop flag.
 *
 * This function locks the simulation stop mutex and checks whether the 
 * `sim_stop` flag is set to indicate the simulation should stop. If it is 
 * set, the function returns 1 to terminate the philosopher's routine.
 *
 * Parameters:
 * - philo: A pointer to the philosopher structure containing the relevant 
 *          data for the philosopher.
 *
 * Returns:
 * - 1 if the simulation should stop.
 * - 0 if the simulation can continue.
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
 * ft_reaper - Checks if a philosopher has died and stops the simulation.
 *
 * This function checks if a philosopher has surpassed the allowed time to 
 * live by comparing the current time with the philosopher's last meal time. 
 * If the philosopher has died, it sets the `sim_stop` flag to 1, indicating 
 * the simulation should stop, and prints the death message.
 *
 * Parameters:
 * - data: The simulation data structure, which contains the time control 
 *         and simulation status.
 * - philo: A pointer to the philosopher structure to check for death.
 *
 * Returns:
 * - 1 if the philosopher has died and the simulation should stop.
 * - 0 if the philosopher is still alive.
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
		ft_printf("%d %d died\n",
			(int)(current_time - data->start_time), philo->id);
		pthread_mutex_unlock(&data->write_lock);
	}
	return (1);
}

/*
 * ft_maxmeal - Checks if all philosophers have eaten the required number 
 * of meals to stop the simulation.
 *
 * This function checks if every philosopher has eaten at least the number 
 * of meals specified by `must_eat`. If this condition is met for all 
 * philosophers, the simulation stop flag (`sim_stop`) is set to 1 to stop 
 * the simulation.
 *
 * Parameters:
 * - data: The simulation data structure, which contains the `must_eat` 
 *         condition and other control information.
 * - philos: An array of philosopher structures containing the number of 
 *           meals each philosopher has eaten.
 *
 * Returns:
 * - 1 if all philosophers have eaten the required number of meals.
 * - 0 if any philosopher still needs to eat more meals.
 */
int	ft_maxmeal(t_data *data, t_philo *philos)
{
	int	i;
	int	eaten;

	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_lock(&data->sim_stop_lock);
		eaten = philos[i].meals_eaten;
		pthread_mutex_unlock(&data->sim_stop_lock);
		if (eaten < data->must_eat)
			return (0);
		i++;
	}
	pthread_mutex_lock(&data->sim_stop_lock);
	data->sim_stop = 1;
	pthread_mutex_unlock(&data->sim_stop_lock);
	return (1);
}

/*
 * ft_status - Checks the status of the simulation to determine if it should 
 * continue running.
 *
 * This function checks if any philosopher has died by calling the `ft_reaper` 
 * function for each philosopher. It also checks whether the simulation should 
 * stop due to the `must_eat` condition, using the `ft_maxmeal` function if 
 * necessary. If either condition is met, the function will return 1 to stop 
 * the simulation. 
 *
 * Parameters:
 * - data: The simulation data structure, which contains the `sim_stop` flag 
 *         and other control information.
 * - philos: An array of philosopher structures to check their current status.
 *
 * Returns:
 * - 1 if the simulation should stop (a philosopher has died or all have eaten 
 *   the required meals).
 * - 0 if the simulation should continue.
 */
int	ft_status(t_data *data, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		if (ft_reaper(data, &philos[i]))
			return (1);
		i++;
	}
	if (data->must_eat > 0)
	{
		return (ft_maxmeal(data, philos));
	}
	return (0);
}

/*
 * ft_cleanup - Frees memory and destroys mutexes after the simulation ends.
 *
 * This function ensures that all dynamically allocated memory is freed, and
 * all mutexes used throughout the simulation are properly destroyed. This
 * includes cleaning up philosopher threads, mutexes used for forks, and
 * shared locks used for simulation control.
 *
 * Parameters:
 * - data: A pointer to the data structure containing simulation details.
 * - philos: A pointer to the array of philosopher structures.
 */

void	ft_cleanup(t_data *data, t_philo *philos)
{
	int	i;

	if (philos && data->num_philos > 1)
	{
		i = 0;
		while (i < data->num_philos)
		{
			pthread_mutex_lock(&philos[i].data->sim_stop_lock);
			while (!philos[i].thread_done)
				pthread_cond_wait(&philos[i].done_cond,
					&philos[i].data->sim_stop_lock);
			pthread_mutex_unlock(&philos[i].data->sim_stop_lock);
			pthread_mutex_destroy(&data->forks[i]);
			pthread_cond_destroy(&philos[i].done_cond);
			i++;
		}
	}
	if (philos)
		free(philos);
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
	pthread_mutex_destroy(&data->last_meal_lock);
	free(data->forks);
	free(data);
}
