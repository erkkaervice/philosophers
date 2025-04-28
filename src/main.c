/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/28 14:40:52 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/*
 * ft_solo - Simulates the behavior of a single philosopher.
 *
 * This function simulates the actions of a philosopher when there is only
 * one philosopher in the simulation. It locks and unlocks the philosopher's
 * fork and simulates the philosopher dying after a period of time.
 *
 * Parameters:
 * - philo: A pointer to the philosopher structure.
 */
static void	ft_solo(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	ft_printlog(philo, "has taken a fork");
	usleep(philo->data->time_to_die * 1000);
	ft_printlog(philo, "died");
	pthread_mutex_unlock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->data->sim_stop = 1;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
}

/*
 * ft_wait - Waits for all philosopher threads to finish.
 *
 * This function continuously checks the status of the simulation and stops
 * the threads when necessary. It waits for all philosopher threads to finish
 * by calling pthread_join on each one.
 *
 * Parameters:
 * - data: The simulation data.
 * - philos: The array of philosophers.
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
		usleep(500);
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
 * ft_routine - The main routine for each philosopher thread.
 *
 * This function defines the behavior of each philosopher during the simulation.
 * The philosopher repeatedly eats and sleeps, while checking whether the
 * simulation should stop by calling ft_stoplock. Once the philosopher's
 * routine finishes, it signals that the thread is done.
 *
 * Parameters:
 * - arg: A pointer to the philosopher's data structure.
 *
 * Returns:
 * - NULL when the routine completes.
 */
static void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep(2000);
	while (!ft_stoplock(philo))
	{
		ft_eat(philo);
		if (philo->data->must_eat > 0)
		{
			pthread_mutex_lock(&philo->data->sim_stop_lock);
			if (philo->meals_eaten >= philo->data->must_eat)
			{
				pthread_mutex_unlock(&philo->data->sim_stop_lock);
				break ;
			}
			pthread_mutex_unlock(&philo->data->sim_stop_lock);
		}
		ft_sleepthink(philo);
	}
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->thread_done = 1;
	pthread_cond_signal(&philo->done_cond);
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (NULL);
}

/*
 * ft_threads - Creates and manages philosopher threads for the simulation.
 *
 * This function initializes philosopher threads and starts the simulation.
 * It creates one thread for each philosopher, passing the routine function
 * (`ft_routine`) as the entry point. If thread creation fails partway,
 * it joins already-created threads to prevent leaks. After all threads are
 * created, the function waits for them to finish by calling ft_wait().
 *
 * For the special case of a single philosopher, it handles the routine
 * separately and signals completion safely under lock.
 *
 * Parameters:
 * - data: The simulation data containing the number of philosophers and
 *         other shared information.
 * - philos: An array of philosopher structures, each containing thread
 *           and synchronization data.
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
