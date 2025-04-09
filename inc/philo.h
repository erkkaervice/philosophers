/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philo.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: eala-lah <eala-lah@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/01 15:28:41 by eala-lah          #+#    #+#             */
/*   Updated: 2025/04/09 11:54:45 by eala-lah         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILO_H
# define PHILO_H

# include "../libft/inc/libft.h"
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
	int				thread_done;
	pthread_cond_t	done_cond;
}	t_philo;

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

typedef pthread_mutex_t	t_fork;

long long	ft_time(void);
void		ft_usleep(long long ms);
void		ft_printlog(t_data *data, int id, char *msg);
t_data		*ft_initdata(int ac, char **av);
void		*ft_routine(void *arg);
void		ft_threads(t_data *data, t_philo *philos);
void		ft_eat(t_philo *philo);
void		ft_sleepthink(t_philo *philo);
int			ft_status(t_data *data, t_philo *philos);
int			ft_stoplock(t_philo *philo);
int			ft_forks(t_philo *philo);
int			ft_reaper(t_data *data, t_philo *philo);

#endif
