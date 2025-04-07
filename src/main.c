/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/07 15:29:54 by eala-lah         ###   ########.fr       */
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

long long	get_time(void)
{
	struct timeval	tv;
	long long		ms;

	gettimeofday(&tv, NULL);
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
	return (ms);
}

void	ft_usleep(long long ms)
{
	long long	start_time;

	start_time = get_time();
	while (get_time() - start_time < ms)
		usleep(100);
}

void	cleanup(t_data *data, t_philo *philos)
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
			}
			i++;
		}
		free(philos);
	}
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
	free(data->forks);
	free(data);
}

int	main(int ac, char **av)
{
	t_data	*data;
	t_philo	*philos;

	if (ac != 5)
	{
		ft_printf("Usage: ./philo <# of philos> <die> <eat> <sleep>\n");
		return (1);
	}
	data = init_data(ac, av);
	if (!data)
		return (1);
	philos = data->philos;
	start_threads(data, philos);
	while (1)
	{
		pthread_mutex_lock(&data->sim_stop_lock);
		if (data->sim_stop)
		{
			pthread_mutex_unlock(&data->sim_stop_lock);
			cleanup(data, philos);
			return (0);
		}
		pthread_mutex_unlock(&data->sim_stop_lock);
	}
	return (0);
}
