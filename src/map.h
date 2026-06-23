#ifndef MAP_H
#define MAP_H

#include <pthread.h>

typedef enum {
    EMPTY,
    ROADS,
    INTERSECTION
} CellType;

// Estrutura do Veículo
typedef struct {
    int id;
    int tipo;       // 0: Carro comum, 1: Ambulância
    int velocidade; // Em ticks
    int x, y;       // Posição atual
} Veiculo;

// Estrutura de cada Célula da malha viária
typedef struct {
    CellType type;
    char direction;          // 'N', 'S', 'L', 'O' ou ' '
    Veiculo* veiculo;        // Ponteiro para o veículo (NULL se vazio)
    pthread_mutex_t mutex;   // Mutex individual da célula
    int id_semaforo;
} Cell;

// Estrutura que encapsula a Matriz Dinâmica do Mapa
typedef struct {
    int rows;
    int columns;
    Cell **cell_grid;
} Map;

// Declarações Globais
extern Map *mapa_global;

// Funções do Mapeador
Map *load_map(const char *path_file);
void renderizar_mapa();
void destroy_map(Map *mapa);
int mover_veiculo(int x_atual, int y_atual, int x_destino, int y_destino);

#endif