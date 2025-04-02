/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 13:40:19 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Philosopher eats */
void	eat(t_philo *philo)
{
	t_fork	*first;
	t_fork	*second;

	// Always lock the left fork first and the right fork second
	first = philo->left_fork;
	second = philo->right_fork;
	pthread_mutex_lock(first);
	pthread_mutex_lock(second);
	print_log(philo->data, philo->id, "is eating");
	ft_usleep(philo->data->time_to_eat);
	philo->last_meal = get_time();
	philo->meals_eaten++;
	pthread_mutex_unlock(second);
	pthread_mutex_unlock(first);
}


/* Philosopher sleeps or thinks */
void	sleep_think(t_philo *philo)
{
	print_log(philo->data, philo->id, "is sleeping");
	ft_usleep(philo->data->time_to_sleep);
	print_log(philo->data, philo->id, "is thinking");
}
