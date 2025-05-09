/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/05/09 11:37:17 by eala-lah         ###   ########.fr       */
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
 * - id: Number identifier for each philosopher.
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
 * - time_to_die: The time before a philosopher dies from starvation.
 * - time_to_eat: The time a philosopher spends eating.
 * - time_to_sleep: The time a philosopher sleeps after eating.
 * - sim_stop: Flag to indicate if the simulation should stop.
 * - must_eat: Global condition that dictates how many times each philosopher
 *   must eat before the simulation ends. It is checked by the philosophers
 *   as part of the termination condition.
 * - write_lock: Mutex used to synchronize logging operations.
 * - sim_stop_lock: Mutex used to safely check and modify the sim_stop flag.
 * - last_meal_lock: Mutex used to protect access to philosophers' last meal.
 * - forks: Array of mutexes representing the forks used by philosophers.
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
 * Functions for managing timestamps, managing actions and data initialization.
 */
long long	ft_time(void);
void		ft_printlog(t_philo *philo, char *msg);
t_data		*ft_initdata(int ac, char **av);

/* --- Philosopher Action Functions ---
 * Functions that simulate the philosopher's actions such as eating and sleeping.
 */
void		ft_eat(t_philo *philo);
void		ft_sleepthink(t_philo *philo);

/* --- Simulation Control and Monitoring ---
 * Functions for checking the philosophers' status and stopping the simulation.
 */
int			ft_status(t_data *data, t_philo *philos);
int			ft_stoplock(t_philo *philo);
int			ft_maxmeal(t_data *data, t_philo *philos);

/* --- Cleanup Function ---
 * Function for cleaning up the simulation after it ends.
 */
void		ft_cleanup(t_data *data, t_philo *philos);
void		ft_usleep(long long duration_ms);

#endif
