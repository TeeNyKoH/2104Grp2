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

Map *create_Map(int vertices) 			                        : Allocates space for the map
destroy_Map(Map* m)     			                        : Frees up the memory used by the map
print_Map(Map *m)       			                        : Prints the edges in a dot format
print_Edge(Map *m,int row,int columns)                                  : Prints an Edge Map after you added edges using add_edge()
print_Barcode(Map *m, int row, int columns)                             : Prints a Barcode Map after you added barcodes using add_barcode()
print_Hump(Map *m,int row, int columns)                                 : Prints a Hump Map after you added Humps using add_humps;  
print_Path(Map *m, int row, int columns);                               : Prints the path of car
add_Path(Map *m,unsigned int from_node, unsigned int to_node)           : Add the path (^,v,<-,->) into the Path Map from the from_node to to_node
has_Path(Map *m, unsigned int from_node, unsigned int to_node)          : Checks whether there is already a Path from the from_node to to_node
add_edges(Map *m, unsigned int  from_node, unsigned int to_node)        : Add a Edge from the from_node to to_node
has_edge(Map *m, unsigned int from_node, unsigned int to_node)          : Check for existing edges from the from_node to to_node
add_barcode(Map *m, unsigned int  from_node, unsigned int to_node)      : Add a Barcode at the barcode map from the from_node to to_node
has_barcode(Map *m,unsigned int from_node, unsigned int to_node);       : Check whether there is already a barcode in the map from the from_node to to_node
add_hump(Map *m,unsigned int from_node, unsigned int to_node)           : Add the hump at the hump map from the from_node to to_node.
has_hump(Map *m, unsigned int from_node, unsigned int to_node)          : Check whether there is already a hump from the from_node to to_node
add_cost(Map *m, unsigned int from_node,unsigned int to_node,int cost)  : Add cost to edge from the from_node to to_node
map_complete(Map *m, int row, int columns)                              : Check if all vertices have at least 1 edge & prints false if not completed
Find_cost(Map *m, unsigned int from_node, unsigned int to_node)         : Returns the cost between the two nodes
start_dijkstra(Map *m,int n, int startnode,int dest)                    : Search for the lowest cost path from the start node to the destination node


