/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/09 12:46:07 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

/* Includes necessary libraries for the project.
 * - Libft: Includes common string, memory, and utility functions.
 * - pthread: Provides the pthread library for creating threads and
 *   synchronization.
 * - sys/time: Includes functions for time-related operations (e.g.,
 *   gettimeofday).
 */
# include "../libft/inc/libft.h"
# include <pthread.h>
# include <sys/time.h>

/* Structure representing a philosopher.
 * - id: Unique identifier for each philosopher.
 * - last_meal: Timestamp of the last meal.
 * - meals_eaten: The number of meals the philosopher has eaten.
 * - thread: The philosopher's thread.
 * - data: A pointer to the shared data structure.
 * - left_fork, right_fork: Pointers to the mutexes representing the
 *   philosopher's forks.
 * - thread_done: Flag indicating whether the philosopher's thread has
 *   finished.
 * - done_cond: Condition variable used for synchronizing thread
 *   completion.
 */
typedef struct s_philo
{
	int				id;
	long long		last_meal;
	int				meals_eaten;
	pthread_t		thread;
	struct s_data	*data;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	int				thread_done;
	pthread_cond_t	done_cond;
}	t_philo;

/* Structure representing the simulation data.
 * - num_philos: The number of philosophers.
 * - time_to_die: The time (in milliseconds) before a philosopher dies from
 *   starvation.
 * - time_to_eat: The time (in milliseconds) a philosopher takes to eat.
 * - time_to_sleep: The time (in milliseconds) a philosopher sleeps after
 *   eating.
 * - sim_stop: Flag to stop the simulation.
 * - start_time: The simulation's start time.
 * - forks: Array of mutexes representing the forks used by philosophers.
 * - write_lock: Mutex used to synchronize logging operations.
 * - sim_stop_lock: Mutex used to safely check and modify the sim_stop flag.
 * - last_meal_lock: Mutex used to protect access to philosophers' last meal
 *   timestamps.
 * - philos: Array of philosopher structures.
 */
typedef struct s_data
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				sim_stop;
	long long		start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	sim_stop_lock;
	pthread_mutex_t	last_meal_lock;
	t_philo			*philos;
}	t_data;

/* Type definition for fork mutexes.
 * This is used to represent the forks in the simulation.
 */
typedef pthread_mutex_t	t_fork;

/* --- Core Simulation Functions ---
 * Functions related to simulation time, initialization, and managing
 * philosopher actions.
 */
long long	ft_time(void);
void		ft_printlog(t_data *data, int id, char *msg);
t_data		*ft_initdata(int ac, char **av);
void		*ft_routine(void *arg);
void		ft_threads(t_data *data, t_philo *philos);
int			ft_status(t_data *data, t_philo *philos);

/* --- Philosopher Action Functions ---
 * Functions that simulate the philosopher's actions such as eating,
 * sleeping, and interacting with forks.
 */
void		ft_eat(t_philo *philo);
void		ft_sleepthink(t_philo *philo);
int			ft_forks(t_philo *philo);

/* --- Simulation Control Functions ---
 * Functions for controlling the simulation's status, stopping the
 * simulation, and reaping dead philosophers.
 */
int			ft_stoplock(t_philo *philo);
int			ft_reaper(t_data *data, t_philo *philo);

#endif
