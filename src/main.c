/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/09 13:47:50 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_usleep(long long duration_ms)
{
	long long	start;

	start = ft_time();
	while (ft_time() - start < duration_ms)
		usleep(50);
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
	ft_usleep(philo->data->time_to_die);
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

/*
 * Entry point of the simulation.
 *
 * Initializes the simulation, creates philosopher threads, and waits 
 * for their completion. Handles the case of a single philosopher separately.
 */
int	main(int ac, char **av)
{
	t_data	*data;
	t_philo	*philos;

	if (ac != 5 && ac != 6)
		return (ft_printf("Use: ./philo <# phs> <die> <eat> <slp> [max]\n"), 1);
	if (ft_atoi(av[1]) <= 0 || ft_atoi(av[2]) <= 0
		|| ft_atoi(av[3]) <= 0 || ft_atoi(av[4]) <= 0
		|| (ac == 6 && ft_atoi(av[5]) <= 0))
		return (ft_printf("These are not the args you were looking for\n"), 1);
	data = ft_initdata(ac, av);
	if (!data)
		return (1);
	philos = data->philos;
	ft_threads(data, philos);
	if (data->num_philos == 1)
		return (ft_cleanup(data, philos), 0);
	ft_cleanup(data, philos);
	return (0);
}
