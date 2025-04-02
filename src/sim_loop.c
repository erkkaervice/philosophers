/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sim_loop.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:54 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 14:38:32 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Philosopher routine: Eat, sleep, and think */
void	*routine(void *arg)
{
	t_philo	*philo;

	philo = (t_philo *)arg;
	while (1)
	{
		eat(philo);         // Philosopher eats
		sleep_think(philo); // Philosopher sleeps or thinks

		// Check the simulation status
		if (check_simulation_status(philo->data, philo->data->philos))  // Access philos array through data
		{
			print_log(philo->data, philo->id, "stopped simulation"); // Log when simulation stops
			break;  // Stop the simulation if needed
		}
	}
	return (NULL);
}

void start_threads(t_data *data, t_philo *philos)
{
    int i;

    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_create(&philos[i].thread, NULL, routine, &philos[i]) != 0)
        {
            printf("Error creating thread for philosopher %d\n", philos[i].id);
            return; // Exit if thread creation fails
        }
        i++;
    }

    // Wait for all threads to finish (join threads)
    i = 0;
    while (i < data->num_philos)
    {
        // Ensure we only join threads that were successfully created
        if (pthread_join(philos[i].thread, NULL) != 0)
        {
            printf("Error joining thread for philosopher %d\n", philos[i].id);
            return; // Exit if thread joining fails
        }
        i++;
    }
}
