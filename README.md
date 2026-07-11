# Traffic-simulator

![C11](https://img.shields.io/badge/C-11-blue) ![Pthreads](https://img.shields.io/badge/Concurrency-Pthreads-informational) ![Platform](https://img.shields.io/badge/Platform-Linux%20%7C%20Windows%20(MSYS2)-lightgrey)

Simulador de tráfego urbano em C, desenvolvido para a disciplina de
**Sistemas Operacionais**, com foco no aprendizado de **Concorrência,
Sincronização e Deadlocks**.

Cada veículo é representado por uma thread (Pthreads) que compete por
espaços de uma malha viária com 16 cruzamentos, semáforos, vias de mão
única e mão dupla, e uma ambulância com prioridade de passagem.

## Índice

- [Requisitos do enunciado atendidos](#requisitos-do-enunciado-atendidos)
- [Requisitos de ambiente](#requisitos-de-ambiente)
- [Estrutura do projeto](#estrutura-do-projeto)
- [Como rodar o projeto](#como-rodar-o-projeto)
- [Ajustando parâmetros da simulação](#ajustando-parâmetros-da-simulação)
- [Verificação de corretude (ThreadSanitizer)](#verificação-de-corretude-threadsanitizer)
- [Documentação adicional](#documentação-adicional)
- [Integrantes e responsabilidades](#integrantes-e-responsabilidades)

## Requisitos do enunciado atendidos

| Requisito | Mínimo exigido | Implementado |
|---|---|---|
| Cruzamentos | ≥ 8 | **16** (malha 4×4) |
| Veículos simultâneos | 10 a 20 | **15** (14 carros comuns + 1 ambulância) |
| Vias de mão única e dupla | sim | sim, alternadas na malha |
| Prioridade de veículo especial | sim | ambulância com prioridade de passagem |
| Sincronização sem espera ocupada | sim | mutexes + variáveis de condição + semáforos |
| Encerramento sem deadlock/vazamento | sim | validado com ThreadSanitizer |

## Requisitos de ambiente

O projeto usa apenas C11 padrão, Pthreads e semáforos POSIX
(`<semaphore.h>`) — sem dependências externas.

### Linux (recomendado, inclusive GitHub Codespaces)

```bash
sudo apt-get install -y build-essential   # gcc + make, se ainda não tiver
```

### Windows

- MSYS2 instalado, com o terminal **MSYS2 MinGW64** aberto.
- Pacotes: `mingw-w64-x86_64-gcc` e `make`.

```bash
pacman -S --needed mingw-w64-x86_64-gcc make
```

### macOS

> ⚠️ **Atenção:** o macOS não implementa semáforos POSIX *unnamed*
> (`sem_init` retorna erro nativamente). Este projeto usa `sem_init`
> diretamente, então **não builda em macOS sem uma camada de
> compatibilidade** (ex.: substituir por `dispatch_semaphore_t` ou
> semáforos nomeados). Use Linux ou WSL/MSYS2 no Windows por enquanto.

## Estrutura do projeto

```
Traffic-simulator/
├── src/                     // código-fonte principal
│   ├── main.c                // ponto de entrada da simulação
│   ├── mapa.c                // funções para criar e gerenciar o mapa
│   ├── veiculos.c            // lógica dos carros e ambulância
│   ├── relogio.c              // controle do tempo (ticks)
│   ├── semaforos.c            // controle dos sinais de trânsito
│   └── sincronizacao.c        // mutex, semáforos, variáveis de condição
│
├── include/                 // cabeçalhos (interfaces)
│   ├── mapa.h
│   ├── veiculos.h
│   ├── relogio.h
│   ├── semaforos.h
│   └── sincronizacao.h
│
├── docs/                     // documentação
│   ├── relatorio.md           // explicação técnica
│   ├── planejamento.md        // divisão de tarefas e decisões
│   └── Mapa-referencia.txt    // mapa lógico original que inspirou a malha
│
├── README.md                 // este arquivo
└── Makefile
```

## Como rodar o projeto

### Linux / GitHub Codespaces

1. **Confirme que `gcc` e `make` estão instalados:**

   ```bash
   gcc --version
   make --version
   ```

   Se algum comando não for encontrado, instale as ferramentas de build:

   ```bash
   sudo apt-get update
   sudo apt-get install -y build-essential
   ```

2. **Clone o repositório e entre na pasta do projeto:**

   ```bash
   git clone https://github.com/ramona-dev/Traffic-simulator.git
   cd Traffic-simulator
   ```

3. **Compile o projeto:**

   ```bash
   make
   ```

   Isso cria a pasta `build/` com os arquivos `.o` de cada módulo e gera o
   binário `traffic-simulator` na raiz do projeto. A saída esperada é uma
   sequência de linhas `gcc ... -c src/xxx.c -o build/xxx.o`, seguida da
   linha final de link (`gcc build/*.o -o traffic-simulator`), **sem
   nenhuma linha de warning**. Se aparecer algum warning do compilador
   (`-Wall -Wextra` estão ativos), revise o código antes de prosseguir —
   ele sinaliza algo que pode ser um bug real.

4. **Confirme que o binário foi criado:**

   ```bash
   ls -l traffic-simulator
   ```

5. **Execute a simulação:**

   ```bash
   ./traffic-simulator
   ```

   A tela do terminal vai limpar e redesenhar a cada 250ms, mostrando a
   malha viária, os semáforos e os veículos em movimento (veja a seção
   abaixo para entender a saída). Deixe rodar até o fim — leva cerca de
   1 minuto (240 ticks) — ou interrompa a qualquer momento com `Ctrl+C`.

   Alternativamente, os passos 3 e 5 podem ser feitos de uma vez com:

   ```bash
   make run
   ```

6. **(Opcional) Limpe os artefatos de build quando terminar:**

   ```bash
   make clean
   ```

   Isso remove a pasta `build/` e o binário `traffic-simulator`, deixando
   só o código-fonte — útil antes de recompilar do zero ou de comitar
   mudanças.

#### Problemas comuns

| Sintoma | Causa provável | Solução |
|---|---|---|
| `bash: make: command not found` | `make` não instalado | `sudo apt-get install -y build-essential` |
| `bash: gcc: command not found` | `gcc` não instalado | idem acima |
| `undefined reference to pthread_create` | linkagem sem `-pthread` | não deve ocorrer com o Makefile fornecido; se compilar manualmente, inclua `-pthread` |
| Tela "quebrada" / caracteres estranhos (`\033[H`) aparecendo como texto | terminal sem suporte a ANSI | rode em um terminal moderno (GNOME Terminal, iTerm2, Windows Terminal); evite terminais legados |

### Windows (MSYS2 MinGW64)

1. **Instale o MSYS2** (se ainda não tiver): baixe em
   [msys2.org](https://www.msys2.org/) e siga o instalador padrão.

2. **Abra o terminal "MSYS2 MinGW64"** (não o "MSYS2 MSYS" nem o "MSYS2
   UCRT64" — o nome exato importa, pois define quais compiladores ficam
   disponíveis no `PATH`).

3. **Instale o compilador e o `make`:**

   ```bash
   pacman -S --needed mingw-w64-x86_64-gcc make
   ```

   Confirme a instalação:

   ```bash
   gcc --version
   make --version
   ```

4. **Entre na pasta do projeto.** Se você clonou ou extraiu o projeto em
   uma pasta do Windows (ex.: Downloads), o caminho equivalente dentro do
   MSYS2 costuma ser:

   ```bash
   cd /c/Users/$USERNAME/Downloads/Traffic-simulator*/Traffic-simulator
   ```

   (troque `$USERNAME` pelo seu usuário do Windows, ou ajuste o caminho
   conforme onde o projeto estiver).

5. **Compile:**

   ```bash
   make
   ```

6. **Execute:**

   ```bash
   ./traffic-simulator
   ```

7. **(Opcional) Limpe os artefatos:**

   ```bash
   make clean
   ```

O Makefile usa `-Wall -Wextra -std=c11 -pthread` em ambas as plataformas,
então qualquer warning de compilação indica algo que deve ser revisado.

A simulação roda por **240 ticks** (configurável em
`DURACAO_SIMULACAO_TICKS` em `src/main.c`), cada tick durando 250ms
(configurável em `DURACAO_TICK_MS`), totalizando cerca de 1 minuto de
execução. A cada tick a tela é limpa e redesenhada (sequência ANSI
`\033[H\033[J`), mostrando:

- a malha viária (ruas, cruzamentos, paredes);
- o estado dos semáforos em cada cruzamento (`+` = verde no eixo
  Norte-Sul, `x` = vermelho no eixo Norte-Sul);
- a posição de cada veículo (`#`);
- uma lista textual abaixo do mapa com id, tipo, posição, direção e
  velocidade de cada veículo ainda ativo.

Ao final da simulação, o programa imprime quantos ticks decorreram e
encerra normalmente (todas as threads são finalizadas com `pthread_join`
antes do `return 0`, sem vazamento de threads).

## Ajustando parâmetros da simulação

No início de `src/main.c`:

| Constante | Significado |
|---|---|
| `NUM_CARROS_COMUNS` | quantidade de carros comuns (padrão 14) |
| `NUM_AMBULANCIAS` | quantidade de ambulâncias (padrão 1) |
| `DURACAO_TICK_MS` | duração de cada tick em milissegundos |
| `PERIODO_TROCA_SINAL` | a cada quantos "pulsos" de 100ms o controlador de semáforos alterna os sinais automaticamente |
| `DURACAO_SIMULACAO_TICKS` | quantos ticks a simulação roda antes de encerrar |

O enunciado exige entre 10 e 20 carros simultâneos; o padrão (14 carros +
1 ambulância = 15 threads de veículo) atende a esse intervalo.

## Verificação de corretude (ThreadSanitizer)

O projeto foi validado também com o ThreadSanitizer do GCC, que detecta
condições de corrida (data races) em tempo de execução (requer GCC com
suporte a `-fsanitize=thread`, disponível em Linux/MSYS2):

```bash
gcc -Wall -Wextra -std=c11 -Iinclude -pthread -g -fsanitize=thread \
    -o traffic-simulator-tsan src/*.c
./traffic-simulator-tsan
```

Em múltiplas execuções de validação durante o desenvolvimento, a versão
final não apresentou nenhum warning do ThreadSanitizer e finalizou
corretamente (todas as 15 threads de veículo + relógio + semáforos +
visualização encerradas via `pthread_join`, sem deadlock).

## Integrantes e responsabilidades

| Integrante | Módulo(s) | Responsabilidade |
|---|---|---|
| _(Heberthy Samir)_ | `mapa.c` / `mapa.h` | Malha viária, células, ocupação atômica |
| _(Ramona Vitória)_ | `relogio.c` / `relogio.h` | Tick global, variável de condição |
| _(Pedro Kauan)_ | `semaforos.c` / `semaforos.h` | Sinais, prioridade da ambulância |
| _(Joaquim Arthur)_ | `veiculos.c` / `veiculos.h` | Lógica de movimento dos veículos |
| _(José Welton/Ramona)_ | `sincronizacao.c` / `main.c` | Anti-deadlock, log thread-safe, visualização |
