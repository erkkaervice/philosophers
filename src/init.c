#include "philo.h"

/*
 * init_mutexes - Allocates memory and initializes mutexes for forks.
 *
 * This function allocates memory for the mutex array representing the forks 
 * for each philosopher. It also initializes each mutex. If any mutex 
 * initialization fails, all previously initialized mutexes are destroyed 
 * and memory is freed.
 *
 * Parameters:
 * - data: A pointer to the shared data structure that holds the mutex array.
 *
 * Returns:
 * - 0 if initialization is successful.
 * - 1 if any mutex initialization fails.
 */
static int init_mutexes(t_data *data)
{
    int i;

    // Initialize forks mutexes
    data->forks = malloc(sizeof(pthread_mutex_t) * data->num_philos);
    if (!data->forks)
    {
        ft_printf("Memory allocation failed for forks\n");
        free(data);
        return (1);
    }
    i = 0;
    while (i < data->num_philos)
    {
        if (pthread_mutex_init(&data->forks[i], NULL) != 0)
        {
            ft_printf("Failed to initialize mutex for fork %d\n", i);
            while (i-- > 0)
                pthread_mutex_destroy(&data->forks[i]);
            free(data->forks);
            free(data);
            return (1);
        }
        i++;
    }
    return (0);
}

/*
 * init_mutexes_locks - Initializes additional mutex locks for write and 
 * simulation stop flags.
 *
 * This function initializes the mutex for logging output (write_lock) and 
 * the mutex used to safely modify the simulation stop flag (sim_stop_lock).
 * If any initialization fails, already initialized mutexes are destroyed 
 * and the function returns an error.
 *
 * Parameters:
 * - data: A pointer to the shared data structure containing mutexes.
 *
 * Returns:
 * - 0 if both mutexes are successfully initialized.
 * - 1 if any mutex initialization fails.
 */
static int init_mutexes_locks(t_data *data)
{
    if (pthread_mutex_init(&data->write_lock, NULL) != 0)
    {
        ft_printf("Failed to initialize write_lock mutex\n");
        return (1);
    }
    else
    {
        ft_printf("write_lock initialized successfully\n");
    }

    if (pthread_mutex_init(&data->sim_stop_lock, NULL) != 0)
    {
        ft_printf("Failed to initialize sim_stop_lock mutex\n");
        pthread_mutex_destroy(&data->write_lock);
        return (1);
    }
    else
    {
        ft_printf("sim_stop_lock initialized successfully\n");
    }

    if (pthread_mutex_init(&data->last_meal_lock, NULL) != 0)
    {
        ft_printf("Failed to initialize last_meal_lock mutex\n");
        pthread_mutex_destroy(&data->write_lock);
        pthread_mutex_destroy(&data->sim_stop_lock);
        return (1);
    }
    else
    {
        ft_printf("last_meal_lock initialized successfully\n");
    }

    return (0);
}

/*
 * init_philos - Initializes philosopher structures with default values.
 *
 * This function initializes each philosopher with their unique ID, initial 
 * meal count, and sets up the forks for each philosopher. The left and right 
 * forks are assigned based on the philosopher's position in the array.
 *
 * Parameters:
 * - data: A pointer to the shared data structure holding simulation settings.
 * - philos: An array of philosopher structures to initialize.
 */
#include <stdio.h>
#include <stdlib.h>

int init_philos(t_data *data, t_philo *philos)
{
    int i;

    i = 0;
    while (i < data->num_philos)
    {
        philos[i].thread_done = 0;
        pthread_cond_init(&philos[i].done_cond, NULL);
        philos[i].id = i + 1;
        philos[i].meals_eaten = 0;
        philos[i].last_meal = get_time();  // Initialize last meal time
        philos[i].data = data;
        philos[i].left_fork = &data->forks[i];
        philos[i].right_fork = &data->forks[(i + 1) % data->num_philos];
        i++;
    }
    return (0);  // Return success
}


/*
 * init_alloc_data - Allocates and initializes the data structure for 
 * simulation parameters.
 *
 * This function allocates memory for the main simulation data structure 
 * and initializes basic parameters such as the number of philosophers, 
 * their eating times, and the start time. If memory allocation fails at 
 * any point, the function returns NULL.
 *
 * Parameters:
 * - av: An array of argument strings passed to the program.
 *
 * Returns:
 * - A pointer to the allocated and initialized simulation data structure.
 * - NULL if memory allocation fails.
 */
static t_data *init_alloc_data(char **av)
{
    t_data *data;

    data = malloc(sizeof(t_data));
    if (!data)
    {
        ft_printf("Memory allocation failed for data\n");
        return (NULL);
    }

    data->num_philos = ft_atoi(av[1]);
    data->philos = malloc(sizeof(t_philo) * data->num_philos);
    if (!data->philos)
    {
        ft_printf("Memory allocation failed for philosophers\n");
        free(data);
        return (NULL);
    }

    data->time_to_die = ft_atoi(av[2]);
    data->time_to_eat = ft_atoi(av[3]);
    data->time_to_sleep = ft_atoi(av[4]);
    data->sim_stop = 0;
    data->start_time = get_time();

    return (data);
}

/*
 * init_data - Initializes the simulation data structure and allocates 
 * necessary memory for mutexes, philosophers, and simulation parameters.
 *
 * This function calls helper functions to allocate and initialize the data 
 * structure, mutexes, and philosophers. If any initialization fails, 
 * it frees any previously allocated memory and returns NULL.
 *
 * Parameters:
 * - ac: The number of command-line arguments.
 * - av: The array of command-line argument strings.
 *
 * Returns:
 * - A pointer to the initialized data structure.
 * - NULL if initialization fails at any step.
 */
t_data *init_data(int ac, char **av)
{
    t_data *data;

    (void)ac;  // Suppress unused parameter warning

    // Allocate memory and initialize simulation data
    data = init_alloc_data(av);
    if (!data)
        return (NULL);

    // Initialize forks and mutexes
    if (init_mutexes(data)) // This initializes the forks mutex
        return (NULL);
    if (init_mutexes_locks(data)) // This initializes the additional mutexes
        return (NULL);

    // Initialize philosopher structures
    if (init_philos(data, data->philos))
        return (NULL);

    return (data);
}
