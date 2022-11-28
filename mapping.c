#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "Mapping.h"
#include "communications.h"

#define MAX (m->vertices + 1)
#define INFINITY 9999


struct Map
{
    int vertices;
    bool **path;
    bool **barcode;
    bool **hump;
    bool **edges;
    int **cost;
};

Map *create_Map(int vertices)
{
    Map *m = malloc(sizeof(*m));
    if (m == NULL)
    {
        return NULL;
    }
    m->vertices = vertices;

    // allocate memory to matrix
    m->edges = calloc(sizeof(bool *), m->vertices);
    if (m->edges == NULL)
    {
        free(m);
        return NULL;
    }

    for (int i = 0; i < m->vertices; i++)
    {
        m->edges[i] = calloc(sizeof(bool), m->vertices);
        if (m->edges[i] == NULL)
        {
            destroy_Map(m);
            return NULL;
        }
    }
    // allocate memory to direction matrix
    m->path = calloc(sizeof(bool *), m->vertices);
    if (m->path == NULL)
    {
        free(m);
        return NULL;
    }

    for (int i = 0; i < m->vertices; i++)
    {
        m->path[i] = calloc(sizeof(bool), m->vertices);
        if (m->path[i] == NULL)
        {
            destroy_Map(m);
            return NULL;
        }
    }

    // allocate memory to Hump matrix
    m->hump = calloc(sizeof(bool *), m->vertices);
    if (m->hump == NULL)
    {
        free(m);
        return NULL;
    }

    for (int i = 0; i < m->vertices; i++)
    {
        m->hump[i] = calloc(sizeof(bool), m->vertices);
        if (m->hump[i] == NULL)
        {
            destroy_Map(m);
            return NULL;
        }
    }
    // allocate memory to Barcode matrix
    m->barcode = calloc(sizeof(bool *), m->vertices);
    if (m->barcode == NULL)
    {
        free(m);
        return NULL;
    }

    for (int i = 0; i < m->vertices; i++)
    {
        m->barcode[i] = calloc(sizeof(bool), m->vertices);
        if (m->barcode[i] == NULL)
        {
            destroy_Map(m);
            return NULL;
        }
    }

    // allocate memory to cost
    m->cost = calloc(sizeof(bool *), m->vertices);
    if (m->cost == NULL)
    {
        free(m);
        return NULL;
    }
    for (int i = 0; i < m->vertices; i++)
    {
        m->cost[i] = calloc(sizeof(bool), m->vertices);
        if (m->cost[i] == NULL)
        {
            destroy_Map(m);
            return NULL;
        }
    }
    return m;
}

void destroy_Map(Map *m)
{
    /* free Up the vertices matrix memory*/
    if (m->edges == NULL)
    {
        return;
    }
    for (int i = 0; i < m->vertices; i++)
    {
        if (m->edges[i] != NULL)
        {
            free(m->edges[i]);
        }
    }
    /* free up Direction matrix memory*/
    if (m->path == NULL)
    {
        return;
    }
    for (int i = 0; i < m->vertices; i++)
    {
        if (m->path[i] != NULL)
        {
            free(m->path[i]);
        }
    }

    /* free up Hump matrix memory*/
    if (m->hump == NULL)
    {
        return;
    }
    for (int i = 0; i < m->vertices; i++)
    {
        if (m->hump[i] != NULL)
        {
            free(m->hump[i]);
        }
    }
    /* free up barcode matrix memory*/
    if (m->barcode == NULL)
    {
        return;
    }
    for (int i = 0; i < m->vertices; i++)
    {
        if (m->barcode[i] != NULL)
        {
            free(m->barcode[i]);
        }
    }
    /* free up cost memory*/
    if (m->cost == NULL)
    {
        return;
    }
    for (int i = 0; i < m->vertices; i++)
    {
        if (m->cost[i] != NULL)
        {
            free(m->cost[i]);
        }
    }
    free(m->edges);
    free(m->path);
    free(m->hump);
    free(m->barcode);
    free(m->cost);
}

/* Create a dot format Graph*/
void print_Map(Map *m)
{
    uartprintf("This prints the Edges between each node, which shows the available path from each node.");
	readyPrintf();
    long_printf("digraph {\n");

    for (int from = 0; from < m->vertices; from++)
    {
        for (int to = 0; to < m->vertices; to++)
        {
            if (m->edges[from][to])
            {
                long_printf("%d -> %d\n", from, to);
            }
        }
    }
    long_printf("}\n");
	stopPrintf();
}

/* This is to print out the edges(available path) in a table format*/
void print_Edge(Map *m, int row, int columns)
{
	readyPrintf();
    long_printf("\nEdge Map: x represents available route , Blank means either blocked or not mapped.\n\n");
    int nodes = 0;
    int xnode = 1;
    for (int x = 0; x < row; x++)
    {
        for (int y = 0; y < columns; y++)
        {
            if (y < columns)
            {
                if (m->edges[nodes][nodes + 1])
                {
                    long_printf(" x");
                }
                else if (m->edges[nodes + 1][nodes])
                {
                    long_printf(" x");
                }
                else
                {
                    long_printf("  ");
                }
            }
            long_printf("%3d", nodes + 1);
            nodes++;
            xnode++;
        }
        long_printf("\n");
        for (int z = 0; z < columns; z++)
        {
            if ((z < columns) && (xnode <= row * columns))
            {
                if (m->edges[xnode - columns][xnode])
                {
                    long_printf(" %4s", " x");
                }
                else if (m->edges[xnode][xnode - columns])
                {
                    long_printf(" %4s", " x");
                }
                else
                {
                    long_printf("%5s", " ");
                }
            }
            xnode++;
        }
        xnode -= columns;

        long_printf("\n");
    }
	stopPrintf();
}


void print_Barcode(Map *m, int row, int columns)
{
	readyPrintf();
    long_printf("\nBarcode Map: B represents Barcode in the map.\n\n");
    int nodes = 0;
    int xnode = 1;
    for (int x = 0; x < row; x++)
    {
        for (int y = 0; y < columns; y++)
        {
            if (y < columns)
            {
                if (m->barcode[nodes][nodes + 1])
                {
                    long_printf(" B");
                }
                else if (m->barcode[nodes + 1][nodes])
                {
                    long_printf(" B");
                }
                else
                {
                    long_printf("  ");
                }
            }
            long_printf("%3d", nodes + 1);
            nodes++;
            xnode++;
        }
        long_printf("\n");
        for (int z = 0; z < columns; z++)
        {
            if ((z < columns) && (xnode <= row * columns))
            {
                if (m->barcode[xnode - columns][xnode])
                {
                    long_printf(" %4s", " B");
                }
                else if (m->barcode[xnode][xnode - columns])
                {
                    long_printf(" %4s", " B");
                }
                else
                {
                    long_printf("%5s", "  ");
                }
            }
            xnode++;
        }
        xnode -= columns;

        long_printf("\n");
    }
	stopPrintf();
}

void print_Hump(Map *m, int row, int columns)
{
	readyPrintf();
    long_printf("\nHump Map:\n\n");
    int nodes = 0;
    int xnode = 1;
    for (int x = 0; x < row; x++)
    {
        for (int y = 0; y < columns; y++)
        {
            if (y < columns)
            {
                if (m->hump[nodes][nodes + 1])
                {
                    long_printf(" H");
                }
                else if (m->hump[nodes + 1][nodes])
                {
                    long_printf(" H");
                }
                else
                {
                    long_printf("  ");
                }
            }
            long_printf("%3d", nodes + 1);
            nodes++;
            xnode++;
        }
        long_printf("\n");
        for (int z = 0; z < columns; z++)
        {
            if ((z < columns) && (xnode <= row * columns))
            {
                if (m->hump[xnode - columns][xnode])
                {
                    long_printf(" %4s", " H");
                }
                else if (m->hump[xnode][xnode - columns])
                {
                    long_printf(" %4s", " H");
                }
                else
                {
                    long_printf("%5s", " ");
                }
            }
            xnode++;
        }
        xnode -= columns;

        long_printf("\n");
    }
	stopPrintf();
}

void print_Path(Map *m, int row, int columns)
{
	readyPrintf();
    long_printf("\nPath Map:\n\n");
    int nodes = 0;
    int xnode = 1;
    for (int x = 0; x < row; x++)
    {
        for (int y = 0; y < columns; y++)
        {
            if (y < columns)
            {
                if (m->path[nodes][nodes + 1])
                {
                    long_printf(" >");
                }
                else if (m->path[nodes + 1][nodes])
                {
                    long_printf(" <");
                }
                else
                {
                    long_printf("  ");
                }
            }
            long_printf("%3d", nodes + 1);
            nodes++;
            xnode++;
        }
        long_printf("\n");
        for (int z = 0; z < columns; z++)
        {
            if ((z < columns) && (xnode <= row * columns))
            {
                if (m->path[xnode - columns][xnode])
                {
                    long_printf(" %4s", "v");
                }
                else if (m->path[xnode][xnode - columns])
                {
                    long_printf(" %4s", "^");
                }
                else
                {
                    long_printf("%5s", " ");
                }
            }
            xnode++;
        }
        xnode -= columns;
        long_printf("\n");
    }
	stopPrintf();
}

bool add_Path(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);
    if (has_Path(m, from_node, to_node))
    { // if it already have path , return false
        return false;
    }

    m->path[from_node][to_node] = true;
    return true;
}

bool has_Path(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);

    if (m->path[from_node][to_node] || m->path[to_node][from_node])
    { // undirected graph
        return true;
    }
    else
    {
        return false;
    }
}

bool add_edges(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);
    if (has_edge(m, from_node, to_node))
    { // if it already have edges , return false
        return false;
    }

    m->edges[from_node][to_node] = true;
    m->cost[from_node][to_node] = 0;
    return true;
}

bool has_edge(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);

    if (m->edges[from_node][to_node] || m->edges[to_node][from_node])
    { // undirected graph
        return true;
    }
    else
    {
        return false;
    }
}

bool add_barcode(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);
    if (has_barcode(m, from_node, to_node))
    { // if it already have barcode , return false
        return false;
    }

    m->barcode[from_node][to_node] = true;
    return true;
}

bool has_barcode(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);

    if (m->barcode[from_node][to_node] || m->barcode[to_node][from_node])
    { // undirected graph
        return true;
    }
    else
    {
        return false;
    }
}

bool add_hump(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);
    if (has_hump(m, from_node, to_node))
    { // if it already have barcode , return false
        return false;
    }

    m->hump[from_node][to_node] = true;
    add_cost(m, from_node, to_node, 1); // if there is hump on the path , cost of the edge = 1
    return true;
}

bool has_hump(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);

    if (m->hump[from_node][to_node] || m->hump[to_node][from_node])
    { // undirected graph
        return true;
    }
    else
    {
        return false;
    }
}

void add_cost(Map *m, unsigned int from_node, unsigned int to_node, int cost)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);

    if (has_edge(m, from_node, to_node))
    {
        m->cost[from_node][to_node] = cost;
        m->cost[to_node][from_node] = cost;
    }
}

int Find_cost(Map *m, unsigned int from_node, unsigned int to_node)
{
    assert(m != NULL);
    assert(from_node < m->vertices);
    assert(to_node <= m->vertices);
    int edge_cost = 0;

    if (has_edge(m, from_node, to_node))
    {
        edge_cost = m->cost[from_node][to_node];
    }

    return edge_cost;
}

bool map_complete(Map *m, int row,
                  int columns)
{ // check if all vertices have at least 1 edge & prints false if not completed
	readyPrintf();
    int node = 1;
    bool checked = true;
    while (node != (row * columns) + 1)
    {
        if (node - 1 == 0)
        { // first node (0)
            if (has_edge(m, node, node + 1) || has_edge(m, node, node + columns))
            {
                node++;
            }
            else
            {
                long_printf("Node %d failed\n", node);
                checked = false;
                break;
            }
        }
        else if (node < columns)
        { // first row nodes
            if (has_edge(m, node, node + 1) || has_edge(m, node, node + columns) || has_edge(m, node, node - 1))
            {
                node++;
            }
            else
            {
                long_printf("Node %d failed\n", node);
                checked = false;
                break;
            }
        }
        else if ((node - 1) % columns == 0)
        { // first node of every row (except 1st row)
            if (has_edge(m, node, node + 1) || has_edge(m, node, node - columns))
            {
                node++;
            }
            else
            {
                checked = false;
                break;
            }
        }
        else if (node + columns <= (row * columns))
        { // anywhere but last row
            if (has_edge(m, node, node + 1) || has_edge(m, node, node + columns) || has_edge(m, node, node - columns) ||
                has_edge(m, node, node - 1))
            {
                node++;
            }
            else
            {
                long_printf("Node %d failed\n", node);
                checked = false;
                break;
            }
        }
        else if (node == (row * columns))
        { // last node
            if (has_edge(m, node, node - 1) || has_edge(m, node, node - columns))
            {
                node++;
            }
            else
            {
                long_printf("Node %d failed\n", node);
                checked = false;
                break;
            }
        }
        else
        { // last row
            if (has_edge(m, node, node + 1) || has_edge(m, node, node - columns) || has_edge(m, node, node - 1))
            {
                node++;
            }
            else
            {
                long_printf("Node %d failed\n", node);
                checked = false;
                break;
            }
        }
    }
	stopPrintf();
    if (checked == false)
    {
        return false;
    }
    else
    {
        return true;
    }
}


void dijkstra(Map *m, int G[MAX][MAX], int n, int startnode, int dest);

void start_dijkstra(Map *m, int n, int startnode, int dest)
{
    int i, j, G[MAX][MAX];
    // Create Cost matrix;
    for (i = 0; i < m->vertices; i++)
    { 
        for (j = 0; j < m->vertices; j++)
        {
            if (i != j)
            {
                G[i][j] = Find_cost(m, i, j);
            }
            else
            {
                G[i][j] = 0;
            }
        }
    }
    dijkstra(m, G, n, startnode, dest);
}

void dijkstra(Map *m, int G[MAX][MAX], int n, int startnode, int dest)
{
    int cost[MAX][MAX], distance[MAX], pred[MAX];
    int visited[MAX], count, mindistance, nextnode, i, j;

    // pred[] stores the predecessor of each node
    // count gives the number of nodes seen so far
    // create the cost matrix
    for (i = 0; i <= n; i++)
    {
        for (j = 0; j <= n; j++)
        {
            if (has_edge(m, i, j) == false)
            {
                cost[i][j] = INFINITY;
            }
            else
            {
                cost[i][j] = G[i][j];
            }
        }
    }
    // initialize pred[],distance[] and visited[]
    for (i = 0; i <= n; i++)
    {
        distance[i] = cost[startnode][i];
        pred[i] = startnode;
        visited[i] = 0;
    }

    distance[startnode] = 0;
    visited[startnode] = 1;
    count = 1;

    while (count < n - 1)
    {
        mindistance = INFINITY;

        // nextnode gives the node at minimum distance
        for (i = 0; i <= n; i++)
            if (distance[i] < mindistance && !visited[i])
            {
                mindistance = distance[i];
                nextnode = i;
            }

        // check if a better path exists through nextnode
        visited[nextnode] = 1;
        for (i = 0; i <= n; i++)
            if (!visited[i])
                if (mindistance + cost[nextnode][i] < distance[i])
                {
                    distance[i] = mindistance + cost[nextnode][i];
                    pred[i] = nextnode;
                }
        count++;
    }

    // print the path and distance
    uartprintf("\nCost taken to reach destination %d: %d", dest, distance[dest]);
    uartprintf("\nPath: %d", dest);

    j = dest;
	readyPrintf();
    do
    {
        j = pred[j];
        long_printf(" <- %d", j);

    } while (j != startnode);
	stopPrintf();
    j = dest;
    add_Path(m, pred[j], dest);

    j = dest;
    int currentnode = startnode;
    do
    {
        j = pred[j];

        if ((has_Path(m, currentnode, j) || has_Path(m, j, currentnode) || currentnode == startnode) == false)
        {
            add_Path(m, j, currentnode); // adding into path
        }
        currentnode = j;
    } while (j != startnode);
}