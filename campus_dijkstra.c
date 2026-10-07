/*
 * PBLE 2: Campus Shortest Route Finder - Dijkstra's Algorithm
 *
 * - Graph stored as an adjacency matrix (undirected, non-negative weights)
 * - Dijkstra implemented with plain arrays/loops (greedy method)
 * - parent[] array is used to reconstruct each shortest path
 *
 * TIME COMPLEXITY: O(V^2)
 *   The outer loop runs V times (one vertex is finalised per iteration).
 *   Each iteration (a) scans all V vertices to find the unvisited vertex with
 *   the minimum distance -> O(V), and (b) scans one matrix row to relax all
 *   neighbours -> O(V).  Total = V * (V + V) = O(V^2).
 *   Space complexity: O(V^2) for the adjacency matrix.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX      20      /* maximum number of locations          */
#define INF      9999    /* "no direct road" / "unreachable"     */
#define NAME_LEN 40

/* ---------------- Global data ---------------- */
int  n = 0;                         /* number of locations              */
int  graph[MAX][MAX];               /* adjacency matrix                 */
char names[MAX][NAME_LEN];          /* location names                   */
int  dist[MAX];                     /* shortest distance from source    */
int  parent[MAX];                   /* predecessor on shortest path     */
int  visited[MAX];                  /* 1 once distance is finalised     */
int  source = -1;                   /* selected source (0-based)        */
int  graphEntered = 0;              /* has a graph been entered?        */
int  computed = 0;                  /* has Dijkstra been run for source */

/* ---------------- Input helpers ---------------- */

/* Read a whole line safely and strip the newline. */
void readLine(char *buf, int size)
{
    if (fgets(buf, size, stdin) == NULL) {
        /* End of input (no keyboard attached, Ctrl+D / Ctrl+Z, closed pipe):
           stop instead of looping forever on the menu. */
        printf("\nNo more input available. Exiting program.\n");
        exit(0);
    }
    buf[strcspn(buf, "\n")] = '\0';
}

/* Read an integer; returns 1 on success, 0 on invalid input. */
int readInt(int *value)
{
    char line[64];
    readLine(line, sizeof(line));
    return sscanf(line, "%d", value) == 1;
}

/* ---------------- Graph handling ---------------- */

void clearGraph(void)
{
    int i, j;
    for (i = 0; i < MAX; i++)
        for (j = 0; j < MAX; j++)
            graph[i][j] = (i == j) ? 0 : INF;   /* 0 diagonal, INF elsewhere */
    computed = 0;
    source = -1;
}

/* Load the sample campus from the problem statement. */
void loadSampleGraph(void)
{
    const char *sample[] = { "Main Gate", "Library", "Computer Department",
                             "Laboratory", "Auditorium" };
    /* {from, to, distance} using 1-based location numbers */
    int roads[][3] = { {1,2,4}, {1,4,10}, {2,3,3}, {2,4,5}, {3,5,5}, {4,5,6} };
    int i, count = 6;

    n = 5;
    clearGraph();
    for (i = 0; i < n; i++)
        strcpy(names[i], sample[i]);
    for (i = 0; i < count; i++) {
        int u = roads[i][0] - 1, v = roads[i][1] - 1, w = roads[i][2];
        graph[u][v] = graph[v][u] = w;           /* roads are two-way */
    }
    graphEntered = 1;
    printf("\nSample campus graph loaded (5 locations, %d roads).\n", count);
}

void enterGraph(void)
{
    int i, roads, u, v, w, choice;

    printf("\nLoad the sample campus graph? (1 = Yes, 0 = Enter my own): ");
    if (readInt(&choice) && choice == 1) {
        loadSampleGraph();
        return;
    }

    printf("Enter number of locations (1-%d): ", MAX);
    if (!readInt(&n) || n < 1 || n > MAX) {
        printf("Invalid number of locations.\n");
        n = 0;
        graphEntered = 0;
        return;
    }

    clearGraph();
    graphEntered = 0;               /* becomes 1 only after successful entry */
    for (i = 0; i < n; i++) {
        printf("Enter name of location %d: ", i + 1);
        readLine(names[i], NAME_LEN);
        if (names[i][0] == '\0')
            sprintf(names[i], "Location %d", i + 1);
    }

    printf("Enter number of roads: ");
    if (!readInt(&roads) || roads < 0) {
        printf("Invalid number of roads.\n");
        return;
    }

    for (i = 0; i < roads; i++) {
        printf("Road %d - enter  from  to  distance : ", i + 1);
        char line[100];
        readLine(line, sizeof(line));
        if (sscanf(line, "%d %d %d", &u, &v, &w) != 3 ||
            u < 1 || u > n || v < 1 || v > n) {
            printf("  Invalid location numbers. Re-enter this road.\n");
            i--;
            continue;
        }
        if (w < 0) {
            printf("  Distance must be non-negative. Re-enter this road.\n");
            i--;
            continue;
        }
        if (u == v) {
            printf("  A road cannot connect a location to itself. Re-enter.\n");
            i--;
            continue;
        }
        graph[u - 1][v - 1] = graph[v - 1][u - 1] = w;   /* undirected */
    }
    graphEntered = 1;
    printf("Graph entered successfully.\n");
}

void displayLocations(void)
{
    int i;
    printf("\nNo.  Location\n");
    for (i = 0; i < n; i++)
        printf("%-4d %s\n", i + 1, names[i]);
}

void displayMatrix(void)
{
    int i, j;
    printf("\nAdjacency Matrix (INF = %d means no direct road)\n\n", INF);
    printf("%6s", "");
    for (j = 0; j < n; j++)
        printf("%6d", j + 1);
    printf("\n");
    for (i = 0; i < n; i++) {
        printf("%6d", i + 1);
        for (j = 0; j < n; j++) {
            if (graph[i][j] == INF)
                printf("%6s", "INF");
            else
                printf("%6d", graph[i][j]);
        }
        printf("\n");
    }
    displayLocations();
}

void selectSource(void)
{
    int choice;
    displayLocations();
    printf("\nEnter source location number (1-%d): ", n);
    if (!readInt(&choice) || choice < 1 || choice > n) {
        printf("Invalid source location.\n");
        return;
    }
    source = choice - 1;
    computed = 0;                    /* old results no longer valid */
    printf("Source selected: %s\n", names[source]);
}

/* ---------------- Dijkstra's Algorithm ---------------- */

void printDistArray(void)
{
    int i;
    printf("   dist[] = { ");
    for (i = 0; i < n; i++) {
        if (dist[i] == INF) printf("INF ");
        else                printf("%d ", dist[i]);
    }
    printf("}\n");
}

void dijkstra(void)
{
    int i, count, u, v, minDist;

    /* Step 1: initialise */
    for (i = 0; i < n; i++) {
        dist[i]    = INF;
        parent[i]  = -1;
        visited[i] = 0;
    }
    dist[source] = 0;

    printf("\n--- Dijkstra's Algorithm: source = %s ---\n", names[source]);
    printf("Initial:\n");
    printDistArray();

    /* Step 2: repeat n times, finalising one vertex each time (greedy) */
    for (count = 0; count < n; count++) {

        /* pick the unvisited vertex with the smallest tentative distance */
        u = -1;
        minDist = INF;
        for (i = 0; i < n; i++) {
            if (!visited[i] && dist[i] < minDist) {
                minDist = dist[i];
                u = i;
            }
        }
        if (u == -1)                 /* remaining vertices are unreachable */
            break;

        visited[u] = 1;
        printf("\nIteration %d: pick %s (distance %d)\n",
               count + 1, names[u], dist[u]);

        /* relax every edge (u, v) */
        for (v = 0; v < n; v++) {
            if (!visited[v] && graph[u][v] != INF &&
                dist[u] + graph[u][v] < dist[v]) {
                dist[v]   = dist[u] + graph[u][v];
                parent[v] = u;
                printf("   Updated %s: distance = %d (via %s)\n",
                       names[v], dist[v], names[u]);
            }
        }
        printDistArray();
    }
    computed = 1;
}

/* Recursively print the path from the source to vertex v. */
void printPath(int v)
{
    if (parent[v] == -1) {
        printf("%s", names[v]);
        return;
    }
    printPath(parent[v]);
    printf(" -> %s", names[v]);
}

void findShortestDistance(void)
{
    dijkstra();
    printf("\nShortest distances computed from %s.\n", names[source]);
    printf("Time complexity: O(V^2)  (V iterations x O(V) minimum search + O(V) relaxation).\n");
}

void displayPaths(void)
{
    int i;
    printf("\nSource: %s\n\n", names[source]);
    printf("%-22s %-10s %s\n", "Destination", "Distance", "Shortest Path");
    printf("------------------------------------------------------------------\n");
    for (i = 0; i < n; i++) {
        if (i == source)
            continue;
        printf("%-22s ", names[i]);
        if (dist[i] == INF) {
            printf("%-10s %s\n", "INF", "No path (unreachable)");
        } else {
            printf("%-10d ", dist[i]);
            printPath(i);
            printf("\n");
        }
    }
}

void displayDistances(void)
{
    int i;
    printf("\nSource: %s\n\n", names[source]);
    printf("%-4s %-22s %s\n", "No.", "Location", "Distance from Source");
    printf("----------------------------------------------\n");
    for (i = 0; i < n; i++) {
        printf("%-4d %-22s ", i + 1, names[i]);
        if (dist[i] == INF) printf("INF (unreachable)\n");
        else                printf("%d\n", dist[i]);
    }
}

/* ---------------- Main menu ---------------- */

void showMenu(void)
{
    printf("\n========== Campus Shortest Route Finder ==========\n");
    printf(" 1. Enter Campus Graph\n");
    printf(" 2. Display Adjacency Matrix\n");
    printf(" 3. Select Source Location\n");
    printf(" 4. Find Shortest Distance\n");
    printf(" 5. Display Shortest Paths\n");
    printf(" 6. Display Distance from Source to All Locations\n");
    printf(" 7. Exit\n");
    printf("==================================================\n");
    printf("Enter your choice: ");
}

int main(void)
{
    int choice;

    clearGraph();

    while (1) {
        showMenu();
        if (!readInt(&choice)) {
            printf("Invalid input. Please enter a number between 1 and 7.\n");
            continue;
        }

        switch (choice) {
        case 1:
            enterGraph();
            break;
        case 2:
            if (!graphEntered) printf("\nPlease enter the graph first (option 1).\n");
            else               displayMatrix();
            break;
        case 3:
            if (!graphEntered) printf("\nPlease enter the graph first (option 1).\n");
            else               selectSource();
            break;
        case 4:
            if (!graphEntered)      printf("\nPlease enter the graph first (option 1).\n");
            else if (source == -1)  printf("\nPlease select a source first (option 3).\n");
            else                    findShortestDistance();
            break;
        case 5:
            /* validate graph, source and result before displaying */
            if (!graphEntered)     printf("\nPlease enter the graph first (option 1).\n");
            else if (source == -1) printf("\nPlease select a source first (option 3).\n");
            else if (!computed)    printf("\nPlease run option 4 first for the selected source.\n");
            else                   displayPaths();
            break;
        case 6:
            /* validate graph, source and result before displaying */
            if (!graphEntered)     printf("\nPlease enter the graph first (option 1).\n");
            else if (source == -1) printf("\nPlease select a source first (option 3).\n");
            else if (!computed)    printf("\nPlease run option 4 first for the selected source.\n");
            else                   displayDistances();
            break;
        case 7:
            printf("\nExiting program. Goodbye!\n");
            return 0;
        default:
            printf("\nInvalid choice! Please enter a number between 1 and 7.\n");
        }
    }
    return 0;
}