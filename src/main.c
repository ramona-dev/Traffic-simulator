#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "mapa.h"

int tick_atual = 0;
pthread_mutex_t mutex_relogio = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_tick = PTHREAD_COND_INITIALIZER;

void* thread_carro(void* arg) {
    Veiculo* meu_carro = (Veiculo*)arg;
    while (1) {
        pthread_mutex_lock(&mutex_relogio);
        pthread_cond_wait(&cond_tick, &mutex_relogio);
        pthread_mutex_unlock(&mutex_relogio);

        int prox_y = meu_carro->y + 1;
        if (prox_y >= mapa_global->columns) prox_y = 0;

        mover_veiculo(meu_carro->x, meu_carro->y, meu_carro->x, prox_y);
    }
    return NULL;
}

void* thread_relogio(void* arg) {
    while (1) {
        sleep(1);
        pthread_mutex_lock(&mutex_relogio);
        tick_atual++;
        
        renderizar_mapa();
        printf("[RELÓGIO] Tick: %d\n", tick_atual);
        
        pthread_cond_broadcast(&cond_tick);
        pthread_mutex_unlock(&mutex_relogio);
    }
    return NULL;
}

int main() {
    // Carrega o mapa dinamicamente a partir do arquivo txt
    // Como vamos compilar e rodar de dentro da pasta 'src', o arquivo está um nível acima (../mapa.txt)
    mapa_global = load_map("../mapa.txt");
    if (mapa_global == NULL) {
        return -1;
    }

    // Criação do carro teste inserido dinamicamente na linha 2 (que é uma via '>')
    Veiculo* carro1 = malloc(sizeof(Veiculo));
    carro1->id = 77;
    carro1->x = 2; 
    carro1->y = 0;
    mapa_global->cell_grid[carro1->x][carro1->y].veiculo = carro1;

    renderizar_mapa();

    pthread_t relogio, t_carro1;
    pthread_create(&relogio, NULL, thread_relogio, NULL);
    pthread_create(&t_carro1, NULL, thread_carro, carro1);

    pthread_join(relogio, NULL);

    destroy_map(mapa_global);
    return 0;
}