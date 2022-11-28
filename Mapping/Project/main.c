#include "Mapping.h"
#include <stdio.h>
#include "pico/binary_info.h"
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define rows 4
#define col 5
#define total rows*col

int main(){
    stdio_init_all();
    Map *m1 = create_Map((rows*col)+1);

    //Add in Edges (4x5)
    add_edges(m1, 1,2);
    add_edges(m1, 1,6);
    add_edges(m1, 2,7);
    add_edges(m1, 2,3);
    add_edges(m1, 3,8);
    add_edges(m1, 3,4);
    add_edges(m1, 4,5);
    add_edges(m1, 5,10);
    add_edges(m1, 6,11);
    add_edges(m1, 7,12);
    add_edges(m1, 8,9);
    add_edges(m1, 10,15);
    add_edges(m1, 11,16);
    add_edges(m1, 12,13);
    add_edges(m1, 13,14);
    add_edges(m1, 14,15);
    add_edges(m1, 15,20);
    add_edges(m1, 16,17);
    add_edges(m1, 17,18);
    add_edges(m1, 18,19);
    add_edges(m1, 19,20);


    //adding Humps
    add_hump(m1,2,3);
    add_hump(m1,7,12);
    add_hump(m1,18,19);
    add_hump(m1,10,15);


    //adding barcodes
    add_barcode(m1,17,18);
    add_barcode(m1,11,16);
    add_barcode(m1,2,3);
    add_barcode(m1,5,10);
    add_barcode(m1, 13,14);


    //check if mapping completed
    if (map_complete(m1,rows,col)){
        printf("\nMapping completed\n");
    }else{
        printf("\nMapping not completed\n");
    }

    
    while(1){
        
        print_Edge(m1,rows,col);
        sleep_ms(2000);
        print_Hump(m1,rows,col);
        sleep_ms(2000);
        print_Barcode(m1,rows,col);
        sleep_ms(2000);
        //Navigation
        start_dijkstra(m1,total,1,18);
        print_Path(m1,rows,col);
        sleep_ms(2000);
    }

}

