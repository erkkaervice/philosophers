/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/24 13:29:45 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
 * main - The entry point of the simulation. Initializes data and philosophers,
 *        starts threads, and manages the simulation loop.
 *
 * The function handles validation of command-line arguments, initialization
 * of simulation data, and creation of philosopher threads. It starts the
 * simulation loop, which monitors the shared stop condition to determine
 * whether the simulation should continue running.
 *
 * In the special case of only one philosopher, it exits early after cleanup
 * since the philosopher would starve without a second fork.
 *
 * Once the stop condition is triggered (either a philosopher dies or all
 * have eaten the required number of times), it performs cleanup and exits.
 *
 * Parameters:
 * - ac: The argument count (should be 5 or 6).
 * - av: The argument values (parameters for simulation).
 *
 * Returns:
 * - 0 if the simulation completes successfully.
 * - 1 if the arguments are invalid or an error occurs during initialization.
 */
int	main(int ac, char **av)
{
	t_data		*data;
	t_philo		*philos;

	if (ac != 5 && ac != 6)
		return (ft_printf("Use: ./philo <# phs> <die> <eat> <slp> [max]\n"), 1);
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	philos = data->philos;
	ft_threads(data, philos);
	if (data->num_philos == 1)
		return (ft_cleanup(data, philos), 0);
	while (!ft_stoplock(philos))
	{
		if (data->must_eat > 0)
			ft_maxmeal(data, philos);
		usleep(100);
	}
	ft_cleanup(data, philos);
	return (0);
}
