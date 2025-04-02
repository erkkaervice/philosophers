/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:49 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 14:48:04 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

// Function to cleanup resources
void cleanup(t_data *data, t_philo *philos)
{
    int i;

    // Join threads
    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_join(philos[i].thread, NULL) != 0)
        {
            printf("Error joining thread for philosopher %d\n", philos[i].id);
            // Handle error gracefully, continue joining others
        }
        i++;
    }

    // Destroy mutexes
    pthread_mutex_destroy(&data->write_lock);
    pthread_mutex_destroy(&data->sim_stop_lock);

    // Destroy forks mutexes
    i = 0;
    while (i < data->num_philos)
    {
        pthread_mutex_destroy(&data->forks[i]);
        i++;
    }

    // Free allocated memory
    free(data->forks);
    free(philos);
    free(data);
}

int main(int argc, char **argv)
{
    t_data  *data;
    t_philo *philos;

    // Check the number of arguments
    if (argc != 5 && argc != 6)
    {
        printf("Usage: ./philo <num_philos> <time_to_die> <time_to_eat> <time_to_sleep> [must_eat]\n");
        return (1);
    }

    // Initialize the philosopher data and structures
    data = init_data(argc, argv);
    if (!data)
    {
        printf("Error: Initialization failed.\n");
        return (1);
    }

    // Allocate memory for philosophers
    philos = data->philos;  // We now use the philosophers already allocated in init_data

    // Start philosopher threads
    start_threads(data, philos);

    // Monitor the simulation and stop when needed
    while (!check_simulation_status(data, philos)) // Continually check simulation status
    {
        usleep(100);  // A small delay to prevent 100% CPU usage, adjust as needed
    }

    // Clean up resources after the simulation
    cleanup(data, philos);

    return (0);
}
