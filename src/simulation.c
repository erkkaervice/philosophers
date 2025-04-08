/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/08 11:58:01 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	start_threads(t_data *data, t_philo *philos)
{
	int	i;

	if (data->num_philos == 1)
		return ;
	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
			return ;
		philos[i].thread_done = 0;
		i++;
	}
	while (!check_simulation_status(data, philos))
		ft_usleep(100);
	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_lock(&philos[i].data->sim_stop_lock);
		while (!philos[i].thread_done)
			pthread_cond_wait(&philos[i].done_cond,
				&philos[i].data->sim_stop_lock);
		pthread_mutex_unlock(&philos[i].data->sim_stop_lock);
		pthread_join(philos[i].thread, NULL);
		i++;
	}
}

void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		pthread_mutex_lock(&philo->data->sim_stop_lock);
		if (philo->data->sim_stop)
		{
			pthread_mutex_unlock(&philo->data->sim_stop_lock);
			break ;
		}
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		eat(philo);
		sleep_think(philo);
	}
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	philo->thread_done = 1;
	pthread_cond_signal(&philo->done_cond);
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (NULL);
}

int	check_simulation_status(t_data *data, t_philo *philos)
{
	int			i;
	long long	current_time;
	long long	last_meal;

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
		pthread_mutex_lock(&data->sim_stop_lock);
		last_meal = philos[i].last_meal;
		pthread_mutex_unlock(&data->sim_stop_lock);
		if ((current_time = get_time()) - last_meal >= data->time_to_die)
		{
			pthread_mutex_lock(&data->sim_stop_lock);
			data->sim_stop = 1;
			pthread_mutex_unlock(&data->sim_stop_lock);
			pthread_mutex_lock(&data->write_lock);
			ft_printf("%d %d has died\n", (int)(current_time - data->start_time), philos[i].id);
			pthread_mutex_unlock(&data->write_lock);
			return (1);
		}
		i++;
	}
	return (0);
}

void	eat(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		return ;
	}
	pthread_mutex_unlock(&philo->data->sim_stop_lock);

	pthread_mutex_lock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	print_log(philo->data, philo->id, "has taken a fork");
	pthread_mutex_unlock(&philo->data->sim_stop_lock);

	pthread_mutex_lock(philo->right_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		return ;
	}
	print_log(philo->data, philo->id, "has taken a fork");
	print_log(philo->data, philo->id, "is eating");
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->data->sim_stop_lock);

	ft_usleep(philo->data->time_to_eat);

	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (!philo->data->sim_stop)
		philo->meals_eaten++;
	pthread_mutex_unlock(&philo->data->sim_stop_lock);

	pthread_mutex_unlock(philo->right_fork);
	pthread_mutex_unlock(philo->left_fork);
}


void	sleep_think(t_philo *philo)
{
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		return ;
	}
	print_log(philo->data, philo->id, "is sleeping");
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	ft_usleep(philo->data->time_to_sleep);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (!philo->data->sim_stop)
		print_log(philo->data, philo->id, "is thinking");
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
}
