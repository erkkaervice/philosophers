/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:54 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 13:45:40 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Get the current timestamp in milliseconds */
long long	get_time(void)
{
	struct timeval	tv;
	long long		ms;

	gettimeofday(&tv, NULL); // Get the current time
	ms = (tv.tv_sec * 1000) + (tv.tv_usec / 1000); // Convert to milliseconds
	return (ms);
}

/* Sleep for a specific number of milliseconds */
void	ft_usleep(long long ms)
{
	long long	start_time;

	start_time = get_time(); // Get the start time in milliseconds
	while (get_time() - start_time < ms) // Sleep until the specified time has passed
		usleep(100); // Sleep in 100-microsecond intervals to allow other threads to run
}
