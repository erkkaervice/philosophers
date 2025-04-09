/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/09 11:54:45 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	ft_threads(t_data *data, t_philo *philos)
{
	int	i;

	if (data->num_philos == 1)
		return ;
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&philos[i].thread, NULL, ft_routine, &philos[i]) != 0)
			return ;
		philos[i].thread_done = 0;
		i++;
	}
	while (!ft_status(data, philos))
		ft_usleep(100);
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
	while (1)
	{
		if (ft_stoplock(philo))
			break ;
		ft_eat(philo);
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
		i++;
	}
	return (0);
}

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
