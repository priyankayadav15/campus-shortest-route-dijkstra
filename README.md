# Campus Shortest Route Finder (Dijkstra's Algorithm)

Menu-driven C program that finds the shortest distance and path from a
selected source to every location on a campus, using an adjacency matrix
and Dijkstra's algorithm.

## Run
    gcc campus_dijkstra.c -o campus
    ./campus

## Time Complexity
O(V^2): V iterations, each with an O(V) minimum search and O(V) relaxation.

## Sample Output
Source: Main Gate
| Destination | Distance | Path |
|---|---|---|
| Library | 4 | Main Gate -> Library |
| Computer Department | 7 | Main Gate -> Library -> Computer Department |
| Laboratory | 9 | Main Gate -> Library -> Laboratory |
| Auditorium | 12 | Main Gate -> Library -> Computer Department -> Auditorium |