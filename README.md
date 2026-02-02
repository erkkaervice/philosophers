# Philosophers

A multithreaded simulation of the classic **Dining Philosophers problem**, implemented in C. This project explores the basics of threading, process synchronization, mutexes, and shared memory.

## 💡 Overview

In this simulation:
* One or more philosophers sit at a round table.
* There is a large bowl of spaghetti in the middle of the table.
* The philosophers alternatively **eat**, **think**, or **sleep**.
* There are as many forks as there are philosophers.
* Because serving spaghetti with a single fork is very difficult, a philosopher takes their right and their left forks to eat, one in each hand.
* **Goal:** The simulation stops when a philosopher dies of starvation or when all philosophers have eaten enough. Every philosopher needs to eat and should never starve.

## 🛠️ Implementation Details

This project uses **threads** and **mutexes** to manage concurrency and prevent data races.

* **Threads:** Each philosopher is a thread (`pthread_create`).
* **Mutexes:** * Forks are protected by mutexes (`pthread_mutex_t`).
    * Shared data (printing logs, checking simulation status, updating meal counts) is protected by specific mutex locks (`write_lock`, `sim_stop_lock`, `last_meal_lock`).
* **Deadlock Prevention:** To prevent philosophers from instantly deadlocking (everyone taking their left fork and waiting forever for the right), even-numbered philosophers delay their start slightly to stagger fork acquisition.
* **Precision Timing:** A custom `ft_usleep` function is implemented to ensure accurate timing for eating and sleeping actions, preventing CPU hogging while waiting.

## 📦 Installation & Compilation

Cloning the repository and compiling the executable:

```bash
git clone <repository-url>
cd philosophers
make
```

This will generate the `philo` executable.

## 🚀 Usage

Run the simulation with the following arguments:

```bash
./philo number_of_philosophers time_to_die time_to_eat time_to_sleep [number_of_times_each_philosopher_must_eat]
```

### Arguments

1.  **number_of_philosophers**: The number of philosophers and also the number of forks.
2.  **time_to_die**: (in milliseconds) If a philosopher doesn't start eating 'time_to_die' ms after the beginning of their last meal or the start of the simulation, they die.
3.  **time_to_eat**: (in milliseconds) The time it takes for a philosopher to eat. During this time, they will need to hold two forks.
4.  **time_to_sleep**: (in milliseconds) The time a philosopher will spend sleeping.
5.  **[number_of_times_each_philosopher_must_eat]**: (optional) If all philosophers have eaten at least 'number_of_times_each_philosopher_must_eat' times, the simulation stops. If not specified, the simulation stops when a philosopher dies.

### Examples

```bash
# 5 Philosophers, 800ms to die, 200ms to eat, 200ms to sleep.
# No one should die.
./philo 5 800 200 200

# 5 Philosophers, 800ms to die, 200ms to eat, 200ms to sleep.
# Simulation stops after everyone eats 7 times.
./philo 5 800 200 200 7

# 4 Philosophers, 310ms to die, 200ms to eat, 100ms to sleep.
# A philosopher should die.
./philo 4 310 200 100
```

## 📂 Project Structure

* **`src/main.c`**: Entry point, argument validation, and cleanup calls.
* **`src/init.c`**: Initialization of memory, mutexes, and philosopher structures.
* **`src/simulation.c`**: Core thread routines (`ft_routine`), thread creation, and monitoring logic.
* **`src/actions.c`**: Philosopher actions (taking forks, eating, sleeping, thinking) and logging.
* **`src/exit.c`**: Logic for checking death conditions (`ft_reaper`), simulation status, and stopping threads.
* **`inc/philo.h`**: Header file containing struct definitions and function prototypes.

## 🧪 Testing

The Makefile includes a test rule that downloads a tester script:

```bash
make test
```

*Note: This pulls an external script from `erkkaervice/area51`.*

## ⚠️ Key Constraints Handled
* **Data Races:** Strictly avoided using mutex locks whenever reading or writing shared memory (like the `sim_stop` flag or `last_meal` timestamps).
* **CPU Usage:** Optimized using `usleep` loops to check for death conditions frequently without busy-waiting aggressively.
* **Solo Case:** Special handling for 1 philosopher (who has only 1 fork and inevitably dies).
