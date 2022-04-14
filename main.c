#include <stdio.h>
#include <stdbool.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.c"
//#include "pila.c"

typedef struct{
    //Pila* productos;
    size_t cantProductos;
}Carrito;  

typedef struct{
    HashMap* arrCarritos;
    size_t cantCarritos;
} Carritos;

typedef struct{
    char nombre[51];
    size_t cantidad;
    size_t valor;
}ProductoCarrito;

typedef struct{
    char nombre[51];
    char marca[31];
    char tipo[31];
    size_t stock;
    size_t valor;
} Producto;

typedef struct{
    HashMap* tipo;
    HashMap* marca;
    HashMap* nombre;
} Stock;

Stock *createStock(){
    Stock* stock = (Stock*) malloc( sizeof(Stock) );
    stock->nombre = createMap(150);
    stock->marca = createMap(50);
    stock->tipo = createMap(30);
    return stock;
}

const char *get_csv_field (char * tmp, int k) {
    int open_mark = 0;
    char* ret=(char*) malloc (100*sizeof(char));
    int ini_i=0, i=0;
    int j=0;
    while(tmp[i+1]!='\0'){

        if(tmp[i]== '\"'){
            open_mark = 1-open_mark;
            if(open_mark) ini_i = i+1;
            i++;
            continue;
        }

        if(open_mark || tmp[i]!= ','){
            if(k==j) ret[i-ini_i] = tmp[i];
            i++;
            continue;
        }

        if(tmp[i]== ','){
            if(k==j) {
               ret[i-ini_i] = 0;
               return ret;
            }
            j++; ini_i = i+1;
        }

        i++;
    }

    if(k==j) {
       ret[i-ini_i] = 0;
       return ret;
    }


    return NULL;
}

void agregarXNombre(HashMap* MapNombre, Producto* nuevoPrdcto){
    Pair* aux ;
    Producto* prdctoAux ;
    
    if( (aux = searchMap( MapNombre, nuevoPrdcto->nombre)) ){
        prdctoAux = (Producto *) aux->value;
        prdctoAux->stock += nuevoPrdcto->stock;
    }
    else{
        insertMap(MapNombre, nuevoPrdcto->nombre, (Producto *) nuevoPrdcto);
    }

}

void agregarXMarca(HashMap* MapMarca , Producto* nuevoPrdcto){
    HashMap* mapaAux ;
    Pair* aux ;
    Producto* prdctoAux ;

    if( (aux = searchMap( MapMarca, nuevoPrdcto->marca)) ){
        mapaAux = (HashMap*) aux->value;
        insertMap(mapaAux, nuevoPrdcto->nombre, nuevoPrdcto);
    }
    else{
        mapaAux = createMap(20);
        insertMap(mapaAux, nuevoPrdcto->nombre, nuevoPrdcto);
        insertMap(MapMarca, nuevoPrdcto->marca, mapaAux);
    }
}

void agregarXTipo(HashMap* MapTipo , Producto* nuevoPrdcto){
    HashMap* mapaAux ;
    Pair* aux ;
    Producto* prdctoAux = (Producto*) malloc( sizeof(Producto) );

    if( (aux = searchMap( MapTipo, nuevoPrdcto->tipo)) ){
        mapaAux = (HashMap*) aux->value;
        insertMap(mapaAux, nuevoPrdcto->nombre, nuevoPrdcto);
    }
    else{
        mapaAux = createMap(20);
        insertMap(mapaAux, nuevoPrdcto->nombre, nuevoPrdcto);
        insertMap(MapTipo, nuevoPrdcto->tipo, mapaAux);
    }
}

void agregarProducto(Stock* almacen, Producto* nuevoPrdcto ){
    agregarXNombre(almacen->nombre, nuevoPrdcto);
    agregarXMarca(almacen->marca, nuevoPrdcto);
    agregarXTipo(almacen->tipo, nuevoPrdcto);
}

void importar(Stock* almacen){
FILE* fp = NULL;
    bool errorArchivo = true;
    while(errorArchivo){
        //system("cls");
        printf("\n========== IMPORTANDO ARCHIVO CSV ==========\n");
        char nombreArchivo[31];
        printf("\nIngrese el nombre del archivo: ");
        fflush(stdin);
        gets(nombreArchivo);
        fp = fopen ( strcat( nombreArchivo, ".csv" ), "r");

        if(!fp){
            char reintento[2];
            printf("\nERROR AL ABRIR EL ARCHIVO O NO EXISTE\n");
            printf("No es necesario que escriba la extension .csv\n");
            printf("Desea intentarlo nuevamente? [s/n]: ");
            fflush(stdin);
            scanf("%s", reintento);

            if(strcmp( reintento, "n" ) == 0){
                printf("\n========== VOLVIENDO AL MENU ==========\n");
                getch();
                return;
            } 
        }
        else{
            errorArchivo = false;
        }
    }
    char linea[1024];
    char nombreLista[31];
    char* aux;
    size_t cantGeneros;
    while (fgets (linea, 1023, fp) != NULL) { 
        Producto* productoActual = (Producto*) malloc( sizeof(Producto) );

        strcpy( productoActual->nombre, aux = (char *) get_csv_field(linea, 0) );
        strcpy( productoActual->marca, aux = (char *) get_csv_field(linea, 1) );
        strcpy( productoActual->tipo, aux = (char *) get_csv_field(linea, 2) );
        productoActual->stock = atoi( aux = (char *) get_csv_field(linea, 3) );
        productoActual->valor = atoi( aux = (char *) get_csv_field(linea, 4) );

        agregarProducto( almacen, productoActual);
        free(aux);
        
    }
    fclose(fp);
    printf("\n========== IMPORTADO CON EXITO ==========\n");
    /*
    Pair* aux2;
    Producto* p;
    aux2 = searchMap(almacen->nombre, "acondicionador 650 ml");
    p = (Producto*) aux2->value;
    printf("%i", p->stock);*/
    getch();
}

int main() { 

    Stock *almacen = createStock();

    int opcion = -1;
    while(opcion != 12) {
        //system("cls");
        printf("\n========== MENU DE COMPRAS ==========\n");
        printf("\n1.-  Importar productos\n");
        printf("2.-  Exportar productos\n");
        printf("3.-  Agregar producto \n");
        printf("4.-  Buscar productos por tipo\n");  
        printf("5.-  Buscar productos por marca\n"); 
        printf("6.-  Buscar producto por nombre\n");  
        printf("7.-  Mostrar todos los productos\n");
        printf("8.-  Agregar producto al carrito\n");
        printf("9.-  Eliminar producto del carrito\n");
        printf("10.- Concretar compra\n");
        printf("11.- Mostrar carritos de compra\n");
        printf("12.- Salir\n");

        printf("\nINGRESE SU OPCION: ");
        scanf("%i", &opcion);

        switch(opcion)
        {
            case 1: 
                importar(almacen);
                break;
            /*case 2: 
                exportar(almacen);
                break;
            case 3: 
                menuAgregarProducto(almacen);
                break;
            case 4: 
                buscarTipo(almacen);
                break;
            case 5: 
                buscarMarca(almacen);
                break;
            case 6: 
                buscarNombre(almacen);
                break;
            case 7: 
                mostrarProductos(almacen);
                break;
            case 8: 
                agregarProductoCarrito(almacen);
                break;
            case 9: 
                eliminarProductoCarrito(almacen);
                break;
            case 10:
                comprarCarrito(almacen);
                break;
            case 11:
                mostrarCarritos(almacen);
                break;
            case 12:
                return 0;*/
        }
    }
    

    return EXIT_SUCCESS;
}