/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   threads.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/24 13:29:46 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

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
void	ft_wait(t_data *data, t_philo *philos)
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
void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep(1000);
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
 * ft_solo - Simulates the behavior of a single philosopher.
 *
 * This function simulates the actions of a philosopher when there is only
 * one philosopher in the simulation. It locks and unlocks the philosopher's
 * fork and simulates the philosopher dying after a period of time.
 *
 * Parameters:
 * - philo: A pointer to the philosopher structure.
 */
void	ft_solo(t_philo *philo)
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
	int				i;
	pthread_mutex_t	*lock;

	if (philos)
	{
		i = 0;
		while (i < data->num_philos)
		{
			if (data->num_philos > 1)
			{
				lock = &philos[i].data->sim_stop_lock;
				pthread_mutex_lock(lock);
				while (!philos[i].thread_done)
					pthread_cond_wait(&philos[i].done_cond, lock);
				pthread_mutex_unlock(lock);
				pthread_mutex_destroy(&data->forks[i]);
				pthread_cond_destroy(&philos[i].done_cond);
			}
			i++;
		}
		free(philos);
	}
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
	pthread_mutex_destroy(&data->last_meal_lock);
	free(data->forks);
	free(data);
}
