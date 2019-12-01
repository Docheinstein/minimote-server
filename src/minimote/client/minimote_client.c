#include <pthread.h>
#include <malloc.h>
#include <logging/logging.h>
#include "minimote_client.h"

#define TCP_BUFFER_SIZE 4096

static pthread_mutex_t minimote_client_next_id_mutex = PTHREAD_MUTEX_INITIALIZER;
static int minimote_client_next_id = 1;

static int minimote_client_get_next_id() {
    int val;
    pthread_mutex_lock(&minimote_client_next_id_mutex);
    val = minimote_client_next_id;
    ++minimote_client_next_id;
    pthread_mutex_unlock(&minimote_client_next_id_mutex);
    return val;
}

void minimote_client_init(
        minimote_client *client,
        struct sockaddr_in address,
        minimote_controller_config controller_config
) {
    client->id = minimote_client_get_next_id();
    client->address = address;
    rolling_buffer_init(&client->tcp_buffer, TCP_BUFFER_SIZE);
    minimote_controller_init(&client->controller, controller_config);
}

void minimote_client_destroy(minimote_client *client) {
    t();
    rolling_buffer_destroy(&client->tcp_buffer);
}
