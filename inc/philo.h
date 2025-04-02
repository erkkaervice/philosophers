/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/02 14:10:04 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>

/* Declare philosopher struct */
typedef struct s_philo
{
    int             id;
    long long      last_meal;
    int             meals_eaten;
    pthread_t      thread;
    struct s_data  *data;            // Pointer to simulation data
    pthread_mutex_t *left_fork;      // Mutex for left fork
    pthread_mutex_t *right_fork;     // Mutex for right fork
}   t_philo;

/* Declare data struct */
typedef struct s_data
{
    int             num_philos;
    int             time_to_die;
    int             time_to_eat;
    int             time_to_sleep;
    int             must_eat;
    int             sim_stop;        // Flag to stop simulation
    long long      start_time;
    pthread_mutex_t *forks;           // Array of forks (mutexes)
    pthread_mutex_t write_lock;       // Protects shared resource for logging
    pthread_mutex_t sim_stop_lock;    // Protects sim_stop flag
    t_philo         *philos;          // Pointer to array of philosophers
}   t_data;

typedef pthread_mutex_t t_fork;        // Fork type is a mutex for each fork

/* Utility functions (utils.c) */
long long  get_time(void);
void        ft_usleep(long long ms);
int         ft_atoi(const char *str);
void        print_log(t_data *data, int id, char *msg);

/* Initialization functions (init.c) */
t_data      *init_data(int argc, char **argv);
int         init_philos(t_data *data, t_philo *philos);  // Corrected function signature

/* Simulation functions (sim_loop.c) */
void        *routine(void *arg);
void        start_threads(t_data *data, t_philo *philos); // Corrected function signature

/* Action functions (actions.c) */
void        eat(t_philo *philo);
void        sleep_think(t_philo *philo);

/* Monitoring functions (monitoring.c) */
int         check_simulation_status(t_data *data, t_philo *philos);  // Corrected function signature

#endif
