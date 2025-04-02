/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitoring.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:27 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 14:10:54 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

int check_simulation_status(t_data *data, t_philo *philos)
{
    int i;

    // Check if data or philos is NULL
    if (!data || !philos)
    {
        printf("Error: Invalid data or philosopher pointers\n");
        return (1);  // Return immediately if any pointer is invalid
    }

    i = 0;
    pthread_mutex_lock(&data->sim_stop_lock);

    // If simulation is already stopped, return immediately
    if (data->sim_stop)
    {
        pthread_mutex_unlock(&data->sim_stop_lock);
        return (1);  // Simulation should stop (someone died or condition met)
    }

    // Check if any philosopher has died
    while (i < data->num_philos)
    {
        // If time since last meal exceeds time_to_die, philosopher dies
        if (get_time() - philos[i].last_meal >= data->time_to_die)
        {
            data->sim_stop = 1;
            pthread_mutex_unlock(&data->sim_stop_lock);
            print_log(data, philos[i].id, "has died");  // Log when a philosopher dies
            return (1);  // Philosopher has died, simulation stops
        }
        i++;
    }

    pthread_mutex_unlock(&data->sim_stop_lock);
    return (0);  // Continue simulation
}
