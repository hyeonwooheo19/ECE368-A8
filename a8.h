#ifndef A8_H
#define A8_H

#include <stdio.h>
#include <stdbool.h>

// struct for the edge
typedef struct Edge {
    int target; // target vertex
    int *weights; // weights of the edge
    struct Edge *next; // next edge in the adjacency list
} Edge;

// struct for the heap node
typedef struct heap_node {
    int weight; // weight of the node
    int vertex; // vertex of the node
    int weight_index; // weight index of the node
} heap_node;

// struct for the min heap
typedef struct min_heap {
    int size; // size of the heap
    int capacity; // capacity of the heap
    heap_node *nodes; // nodes of the heap
    int *pos; // position of the nodes in the heap
} min_heap;

// struct for the graph
typedef struct Graph {
    int v; // number of vertices
    int n; // number of weights
    Edge **adj; // adjacency list of the graph
} Graph;

Edge *create_edge(int target, int *weights);
void add_edge(Graph *graph, int src, int dest, int *weights);
min_heap *create_min_heap(int capacity, int v, int n);
int pos_index(int vertex, int weight_index, int n);
void swap_heap_node(min_heap *heap, int a_index, int b_index, int n);
void min_heapify(min_heap *heap, int index, int n);
heap_node extract_min(min_heap *heap, int n);
void decrease_key(min_heap *heap, int vertex, int weight_index, int new_weight, int n);
void insert_heap(min_heap *heap, int vertex, int weight_index, int weight, int n);
int is_in_heap(min_heap *heap, int vertex, int weight_index, int n);
Graph *read_graph(const char *filename);
int *dijkstra(Graph *graph, int start, int end, int *path_length); 

#endif // A8_H