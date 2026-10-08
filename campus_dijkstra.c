/*
 * PBLE 2: Campus Shortest Route Finder - Dijkstra's Algorithm
 * Adjacency matrix + greedy method + parent[] array to rebuild paths.
 *
 * TIME COMPLEXITY: O(V^2)
 *   The outer loop runs V times (one vertex finalised each time).
 *   Each time: finding the nearest unvisited vertex = O(V),
 *   relaxing its neighbours (one matrix row)         = O(V).
 *   Total = V * (V + V) = O(V^2).    SPACE: O(V^2) for the matrix.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#define MAX 20
#define INF 9999

int  n, src, step;   /* step: 0 = nothing, 1 = graph entered, 2 = source chosen, 3 = Dijkstra done */
int  g[MAX][MAX], dist[MAX], parent[MAX], visited[MAX];
char name[MAX][40];

/* Read an integer. Returns -1 for bad input; exits if input has ended. */
int num(void)
{
    int x;
    if (scanf("%d", &x) != 1) {
        if (feof(stdin)) { printf("\nNo more input available. Exiting program.\n"); exit(0); }
        scanf("%*[^\n]");                       /* throw away the bad line */
        return -1;
    }
    return x;
}

void listLocations(void)
{
    int i;
    printf("\nNo.  Location\n");
    for (i = 0; i < n; i++) printf("%-4d %s\n", i + 1, name[i]);
}

/* Option 1: enter the graph (sample or own) */
void enterGraph(void)
{
    char *sample[] = {"Main Gate", "Library", "Computer Department", "Laboratory", "Auditorium"};
    int roads[][3] = {{1,2,4}, {1,4,10}, {2,3,3}, {2,4,5}, {3,5,5}, {4,5,6}};
    int i, j, r, u, v, w;

    step = 0;
    printf("\nLoad the sample campus graph? (1 = Yes, 0 = Enter my own): ");
    if (num() == 1) {
        n = 5;
        for (i = 0; i < n; i++) strcpy(name[i], sample[i]);
    } else {
        printf("Enter number of locations (1-%d): ", MAX);
        n = num();
        if (n < 1 || n > MAX) { printf("Invalid number of locations.\n"); n = 0; return; }
        for (i = 0; i < n; i++) {
            printf("Enter name of location %d: ", i + 1);
            scanf(" %39[^\n]", name[i]);
        }
    }

    for (i = 0; i < n; i++)                     /* 0 on diagonal, INF elsewhere */
        for (j = 0; j < n; j++) g[i][j] = (i == j) ? 0 : INF;

    if (n == 5 && !strcmp(name[0], "Main Gate") && !strcmp(name[4], "Auditorium")) {
        for (i = 0; i < 6; i++)                 /* sample roads (two-way) */
            g[roads[i][0]-1][roads[i][1]-1] = g[roads[i][1]-1][roads[i][0]-1] = roads[i][2];
        step = 1;
        printf("\nSample campus graph loaded (5 locations, 6 roads).\n");
        return;
    }

    printf("Enter number of roads: ");
    r = num();
    if (r < 0) { printf("Invalid number of roads.\n"); return; }
    for (i = 0; i < r; i++) {
        printf("Road %d - enter  from  to  distance : ", i + 1);
        if (scanf("%d %d %d", &u, &v, &w) != 3) {
            if (feof(stdin)) { printf("\nNo more input available. Exiting program.\n"); exit(0); }
            scanf("%*[^\n]");
            u = 0;                              /* forces the error below */
        }
        if (u < 1 || u > n || v < 1 || v > n || u == v || w < 0) {
            printf("  Invalid road (check numbers, no self-loop, distance >= 0). Re-enter.\n");
            i--;
        } else
            g[u-1][v-1] = g[v-1][u-1] = w;      /* two-way road */
    }
    step = 1;
    printf("Graph entered successfully.\n");
}

/* Option 2 */
void showMatrix(void)
{
    int i, j;
    printf("\nAdjacency Matrix (INF = %d means no direct road)\n\n%6s", INF, "");
    for (j = 0; j < n; j++) printf("%6d", j + 1);
    for (i = 0; i < n; i++) {
        printf("\n%6d", i + 1);
        for (j = 0; j < n; j++) {
            if (g[i][j] == INF) printf("%6s", "INF");
            else                printf("%6d", g[i][j]);
        }
    }
    printf("\n");
    listLocations();
}

/* Option 3 */
void selectSource(void)
{
    int c;
    listLocations();
    printf("\nEnter source location number (1-%d): ", n);
    c = num();
    if (c < 1 || c > n) { printf("Invalid source location.\n"); return; }
    src = c - 1;
    step = 2;                                   /* old results are no longer valid */
    printf("Source selected: %s\n", name[src]);
}

void showDist(void)
{
    int i;
    printf("   dist[] = { ");
    for (i = 0; i < n; i++) {
        if (dist[i] == INF) printf("INF ");
        else                printf("%d ", dist[i]);
    }
    printf("}\n");
}

/* Option 4: Dijkstra's algorithm */
void dijkstra(void)
{
    int i, k, u, v, min;

    for (i = 0; i < n; i++) { dist[i] = INF; parent[i] = -1; visited[i] = 0; }
    dist[src] = 0;
    printf("\n--- Dijkstra's Algorithm: source = %s ---\nInitial:\n", name[src]);
    showDist();

    for (k = 0; k < n; k++) {
        u = -1;                                 /* pick nearest unvisited vertex */
        min = INF;
        for (i = 0; i < n; i++)
            if (!visited[i] && dist[i] < min) { min = dist[i]; u = i; }
        if (u == -1) break;                     /* the rest are unreachable */
        visited[u] = 1;
        printf("\nIteration %d: pick %s (distance %d)\n", k + 1, name[u], dist[u]);

        for (v = 0; v < n; v++)                 /* relax every edge (u, v) */
            if (!visited[v] && g[u][v] != INF && dist[u] + g[u][v] < dist[v]) {
                dist[v] = dist[u] + g[u][v];
                parent[v] = u;
                printf("   Updated %s: distance = %d (via %s)\n", name[v], dist[v], name[u]);
            }
        showDist();
    }
    step = 3;
    printf("\nShortest distances computed from %s.\n", name[src]);
    printf("Time complexity: O(V^2)  (V iterations x O(V) minimum search + O(V) relaxation).\n");
}

void printPath(int v)                           /* follow parent[] back to the source */
{
    if (parent[v] != -1) { printPath(parent[v]); printf(" -> "); }
    printf("%s", name[v]);
}

/* Option 5 */
void showPaths(void)
{
    int i;
    printf("\nSource: %s\n\n%-22s %-10s %s\n", name[src], "Destination", "Distance", "Shortest Path");
    printf("------------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        if (i == src) continue;
        printf("%-22s ", name[i]);
        if (dist[i] == INF) printf("%-10s No path (unreachable)\n", "INF");
        else { printf("%-10d ", dist[i]); printPath(i); printf("\n"); }
    }
}

/* Option 6 */
void showDistances(void)
{
    int i;
    printf("\nSource: %s\n\n%-4s %-22s %s\n", name[src], "No.", "Location", "Distance from Source");
    printf("----------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("%-4d %-22s ", i + 1, name[i]);
        if (dist[i] == INF) printf("INF (unreachable)\n");
        else                printf("%d\n", dist[i]);
    }
}

/* Is the program ready for an option that needs 'level' steps done? */
int ready(int level)
{
    char *msg[] = {"enter the graph first (option 1).",
                   "select a source first (option 3).",
                   "run option 4 first for the selected source."};
    if (step >= level) return 1;
    printf("\nPlease %s\n", msg[step]);
    return 0;
}

int main(void)
{
    int c;
    while (1) {
        printf("\n========== Campus Shortest Route Finder ==========\n"
               " 1. Enter Campus Graph\n 2. Display Adjacency Matrix\n"
               " 3. Select Source Location\n 4. Find Shortest Distance\n"
               " 5. Display Shortest Paths\n"
               " 6. Display Distance from Source to All Locations\n 7. Exit\n"
               "==================================================\n"
               "Enter your choice: ");
        c = num();
        if      (c == 1) enterGraph();
        else if (c == 2) { if (ready(1)) showMatrix(); }
        else if (c == 3) { if (ready(1)) selectSource(); }
        else if (c == 4) { if (ready(2)) dijkstra(); }
        else if (c == 5) { if (ready(3)) showPaths(); }
        else if (c == 6) { if (ready(3)) showDistances(); }
        else if (c == 7) { printf("\nExiting program. Goodbye!\n"); return 0; }
        else printf("\nInvalid choice! Please enter a number between 1 and 7.\n");
    }
}