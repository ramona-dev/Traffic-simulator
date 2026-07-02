#ifndef SINCRONIZACAO_H
#define SINCRONIZACAO_H

extern pthread_mutex_t g_mutex_log;

void sincrnizacao_inicializar(void);

void sincronizacao_destruir(void);

void log_evento(const char *formato, ...);

void travar_em_ordem(pthread_mutex_t *mutexA, int idA,
                        pthread_mutex_t *mutexB, int idB);

void destravar_em_ordem(pthread_mutex_t *mutexA, int idA,
                        pthread_mutex_t *mutexB, int idB);

#endif