#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

void printWithDelay(const char *text, int delay) {
    for (int i = 0; i < strlen(text); i++) {
        putchar(text[i]);
        fflush(stdout);
        usleep(delay * 1000);
    }
}

void printGraph(int graph[], int n) {
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

void BFS(int start, int *graph, int *visited, int *component, int root, int n) {
    int *queue = (int *)malloc(n * sizeof(int));
    int front = -1, rear = -1;

    visited[start] = 1;
    component[start] = root;
    queue[++rear] = start;

    while (front != rear) {
        int vertex = queue[++front];

        for (int i = 0; i < n; i++) {
            if (!visited[i] && graph[vertex * n + i]) {
                visited[i] = 1;
                component[i] = root;
                queue[++rear] = i;
            }
        }
    }

    free(queue);
}

void findConnectedComponents(int *graph, int n) {
    int *visited = (int *)malloc(n * sizeof(int));
    int *component = (int *)malloc(n * sizeof(int));

    for (int i = 0; i < n; i++) {
        visited[i] = 0;
        component[i] = -1;
    }

    for (int i = 0; i < n; i++) {
        if (!visited[i]) {
            BFS(i, graph, visited, component, i, n);
        }
    }

    printf("Connected Components:\n");
    for (int i = 0; i < n; i++) {
        printf("Vertex %d is in the component with root %d\n", i, component[i]);
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
int main() {
    printWithDelay("WELCOME TO PROJECT PARALLELIZATION OF GRAPH ALGORITHMS\n", 100);

    int n = 1024;
    int numRoots = 4;

    int *graph = (int *)malloc(n * n * sizeof(int));

    clock_t start = clock();

    generateRandomGraph(graph, n, numRoots);

    //printGraph(graph, n);

    findConnectedComponents(graph, n);

    clock_t end = clock();
    double sequentialTime = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("Sequential Time: %f seconds\n", sequentialTime);

    free(graph);

    return 0;
}

