#include "map.h"
#include <stdio.h>
#include <stdlib.h>

Map *mapa_global = NULL;

Map *load_map(const char *path_file) {
    FILE *arquivo = fopen(path_file, "r");
    if (arquivo == NULL) {
        perror("Erro ao abrir o arquivo mapa.txt");
        return NULL;
    }

    int rows = 0, columns = 0, current_columns = 0, c;

    // Mede as dimensões do arquivo de texto
    while ((c = fgetc(arquivo)) != EOF) {
        if (c == '\n') {
            rows++;
            if (current_columns > columns) columns = current_columns;
            current_columns = 0;
        } else if (c != '\r') {
            current_columns++;
        }
    }
    if (current_columns > 0) {
        rows++;
        if (current_columns > columns) columns = current_columns;
    }

    rewind(arquivo);
    Map *mapa = (Map *)malloc(sizeof(Map));
    mapa->rows = rows;
    mapa->columns = columns;

    // Aloca as linhas da matriz
    mapa->cell_grid = (Cell **)malloc(rows * sizeof(Cell *));
    for (int i = 0; i < rows; i++) {
        mapa->cell_grid[i] = (Cell *)malloc(columns * sizeof(Cell));
        for (int j = 0; j < columns; j++) {
            mapa->cell_grid[i][j].type = EMPTY;
            mapa->cell_grid[i][j].direction = ' ';
            mapa->cell_grid[i][j].veiculo = NULL;
            mapa->cell_grid[i][j].id_semaforo = -1;
            pthread_mutex_init(&mapa->cell_grid[i][j].mutex, NULL);
        }
    }

    // Preenche as propriedades com base nos caracteres do arquivo
    int i = 0, j = 0;
    while ((c = fgetc(arquivo)) != EOF) {
        if (c == '\n') { i++; j = 0; continue; }
        if (c == '\r') continue;
        if (i >= rows || j >= columns) continue;

        switch (c) {
            case '>': mapa->cell_grid[i][j].direction = 'L'; mapa->cell_grid[i][j].type = ROADS; break;
            case '<': mapa->cell_grid[i][j].direction = 'O'; mapa->cell_grid[i][j].type = ROADS; break;
            case 'v': mapa->cell_grid[i][j].direction = 'S'; mapa->cell_grid[i][j].type = ROADS; break;
            case '^': mapa->cell_grid[i][j].direction = 'N'; mapa->cell_grid[i][j].type = ROADS; break;
            case '+': mapa->cell_grid[i][j].direction = ' '; mapa->cell_grid[i][j].type = INTERSECTION; break;
            case '|': mapa->cell_grid[i][j].direction = 'S'; mapa->cell_grid[i][j].type = ROADS; break; // Sentido genérico vertical
            default:  mapa->cell_grid[i][j].direction = ' '; mapa->cell_grid[i][j].type = EMPTY; break;
        }
        j++;
    }

    fclose(arquivo);
    return mapa;
}

void renderizar_mapa() {
    if (mapa_global == NULL) return;
    system("cls");
    printf("========= SIMULADOR DE TRÁFEGO DINÂMICO =========\n\n");

    for (int i = 0; i < mapa_global->rows; i++) {
        for (int j = 0; j < mapa_global->columns; j++) {
            if (mapa_global->cell_grid[i][j].veiculo != NULL) {
                printf("%2d", mapa_global->cell_grid[i][j].veiculo->id);
            } else {
                switch (mapa_global->cell_grid[i][j].type) {
                    case ROADS:
                        if (mapa_global->cell_grid[i][j].direction == 'L') printf(" >");
                        else if (mapa_global->cell_grid[i][j].direction == 'O') printf(" <");
                        else if (mapa_global->cell_grid[i][j].direction == 'N') printf(" ^");
                        else printf(" v");
                        break;
                    case INTERSECTION: printf(" +"); break;
                    default:           printf("  "); break;
                }
            }
        }
        printf("\n");
    }
    printf("\n=================================================\n");
}

int mover_veiculo(int x_atual, int y_atual, int x_destino, int y_destino) {
    if (mapa_global == NULL) return 0;
    if (x_destino < 0 || x_destino >= mapa_global->rows || y_destino < 0 || y_destino >= mapa_global->columns) return 0;

    Cell *origem = &mapa_global->cell_grid[x_atual][y_atual];
    Cell *destino = &mapa_global->cell_grid[x_destino][y_destino];

    if (pthread_mutex_trylock(&destino->mutex) == 0) {
        if (destino->veiculo == NULL) {
            pthread_mutex_lock(&origem->mutex);
            
            destino->veiculo = origem->veiculo;
            origem->veiculo = NULL;
            destino->veiculo->x = x_destino;
            destino->veiculo->y = y_destino;
            
            pthread_mutex_unlock(&origem->mutex);
            pthread_mutex_unlock(&destino->mutex);
            return 1;
        }
        pthread_mutex_unlock(&destino->mutex);
    }
    return 0;
}

void destroy_map(Map *mapa) {
    if (mapa == NULL) return;
    for (int i = 0; i < mapa->rows; i++) {
        for (int j = 0; j < mapa->columns; j++) {
            pthread_mutex_destroy(&mapa->cell_grid[i][j].mutex);
        }
        free(mapa->cell_grid[i]);
    }
    free(mapa->cell_grid);
    free(mapa);
}