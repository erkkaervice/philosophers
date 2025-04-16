/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/16 17:07:24 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_threads(t_data *data, t_philo *philos)
{
	int	i;

	if (data->num_philos == 1)
	{
		ft_single_philo(&philos[0]);
		return ;
	}
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&philos[i].thread,
				NULL, ft_routine, &philos[i]) != 0)
		{
			ft_printf("Error creating thread for philo %d\n", i);
			return ;
		}
		i++;
	}
	ft_wait_for_threads(data, philos);
}

void	ft_wait_for_threads(t_data *data, t_philo *philos)
{
	int	i;

	while (!data->sim_stop)
	{
		if (ft_status(data, philos))
			break ;
		usleep(100);
	}
	i = 0;
	while (i < data->num_philos)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
}

void	*ft_routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	if (philo->id % 2 == 0)
		usleep(100);
	while (1)
	{
		if (ft_stoplock(philo))
			break ;
		usleep(50);
		ft_eat(philo);
		if (philo->data->must_eat > 0
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

int	ft_status(t_data *data, t_philo *philos)
{
	int	i;

	if (ft_stoplock(philos))
		return (1);
	i = 0;
	while (i < data->num_philos)
	{
		if (ft_reaper(data, &philos[i]))
			return (1);
		i++;
	}
	if (data->must_eat > 0)
	{
		return (ft_check_meals(data, philos));
	}
	return (0);
}

int	ft_check_meals(t_data *data, t_philo *philos)
{
	int	i;

	i = 0;
	while (i < data->num_philos)
	{
		if (philos[i].meals_eaten < data->must_eat)
			return (0);
		i++;
	}
	pthread_mutex_lock(&data->sim_stop_lock);
	data->sim_stop = 1;
	pthread_mutex_unlock(&data->sim_stop_lock);
	return (1);
}

int	ft_reaper(t_data *data, t_philo *philo)
{
	long long	current_time;
	long long	last_meal;

	if (ft_stoplock(philo))
		return (1);
	last_meal = philo->last_meal;
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
