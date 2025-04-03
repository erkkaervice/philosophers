/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/03 11:05:28 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <pthread.h>
# include <sys/time.h>

typedef struct s_philo
{
	int				id;
	long long		last_meal;
	int				meals_eaten;
	pthread_t		thread;
	struct s_data	*data;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
}	t_philo;

typedef struct s_data
{
	int				num_philos;
	int				time_to_die;
	int				time_to_eat;
	int				time_to_sleep;
	int				must_eat;
	int				sim_stop;
	long long		start_time;
	pthread_mutex_t	*forks;
	pthread_mutex_t	write_lock;
	pthread_mutex_t	sim_stop_lock;
	t_philo			*philos;
}	t_data;

typedef pthread_mutex_t	t_fork;

long long	get_time(void);
void		ft_usleep(long long ms);
int			ft_atoi(const char *str);
void		print_log(t_data *data, int id, char *msg);

t_data		*init_data(int argc, char **argv);

void		*routine(void *arg);
void		start_threads(t_data *data, t_philo *philos);

void		eat(t_philo *philo);
void		sleep_think(t_philo *philo);

int			check_simulation_status(t_data *data, t_philo *philos);

#endif
