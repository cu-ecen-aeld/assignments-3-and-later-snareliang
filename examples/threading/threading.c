#include "threading.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>


// Optional: use these functions to add debug or error prints to your application
#define DEBUG_LOG(msg,...)
//#define DEBUG_LOG(msg,...) printf("threading: " msg "\n" , ##__VA_ARGS__)
#define ERROR_LOG(msg,...) printf("threading ERROR: " msg "\n" , ##__VA_ARGS__)

void* threadfunc(void* thread_param)
{

    // TODO: wait, obtain mutex, wait, release mutex as described by thread_data structure
    // hint: use a cast like the one below to obtain thread arguments from your parameter
    //struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // Cast the void pointer to a pointer of type thread_data
    struct thread_data* thread_func_args = (struct thread_data *) thread_param;

    // Sleep before obtaining the mutex
    usleep(thread_func_args->wait_to_obtain_ms * 1000);

    // Obtain the mutex
    int result = pthread_mutex_lock(thread_func_args->mutex);
    if (result != 0) {
        ERROR_LOG("Failed to obtain mutex in threadfunc");
        // Set thread_complete_success to false before exiting
        thread_func_args->thread_complete_success = false;
        return NULL;
    }

    // Sleep after obtaining the mutex
    usleep(thread_func_args->wait_to_release_ms * 1000);

    // Release the mutex
    result = pthread_mutex_unlock(thread_func_args->mutex);
    if (result != 0) {
        ERROR_LOG("Failed to release mutex in threadfunc");
        // Set thread_complete_success to false before exiting
        thread_func_args->thread_complete_success = false;
        return NULL;
    }

    // Set thread_complete_success to true before exiting
    thread_func_args->thread_complete_success = true;

    // Return the thread_param
    return thread_param;
    
}

bool start_thread_obtaining_mutex(pthread_t *thread, pthread_mutex_t *mutex,int wait_to_obtain_ms, int wait_to_release_ms)
{
    /**
     * TODO: allocate memory for thread_data, setup mutex and wait arguments, pass thread_data to created thread
     * using threadfunc() as entry point.
     *
     * return true if successful.
     *
     * See implementation details in threading.h file comment block
     */
    // Allocate memory for thread_data and initialize it
    struct thread_data* thread_data = malloc(sizeof(struct thread_data));
    if (thread_data == NULL) {
        ERROR_LOG("Failed to allocate memory for thread_data\n");
        return false;
    }

    thread_data->mutex = mutex;
    thread_data->wait_to_obtain_ms = wait_to_obtain_ms;
    thread_data->wait_to_release_ms = wait_to_release_ms;

    // create the thread and pass thread_data to it
    if (pthread_create(thread, NULL, threadfunc, thread_data) != 0) {
        ERROR_LOG("Failed to create thread\n");
        free(thread_data);
        return false;
    }

    return true;



}

