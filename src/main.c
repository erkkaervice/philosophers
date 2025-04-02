/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 15:17:20 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

void	print_log(t_data *data, int id, char *msg)
{
	long long	time;

	pthread_mutex_lock(&data->write_lock);
	time = get_time() - data->start_time;
	printf("%lld %d %s\n", time, id, msg);
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
	int i;

	i = 0;
	while (i < data->num_philos)
	{
		if (pthread_join(philos[i].thread, NULL) != 0)
		{
			printf("Error joining thread for philosopher %d\n", philos[i].id);
		}
		i++;
	}
	pthread_mutex_destroy(&data->write_lock);
	pthread_mutex_destroy(&data->sim_stop_lock);
	i = 0;
	while (i < data->num_philos)
	{
		pthread_mutex_destroy(&data->forks[i]);
		i++;
	}
	free(data->forks);
	free(philos);
	free(data);
}

int	main(int argc, char **argv)
{
	t_data	*data;
	t_philo	*philos;

	if (argc != 5 && argc != 6)
	{
		printf("Usage: ./philo <num_philos> <time_to_die> <time_to_eat> <time_to_sleep> [must_eat]\n");
		return (1);
	}
	data = init_data(argc, argv);
	if (!data)
	{
		printf("Error: Initialization failed.\n");
		return (1);
	}
	philos = data->philos;
	start_threads(data, philos);
	while (!check_simulation_status(data, philos))
	{
		usleep(100);
	}
	cleanup(data, philos);
	return (0);
}
