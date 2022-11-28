Mapping and Navigation Sub-group

Dennis Ang Wei Jie  2101715
Leong Shu min       2102162
Geethika            2102283


Mapping concept used: Adjacency Matrix
Navigation Algorithm used: Dijkstra Algorithm


Resources:
https://www.youtube.com/watch?v=nvRkFi8rbOM&ab_channel=JacobSorber
https://www.thecrazyprogrammer.com/2014/03/dijkstra-algorithm-for-finding-shortest-path-of-a-graph.html

Functions:

Map *create_Map(int vertices) 			                                 : Allocates space for the map
destroy_Map(Map* m)     			                                     : Frees up the memory used by the map
print_Map(Map *m)       			                                     : Prints the edges in a dot format
print_Edge(Map *m,int row,int columns)                                  : Prints an Edge Map after you added edges using add_edge()
print_Barcode(Map *m, int row, int columns)                             : Prints a Barcode Map after you added barcodes using add_barcode()
print_Hump(Map *m,int row, int columns)                                 : Prints a Hump Map after you added Humps using add_humps;  
print_Path(Map *m, int row, int columns);                               : Prints the path of car
add_Path(Map *m,unsigned int from_node, unsigned int to_node)           : add the path (^,v,<-,->) into the Path Map
has_Path(Map *m, unsigned int from_node, unsigned int to_node)          : checks whether there is already a Path from the 1st to 2nd node
add_edges(Map *m, unsigned int  from_node, unsigned int to_node)        : add available path/edges from the node
has_edge(Map *m, unsigned int from_node, unsigned int to_node)          : check for existing edges.
add_barcode(Map *m, unsigned int  from_node, unsigned int to_node)      : get barcode value from barcode.
has_barcode(Map *m,unsigned int from_node, unsigned int to_node);       : check whether there is already a barcode
add_hump(Map *m,unsigned int from_node, unsigned int to_node)           : get hump value from UltraSonic team.
has_hump(Map *m, unsigned int from_node, unsigned int to_node)          : check whether there is already a hump
add_cost(Map *m, unsigned int from_node,unsigned int to_node,int cost)  : add cost to edge
map_complete(Map *m, int row, int columns)                              : check if all vertices have at least 1 edge & prints false if not completed
Find_cost(Map *m, unsigned int from_node, unsigned int to_node)         : returns the cost between the two nodes
start_dijkstra(Map *m,int n, int startnode,int dest)                    : search for the lowest cost path from the start node to the destination node