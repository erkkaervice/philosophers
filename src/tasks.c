/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tasks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/08 15:49:21 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/09 11:45:46 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_log(t_data *data, int id, char *msg)
{
	int	time;

	pthread_mutex_lock(&data->write_lock);
	time = (int)(get_time() - data->start_time);
	ft_printf("%d %d %s\n", time, id, msg);
	pthread_mutex_unlock(&data->write_lock);
}

void	ft_usleep(long long ms)
{
	long long	start_time;

	start_time = get_time();
	while (get_time() - start_time < ms)
		usleep(100);
}

int	take_forks(t_philo *philo)
{
	pthread_mutex_lock(philo->left_fork);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (philo->data->sim_stop)
	{
		pthread_mutex_unlock(&philo->data->sim_stop_lock);
		pthread_mutex_unlock(philo->left_fork);
		return (0);
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
		return (0);
	}
	print_log(philo->data, philo->id, "has taken a fork");
	print_log(philo->data, philo->id, "is eating");
	philo->last_meal = get_time();
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
	return (1);
}

void	eat(t_philo *philo)
{
	if (check_sim_stopped(philo) || !take_forks(philo))
		return ;
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
	if (check_sim_stopped(philo))
		return ;
	print_log(philo->data, philo->id, "is sleeping");
	ft_usleep(philo->data->time_to_sleep);
	pthread_mutex_lock(&philo->data->sim_stop_lock);
	if (!philo->data->sim_stop)
		print_log(philo->data, philo->id, "is thinking");
	pthread_mutex_unlock(&philo->data->sim_stop_lock);
}
