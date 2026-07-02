#include "sincronizacao.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <bits/pthreadtypes.h>

pthread_mutex_t g_mutex_log;

void sincronizacao_destruir(void) {
    if (pthread_mutex_init(&g_mutex_log, NULL) != 0) {
        fprintf(stderr, "Erro ao inicializar mutex de log\n");
        exit(EXIT_FAILURE);
    }
}

void sincronizacao_destruir(void) {
    pthread_mutex_destroy(&g_mutex_log);
}

void log_evento(const char *formato, ...) {
    va_list args;
    pthread_mutex_lock(&g_mutex_log);

    va_start(args, formato);
    vprintf(stdout, formato, args);
    va_end(args);
    fflush(stdout);

    pthread_mutex_unlock(&g_mutex_log);
}

void travar_em_ordem(pthread_mutex_t *mutexA, int idA,
                      pthread_mutex_t *mutexB, int idB) {
    if (idA == idB) {
        /* mesmo recurso: trava uma única vez */
        pthread_mutex_lock(mutexA);
        return;
    }
    if (idA < idB) {
        pthread_mutex_lock(mutexA);
        pthread_mutex_lock(mutexB);
    } else {
        pthread_mutex_lock(mutexB);
        pthread_mutex_lock(mutexA);
    }
}

void destravar_em_ordem(pthread_mutex_t *mutexA, int idA,
                         pthread_mutex_t *mutexB, int idB) {
    if (idA == idB) {
        pthread_mutex_unlock(mutexA);
        return;
    }

    if (idA < idB) {
        pthread_mutex_unlock(mutexB);
        pthread_mutex_unlock(mutexA);
        
    } else {
        pthread_mutex_unlock(mutexA);
        pthread_mutex_unlock(mutexB);
    }
}
