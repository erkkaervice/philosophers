/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/23 16:14:34 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

/* Includes standard libraries:
 * - Libft for utilities
 * - pthread for threads and sync
 * - sys/time for timing functions
 */
# include <unistd.h>
# include <stdio.h>
# include <stdlib.h>
# include <pthread.h>
# include <sys/time.h>

/* Philosopher struct:
 * - last_meal: timestamp of last meal
 * - id: philosopher ID
 * - meals_eaten: count of meals eaten
 * - thread: thread object
 * - left_fork, right_fork: mutex forks pointers
 * - data: pointer to shared data struct
 */
typedef struct s_philo
{
	long long		last_meal;
	int				id;
	int				meals_eaten;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	struct s_data	*data;
}	t_philo;

/* Shared data struct:
 * - start_time: simulation start time
 * - num_philos: number of philosophers
 * - time_to_die/eat/sleep: timing params
 * - must_eat: meals required to finish
 * - sim_stop: simulation stop flag
 * - write_lock, sim_stop_lock, last_meal_lock: mutexes for sync
 * - forks: array of fork mutexes
 * - philos: array of philosopher structs
 */
typedef struct s_data
{
	long long		start_time;
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat;
	int				sim_stop;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	sim_stop_lock;
	pthread_mutex_t	last_meal_lock;
	pthread_mutex_t	*forks;
	t_philo			*philos;
}	t_data;

/* Core simulation functions */
long long	ft_time(void);
void		ft_printlog(t_philo *philo, char *msg);
t_data		*ft_initdata(int ac, char **av);

/* Philosopher actions */
void		ft_eat(t_philo *philo);
void		ft_sleepthink(t_philo *philo);

/* Simulation control and monitoring */
void		ft_threads(t_data *data, t_philo *philos);
void		ft_usleep(t_philo *philo, long long duration_ms);
int			ft_status(t_data *data, t_philo *philos);
int			ft_stoplock(t_philo *philo);
int			ft_maxmeal(t_data *data, t_philo *philos);
int			ft_atoi(char const *str);

/* Cleanup simulation resources */
void		ft_cleanup(t_data *data, t_philo *philos);

#endif
