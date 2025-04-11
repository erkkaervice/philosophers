/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/11 13:48:34 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * ft_threads - Creates and manages philosopher threads for the simulation.
 *
 * This function initializes philosopher threads and starts the simulation. 
 * It creates one thread for each philosopher, passing the routine function 
 * (`ft_routine`) as the entry point. After all threads are created, the 
 * function waits for the threads to finish by calling `pthread_join` on each.
 * 
 * The function also manages the initial start time and stops the simulation 
 * if necessary, based on the `ft_status` check.
 *
 * Parameters:
 * - data: The simulation data containing the number of philosophers and other 
 *         shared information.
 * - philos: An array of philosopher structures that hold information about 
 *           each philosopher, including their thread and routine data.
 */
void	ft_threads(t_data *data, t_philo *philos)
{
	int			i;
	int			ret;

	if (data->num_philos == 1)
	{
		ft_single_philo(&philos[0]);
		return ;
	}
	i = 0;
	while (i < data->num_philos)
	{
		ret = pthread_create(&philos[i].thread, NULL, ft_routine, &philos[i]);
		if (ret != 0)
			return ;
		philos[i].thread_done = 0;
		i++;
	}
	while (!ft_status(data, philos))
		usleep(100);
	i = 0;
	while (i < data->num_philos)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
}

/*
 * ft_routine - The main routine for each philosopher thread.
 *
 * This function defines the behavior of each philosopher during the simulation. 
 * The philosopher repeatedly eats and sleeps, while checking whether the 
 * simulation should stop by calling `ft_stoplock`. Once the philosopher's 
 * routine finishes, it signals that the thread is done.
 *
 * Parameters:
 * - arg: A pointer to the philosopher's data structure.
 *
 * Returns:
 * - NULL when the routine completes.
 */
void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		if (ft_stoplock(philo))
			break ;
		ft_eat(philo);
		if (philo->data->must_eat
			&& philo->meals_eaten >= philo->data->must_eat)
			break ;
		ft_sleepthink(philo);
	}
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->thread_done = 1;
	pthread_cond_signal(&philo->done_cond);
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (NULL);
}

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
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		return (1);
	}
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (0);
}

/*
 * ft_status - Checks the status of the simulation to determine if it should 
 * continue running.
 *
 * This function locks the simulation stop mutex and checks whether the 
 * `sim_stop` flag is set. If it is set, the function returns 1 to indicate 
 * that the simulation has stopped. It also checks the individual philosophers 
 * to see if any have died, using the `ft_reaper` function.
 *
 * Parameters:
 * - data: The simulation data structure, which contains the `sim_stop` flag 
 *         and other control information.
 * - philos: An array of philosopher structures to check their current status.
 *
 * Returns:
 * - 1 if the simulation should stop.
 * - 0 if the simulation should continue.
 */
int	ft_status(t_data *data, t_philo *philos)
{
	int	i;

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
		if (ft_reaper(data, &philos[i]))
			return (1);
		if (data->must_eat > 0 && philos[i].meals_eaten < data->must_eat)
			return (0);
		i++;
	}
	return (0);
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

	pthread_mutex_lock(&data->sim_stop_lock);
	last_meal = philo->last_meal;
	pthread_mutex_unlock(&data->sim_stop_lock);
	current_time = ft_time();
	if (current_time - last_meal >= data->time_to_die)
	{
		pthread_mutex_lock(&data->sim_stop_lock);
		data->sim_stop = 1;
		pthread_mutex_unlock(&data->sim_stop_lock);
		pthread_mutex_lock(&data->write_lock);
		ft_printf("%d %d has died\n",
			(int)(current_time - data->start_time), philo->id);
		pthread_mutex_unlock(&data->write_lock);
		return (1);
	}
	return (0);
}
