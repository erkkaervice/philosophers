/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/23 14:33:19 by eala-lah         ###   ########.fr       */
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
 * - last_meal: Timestamp of the last meal. Initialized to the current time
 *   upon philosopher creation, but updates during eating.
 * - id: Unique identifier for each philosopher.
 * - meals_eaten: The number of meals the philosopher has eaten.
 * - thread_done: Flag indicating whether the philosopher has completed
 *   necessary actions, such as eating or dying, and can finish its thread.
 * - thread: The philosopher's thread object used for synchronization.
 * - done_cond: Condition variable used for synchronizing thread completion
 *   by allowing philosophers to signal when they are finished eating or
 *   need to exit.
 * - left_fork, right_fork: Pointers to the mutexes representing the
 *   philosopher's left and right forks, respectively.
 * - data: A pointer to the shared data structure containing global simulation
 *   parameters and state.
 */
typedef struct s_philo
{
	long long		last_meal;
	int				id;
	int				meals_eaten;
	int				thread_done;
	pthread_t		thread;
	pthread_cond_t	done_cond;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	struct s_data	*data;
}	t_philo;

/* Structure representing the simulation data.
 * - start_time: The timestamp of when the simulation begins.
 * - num_philos: The number of philosophers in the simulation.
 * - time_to_die: The time (in milliseconds) before a philosopher dies from
 *   starvation.
 * - time_to_eat: The time (in milliseconds) a philosopher spends eating.
 * - time_to_sleep: The time (in milliseconds) a philosopher sleeps after
 *   eating.
 * - sim_stop: Flag to indicate if the simulation should stop. Set when a
 *   philosopher dies or a condition is met to end the simulation.
 * - must_eat: Global condition that dictates how many times each philosopher
 *   must eat before the simulation ends. It is checked by the philosophers
 *   as part of the termination condition.
 * - write_lock: Mutex used to synchronize logging operations, ensuring
 *   that output is not corrupted by concurrent threads.
 * - sim_stop_lock: Mutex used to safely check and modify the sim_stop flag,
 *   protecting access to the flag that determines if the simulation should
 *   continue or stop.
 * - last_meal_lock: Mutex used to protect access to philosophers' last meal
 *   timestamps to ensure thread-safe updates and comparisons.
 * - forks: Array of mutexes representing the forks used by philosophers
 *   (shared resources).
 * - philos: Array of philosopher structures. Each philosopher contains
 *   their own state and references to the shared forks, ensuring their
 *   individual actions are synchronized with the global simulation.
 */
typedef struct s_data
{
	long long		start_time;
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				sim_stop;
	int				must_eat;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	sim_stop_lock;
	pthread_mutex_t	last_meal_lock;
	pthread_mutex_t	*forks;
	t_philo			*philos;
}	t_data;

/* --- Core Simulation Functions ---
 * Functions related to simulation time, initialization, and managing
 * philosopher actions.
 */
long long	ft_time(void);
void		ft_printlog(t_philo *philo, char *msg);
t_data		*ft_initdata(int ac, char **av);

/* --- Philosopher Thread and Routine Management ---
 * Functions for creating and managing philosopher threads, and handling
 * their routines.
 */
void		ft_threads(t_data *data, t_philo *philos);
void		*ft_routine(void *arg);
void		ft_wait(t_data *data, t_philo *philos);
void		ft_solo(t_philo *philo);

/* --- Philosopher Action Functions ---
 * Functions that simulate the philosopher's actions such as eating,
 * sleeping, and interacting with forks.
 */
void		ft_eat(t_philo *philo);
void		ft_sleepthink(t_philo *philo);
int			ft_forks(t_philo *philo);

/* --- Simulation Control and Monitoring ---
 * Functions for controlling the simulation's status, stopping the
 * simulation, and checking the philosophers' status.
 */
int			ft_stoplock(t_philo *philo);
int			ft_status(t_data *data, t_philo *philos);
int			ft_check_meals(t_data *data, t_philo *philos);

/* --- Cleanup Functions ---
 * Functions for cleaning up the simulation after it ends, including
 * freeing memory and destroying mutexes.
 */
void		ft_cleanup(t_data *data, t_philo *philos);
int			ft_maxmeal(t_data *data, t_philo *philos);
void		ft_single_philo(t_philo *philo);

/* --- Additional Utility Functions ---
 * Miscellaneous helper functions used throughout the simulation.
 */
int			ft_reaper(t_data *data, t_philo *philo);

#endif
