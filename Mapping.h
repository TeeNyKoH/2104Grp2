#ifndef MAPPING
#define MAPPING

/* Includes */
#include <stdbool.h>


/*Struct*/
typedef struct Map Map;

/*Functions*/
Map *create_Map(int vertices);
void destroy_Map(Map* m);
void print_Map(Map *m); //Dot Format
void print_Edge(Map *m,int row,int columns);
void print_Barcode(Map *m, int row, int columns);
void print_Hump(Map *m,int row, int columns);
void print_Path(Map *m, int row, int columns);// get path of car , keeps track of where the car went.
bool add_Path(Map *m,unsigned int from_node, unsigned int to_node);
bool has_Path(Map *m, unsigned int from_node, unsigned int to_node);
bool add_edges(Map *m, unsigned int  from_node, unsigned int to_node); //add available path/edges from the node
bool has_edge(Map *m, unsigned int from_node, unsigned int to_node); //check for existing edges.
bool add_barcode(Map *m, unsigned int  from_node, unsigned int to_node); // get barcode value from barcode.
bool has_barcode(Map *m,unsigned int from_node, unsigned int to_node); //check whether there is already a barcode
bool add_hump(Map *m,unsigned int from_node, unsigned int to_node);// get hump value from UltraSonic team.
bool has_hump(Map *m, unsigned int from_node, unsigned int to_node); //check whether there is already a hump
void add_cost(Map *m, unsigned int from_node,unsigned int to_node,int cost); //add cost to edge
bool map_complete(Map *m, int row, int columns);//check if all vertices have at least 1 edge & prints false if not completed
int Find_cost(Map *m, unsigned int from_node, unsigned int to_node); 
void start_dijkstra(Map *m,int n, int startnode,int dest);

#endif //MAPPING

