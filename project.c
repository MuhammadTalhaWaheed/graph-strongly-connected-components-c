#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MASTER 0

void printWithDelay(const char *text, int delay) {
    for (int i = 0; i < strlen(text); i++) {
        putchar(text[i]);
        fflush(stdout);
        usleep(delay * 1000);
    }
}

void printGraph(int *graph, int n) {
    printf("\nGraph Visualization:\n");

    for (int i = 0; i < n; i++) {
        printf("%d ", i);

        for (int j = 0; j < n; j++) {
            if (graph[i * n + j] == 1) {
                printf("-----> %d ", j);
            }
        }

        printf("\n");
    }
}

void printAnimatedWelcome() {
    const char *welcomeText = "WELCOME TO PROJECT PARALLELIZATION OF GRAPH ALGORITHMS\n";
    int animationDelay = 100;

    printf("----------------------------------------------------------------------------------\n");
    printWithDelay(welcomeText, animationDelay);
    printf("----------------------------------------------------------------------------------\n");
}

void BFS(int start, int *graph, int *visited, int *component, int root, int n) {
    int *queue = (int *)malloc(n * sizeof(int));
    int front = 0, rear = 0;

    queue[rear++] = start;
    visited[start] = 1;
    component[start] = root;

    while (front != rear) {
        int current = queue[front++];
        for (int i = 0; i < n; i++) {
            if (!visited[i] && graph[current * n + i]) {
                visited[i] = 1;
                component[i] = root;
                queue[rear++] = i;
            }
        }
    }

    free(queue);
}


void connectedComponentsMPI(int *graph, int n, int numProcesses, int myRank) {
    int *visited = (int *)malloc(n * sizeof(int));
    int *component = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        visited[i] = 0;
        component[i] = -1;
    }

    MPI_Bcast(graph, n * n, MPI_INT, MASTER, MPI_COMM_WORLD);

    int root = 0;

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            BFS(i, graph, visited, component, root++, n);
        }
    }

    MPI_Barrier(MPI_COMM_WORLD);

    if (myRank == MASTER) {
        printf("Connected Components:\n");
        for (int i = 0; i < n; i++) {
            printf("Vertex %d is in the component with root %d\n", i, component[i]);
        }
    }

    free(visited);
    free(component);
}
void generateRandomGraph(int *graph, int n, int numRoots) {
    srand(time(NULL));

    int verticesPerRoot = n / numRoots;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int root = i / verticesPerRoot;
            graph[i * n + j] = (j / verticesPerRoot == root) ? 1 : 0;
        }
    }
}


void writeGraphToFile(int *graph, int n, const char *filename) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file for writing.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fprintf(file, "%d ", graph[i * n + j]);
        }
        fprintf(file, "\n");
    }

    fclose(file);
}

void readGraphFromFile(int *graph, int n, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        printf("Error opening file for reading.\n");
        exit(1);
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            fscanf(file, "%d", &graph[i * n + j]);
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    printAnimatedWelcome();
double startTime = MPI_Wtime();
    int numProcesses, myRank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    int n = 1024;
    int *graph = (int *)malloc(n * n * sizeof(int));

    if (myRank == MASTER) {
        generateRandomGraph(graph, n,4);
        writeGraphToFile(graph, n, "graph.txt");
    }

    MPI_Bcast(graph, n * n, MPI_INT, MASTER, MPI_COMM_WORLD);

    connectedComponentsMPI(graph, n, numProcesses, myRank);

    free(graph);
 double endTime = MPI_Wtime();

    if (myRank == MASTER) {
        printf("Parallel Time: %f seconds\n", endTime - startTime);
    }
    MPI_Finalize();
    return 0;
}

