#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "a8.h"

// create an edge
Edge *create_edge(int target, int *weights) {
    Edge *edge = (Edge *)malloc(sizeof(Edge));
    edge->target = target;
    edge->weights = weights;
    edge->next = NULL;
    return edge;
}

// add an edge to the graph
void add_edge(Graph *graph, int src, int dest, int *weights) {
    Edge *edge = create_edge(dest, weights);
    edge->next = graph->adj[src];
    graph->adj[src] = edge;
}

// create a min heap
min_heap *create_min_heap(int capacity, int v, int n) {
    min_heap *heap = (min_heap *)malloc(sizeof(min_heap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->nodes = (heap_node *)malloc(capacity * sizeof(heap_node));
    heap->pos = (int *)malloc(v * n * sizeof(int));
    for (int i = 0; i < v * n; i++)
        heap->pos[i] = -1;
    return heap;
}

// get the position index of the node
int pos_index(int vertex, int weight_index, int n) {
    return vertex * n + weight_index;
}

// swap two nodes in the heap
void swap_heap_node(min_heap *heap, int a_index, int b_index, int n) {
    heap_node temp = heap->nodes[a_index];
    heap->nodes[a_index] = heap->nodes[b_index];
    heap->nodes[b_index] = temp;

    int a_pos_index = pos_index(heap->nodes[a_index].vertex, heap->nodes[a_index].weight_index, n);
    int b_pos_index = pos_index(heap->nodes[b_index].vertex, heap->nodes[b_index].weight_index, n);
    heap->pos[a_pos_index] = a_index;
    heap->pos[b_pos_index] = b_index;
}

// min heapify the heap
void min_heapify(min_heap *heap, int index, int n) {
    int smallest = index;
    int left = (2 * index) + 1;
    int right = (2 * index) + 2;

    if (left < heap->size && heap->nodes[left].weight < heap->nodes[smallest].weight)
        smallest = left;

    if (right < heap->size && heap->nodes[right].weight < heap->nodes[smallest].weight)
        smallest = right;

    if (smallest != index) {
        swap_heap_node(heap, smallest, index, n);
        min_heapify(heap, smallest, n);
    }
}

// extract the minimum node from the heap
heap_node extract_min(min_heap *heap, int n) {
    heap_node root = heap->nodes[0];

    int root_pos_index = pos_index(root.vertex, root.weight_index, n);
    heap->pos[root_pos_index] = -1;

    if (heap->size > 1) {
        heap->nodes[0] = heap->nodes[heap->size - 1];

        int moved_pos_index = pos_index(heap->nodes[0].vertex, heap->nodes[0].weight_index, n);
        heap->pos[moved_pos_index] = 0;

        heap->size--;
        min_heapify(heap, 0, n);
    } else {
        heap->size--;
    }

    return root;
}

// decrease the key of the node in the heap
void decrease_key(min_heap *heap, int vertex, int weight_index, int new_weight, int n) {
    int index = heap->pos[pos_index(vertex, weight_index, n)];

    heap->nodes[index].weight = new_weight;

    while (index && heap->nodes[index].weight < heap->nodes[(index - 1) / 2].weight) {
        swap_heap_node(heap, index, (index - 1) / 2, n);
        index = (index - 1) / 2;
    }
}

// insert a node into the heap
void insert_heap(min_heap *heap, int vertex, int weight_index, int weight, int n) {
    int index = heap->size++;
    heap->nodes[index].vertex = vertex;
    heap->nodes[index].weight_index = weight_index;
    heap->nodes[index].weight = weight;

    heap->pos[pos_index(vertex, weight_index, n)] = index;

    while (index && heap->nodes[index].weight < heap->nodes[(index - 1) / 2].weight) {
        swap_heap_node(heap, index, (index - 1) / 2, n);
        index = (index - 1) / 2;
    }
}

// check if the node is in the heap
int is_in_heap(min_heap *heap, int vertex, int weight_index, int n) {
    return heap->pos[pos_index(vertex, weight_index, n)] != -1;
}

// read the graph from the file
Graph *read_graph(const char *filename) {
    FILE *file = fopen(filename, "r");
    int v, n;
    fscanf(file, "%d %d", &v, &n);

    Graph *graph = (Graph *)malloc(sizeof(Graph));
    graph->v = v;
    graph->n = n;

    graph->adj = (Edge **)malloc(v * sizeof(Edge *));
    for (int i = 0; i < v; i++)
        graph->adj[i] = NULL;

    int src, dest;
    while (fscanf(file, "%d %d", &src, &dest) == 2) {
        int *weights = (int *)malloc(n * sizeof(int));
        for (int i = 0; i < n; i++) {
            fscanf(file, "%d", &weights[i]);
        }
        add_edge(graph, src, dest, weights);
    }

    fclose(file);
    return graph;
}

// dijkstra's algorithm
int *dijkstra(Graph *graph, int start, int end, int *path_length) {
    int v = graph->v;
    int n = graph->n;

    int **dist = (int **)malloc(v * sizeof(int *));
    int **prev_v = (int **)malloc(v * sizeof(int *));
    int **prev_time = (int **)malloc(v * sizeof(int *));
    for (int i = 0; i < v; i++) {
        dist[i] = (int *)malloc(n * sizeof(int));
        prev_v[i] = (int *)malloc(n * sizeof(int));
        prev_time[i] = (int *)malloc(n * sizeof(int));
        for (int t = 0; t < n; t++) {
            dist[i][t] = INT_MAX;
            prev_v[i][t] = -1;
            prev_time[i][t] = -1;
        }
    }

    min_heap *heap = create_min_heap(v * n, v, n);

    dist[start][0] = 0;
    insert_heap(heap, start, 0, 0, n);

    while (heap->size > 0) {
        heap_node min_node = extract_min(heap, n);
        int u = min_node.vertex;
        int curr_weight_index = min_node.weight_index;
        int weight_u = min_node.weight;

        for (Edge *edge = graph->adj[u]; edge != NULL; edge = edge->next) {
            int v = edge->target;
            int next_weight_index = (curr_weight_index + 1) % n;
            int w = edge->weights[curr_weight_index % n];
            int new_weight = weight_u + w;

            if (dist[v][next_weight_index] > new_weight) {
                dist[v][next_weight_index] = new_weight;
                prev_v[v][next_weight_index] = u;
                prev_time[v][next_weight_index] = curr_weight_index;

                if (is_in_heap(heap, v, next_weight_index, n)) {
                    decrease_key(heap, v, next_weight_index, new_weight, n);
                } else {
                    insert_heap(heap, v, next_weight_index, new_weight, n);
                }
            }
        }
    }

    int min_dist = INT_MAX;
    int best_weight_index = -1;
    for (int t = 0; t < n; t++) {
        if (dist[end][t] < min_dist) {
            min_dist = dist[end][t];
            best_weight_index = t;
        }
    }

    if (min_dist == INT_MAX) {
        *path_length = 0;
        for (int i = 0; i < v; i++) {
            free(dist[i]);
            free(prev_v[i]);
            free(prev_time[i]);
        }
        free(dist);
        free(prev_v);
        free(prev_time);
        free(heap->nodes);
        free(heap->pos);
        free(heap);
        return NULL;
    }

    int *temp_path = (int *)malloc(v * sizeof(int));
    int path_idx = 0;
    int u = end;
    int curr_time = best_weight_index;

    while (1) {
        temp_path[path_idx++] = u;
        if (u == start && curr_time == 0)
            break;
        int prev_u = prev_v[u][curr_time];
        int prev_weight_index = prev_time[u][curr_time];
        u = prev_u;
        curr_time = prev_weight_index;
    }

    int *path = (int *)malloc(path_idx * sizeof(int));
    *path_length = path_idx;
    for (int i = 0; i < path_idx; i++) {
        path[i] = temp_path[path_idx - i - 1];
    }

    free(temp_path);
    for (int i = 0; i < v; i++) {
        free(dist[i]);
        free(prev_v[i]);
        free(prev_time[i]);
    }
    free(dist);
    free(prev_v);
    free(prev_time);
    free(heap->nodes);
    free(heap->pos);
    free(heap);

    return path;
}

// main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        return EXIT_FAILURE;
    }
    Graph *graph = read_graph(argv[1]);

    char line[1000];
    while (fgets(line, sizeof(line), stdin)) {
        int start, end;
        sscanf(line, "%d %d", &start, &end);

        int path_length;
        int *path = dijkstra(graph, start, end, &path_length);

        if (path == NULL) {
            printf("No path from %d to %d.\n", start, end);
        } else {
            for (int i = 0; i < path_length; i++) {
                printf("%d", path[i]);
                if (i < path_length - 1)
                    printf(" ");
            }
            printf("\n");
            free(path);
        }
    }

    for (int i = 0; i < graph->v; i++) {
        Edge *edge = graph->adj[i];
        while (edge != NULL) {
            Edge *temp = edge;
            edge = edge->next;
            free(temp->weights);
            free(temp);
        }
    }
    free(graph->adj);
    free(graph);

    return EXIT_SUCCESS;
}
