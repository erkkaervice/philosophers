/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:54 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 13:47:43 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philo.h"

/* Print log message with timestamp */
void	print_log(t_data *data, int id, char *msg)
{
	long long	time;

	pthread_mutex_lock(&data->write_lock); // Lock before writing to ensure thread safety
	time = get_time() - data->start_time; // Calculate elapsed time since the start
	printf("%lld %d %s\n", time, id, msg); // Print the log message
	pthread_mutex_unlock(&data->write_lock); // Unlock after writing
}
