/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/04 14:15:37 by eala-lah         ###   ########.fr       */
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

void cleanup(t_data *data, t_philo *philos)
{
    int i;

    if (philos)
    {
        i = 0;
        while (i < data->num_philos)
        {
            if (data->num_philos > 1)
            {
                pthread_mutex_lock(&philos[i].data->sim_stop_lock);
                while (!philos[i].thread_done)
                    pthread_cond_wait(&philos[i].done_cond, &philos[i].data->sim_stop_lock);
                pthread_mutex_unlock(&philos[i].data->sim_stop_lock);
            }
            i++;
        }
        i = 0;
        while (i < data->num_philos)
        {
            if (data->num_philos > 1)
                pthread_mutex_destroy(&data->forks[i]);
            i++;
        }
        free(philos);
    }
    pthread_mutex_destroy(&data->write_lock);
    pthread_mutex_destroy(&data->sim_stop_lock);
    free(data->forks);
    free(data);
}

int main(int ac, char **av)
{
    t_data    *data;
    t_philo    *philos;

    // Argument validation
    if (ac != 5 && ac != 6)
    {
        ft_printf("Usage: ./philo <num_philos> <time_to_die> <time_to_eat> "
                  "<time_to_sleep> [must_eat]\n");
        return (1);
    }

    // Initialize data structure
    data = init_data(ac, av);
    if (!data)
    {
        ft_printf("Error: Initialization failed.\n");
        return (1);
    }

    // Check for valid time values
    if (data->time_to_die <= 0 || data->time_to_eat <= 0 || data->time_to_sleep <= 0)
    {
        ft_printf("Error: Time values must be positive.\n");
        free(data);
        return (1);
    }

    // Handle special case where there is only one philosopher
    if (data->num_philos == 1)
    {
        ft_printf("Simulation stopped: Only one philosopher, no interaction possible.\n");
        cleanup(data, NULL);
        return (0);
    }

    philos = data->philos;

    // Start philosopher threads
    start_threads(data, philos);

    // Main loop to check for simulation stop condition (a philosopher's death)
    while (1)
    {
        pthread_mutex_lock(&data->sim_stop_lock);

        // If simulation has been stopped due to death, exit the loop and program
        if (data->sim_stop)
        {
            pthread_mutex_unlock(&data->sim_stop_lock);
            ft_printf("Simulation has ended due to a philosopher's death.\n");
            cleanup(data, philos);  // Clean up and free resources
            exit(0);  // Exit the entire program immediately
        }

        pthread_mutex_unlock(&data->sim_stop_lock);
        usleep(100);  // Sleep briefly to reduce CPU usage
    }

    return (0);  // This line should never be reached because exit(0) is called
}
