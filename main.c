#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.c"
#include "list.c"

typedef struct{
    char nombre[31];
    List* productos;
    size_t cantProductos;
}Carrito;  

/*  PROPUESTA DE STRUCT
    typedef struct{
        Producto* referencia; //Como idea es que el producto que se agrega al carrito tenga un puntero de referencia al producto real,
        size_t cantidad;      //para conseguir todos los datos y facilitar posteriormente funciones como "eliminar del carrito"
    }ProductoCarrito;
*/

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
    stock->marca = createMap(10);
    stock->tipo = createMap(10);
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

bool esNumero(char *caracter) {
    int cont;
    if( !strcmp(caracter, "0") ) return false;
    for (cont = 0; cont < strlen(caracter); cont++) {
        if (isdigit(caracter[cont]) != true) {
            return false;
        }
    }

    return true;
}
//Sugerencia de usar atoi
size_t convertirNum(char *string) {
    size_t cont;
    size_t num = 0;

    for (cont = 0; cont < strlen(string); cont++) {
        num = (num * 10) + (string[cont] - '0');
    }
    
    return num;
}

void validarS_N(char* opcionS_N){

    while( strcmp( opcionS_N, "n" ) != 0  && strcmp( opcionS_N, "s" ) != 0){
        printf("Ingrese una opcion valida en minuscula [s/n]:");
        fflush(stdin);
        scanf("%s", opcionS_N);
    }

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
        mapaAux = createMap(30);
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
        mapaAux = createMap(30);
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
    system("cls");
    while(errorArchivo){
        system("cls");
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
            validarS_N( reintento );

            if(strcmp( reintento, "n" ) == 0){
                printf("\n============= VOLVIENDO AL MENU ============\n");
                system("pause");
                return;
            } 
        }
        else{
            errorArchivo = false;
        }
    }
    char linea[1024];
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
    printf("\n============ IMPORTADO CON EXITO ===========\n");
    system("pause");
}

void exportar(HashMap* MapNombre ){
    if( MapNombre->size == 0 ){
        system("cls");
        printf("\n========== EXPORTANDO ARCHIVO CSV ==========\n");
        printf("\nNO HAY PRODUCTOS EN EL ALMACEN PARA EXPORTAR\n");
        printf("\n============= VOLVIENDO AL MENU ============\n");
        system("pause");
        return;
    }

    FILE* fp = NULL;
    bool errorArchivo = true;
    while(errorArchivo){
        system("cls");
        printf("\n========== EXPORTANDO ARCHIVO CSV ==========\n");
        char nombreArchivo[31];
        printf("\nIngrese el nombre del archivo: ");
        fflush(stdin);
        gets( nombreArchivo );
        fp = fopen ( strcat( nombreArchivo, ".csv" ), "w");

        if(!fp){
            char reintento[2];
            printf("\nERROR AL ABRIR EL ARCHIVO O NO EXISTE\n");
            printf("No es necesario que escriba la extension .csv\n");
            printf("Desea intentarlo nuevamente? [s/n]: ");
            fflush(stdin);
            scanf("%s", reintento);
            validarS_N( reintento );

            if(strcmp( reintento, "n" ) == 0){
                printf("\n============= VOLVIENDO AL MENU ============\n");
                system("pause");
                return;
            } 
        }
        else{
            errorArchivo = false;
        }
    }

    Pair* aux ;
    Producto* prdctoAux ;
    aux = firstMap( MapNombre );

    while(aux){

        prdctoAux = (Producto*) aux->value;
        fprintf(fp ,"%s,", prdctoAux->nombre);
        fprintf(fp ,"%s,", prdctoAux->marca);
        fprintf(fp ,"%s,", prdctoAux->tipo);
        fprintf(fp ,"%i,", prdctoAux->stock);
        fprintf(fp ,"%i\n", prdctoAux->valor);

        aux = nextMap( MapNombre );
    }

    fclose(fp);
    printf("\n============ EXPORTADO CON EXITO ===========\n");
    system("pause");

}

Producto* createProducto(){
    Producto* productoActual = (Producto*) malloc( sizeof(Producto) );
    char aux[11];
    printf("\nDEBE INGRESAR LOS DATOS DEL PRODUCTO\n");
    printf("\nNombre del producto: ");
    fflush(stdin);
    gets(productoActual->nombre);
    printf("\nMarca del producto: ");
    fflush(stdin);
    gets(productoActual->marca);
    printf("\nTipo de producto: ");
    fflush(stdin);
    gets(productoActual->tipo);
    printf("\nCantidad de productos que agregara al almacen: ");
    fflush(stdin);
    scanf("%s", aux);
    while(!esNumero(aux)){
        printf("Debe ser un numero: ");
        fflush(stdin);
        scanf("%s", aux);
    }
    productoActual->stock = atoi(aux);
    printf("\nValor del producto: ");
    fflush(stdin);
    scanf("%s", aux);
    while(!esNumero(aux)){
        printf("Debe ser un numero: ");
        fflush(stdin);
        scanf("%s", aux);
    }
    productoActual->valor = atoi(aux);
    return productoActual;
}

void menuAgregarProducto( Stock* almacen ){
    char reintento[2];
    bool agregarOtro = true;
    Pair* aux;
    while( agregarOtro ){
        system("cls");
        printf("\n========== MENU AGREGAR PRODUCTO AL ALMACEN ==========\n");
        Producto* producto = createProducto();
        aux = searchMap( almacen->nombre , producto->nombre);
        if( aux ){
            Producto* prdctoAux = (Producto*) aux->value;
            printf("\nEL PRODUCTO YA EXISTE");
            if( is_equal( producto->marca, prdctoAux->marca ) && is_equal( producto->tipo, prdctoAux->tipo ) ){
                printf("\nAUMENTANDO EL STOCK\n");
                agregarProducto(almacen, producto);
                
                if( producto->valor != prdctoAux->valor ){
                    printf("DESEA ACTUALIZAR EL VALOR? [s/n]: ");
                    fflush(stdin);
                    scanf("%s", reintento);
                    validarS_N( reintento );

                    if(strcmp( reintento, "s" ) == 0){
                        prdctoAux->valor = producto->valor;
                    }
                }
            }
            else{
                printf("\nPERO LA MARCA O EL TIPO ES INCORRECTO");
                printf("\nPRODUCTO NO AGREGADO\n");
            } 
        }
        else{
            printf("\nAGREGANDO NUEVO PRODUCTO\n");
            agregarProducto(almacen, producto);
        }
        printf("\nDESEA AGREGAR OTRO PRODUCTO? [s/n]: ");
        fflush(stdin);
        scanf("%s", reintento);
        validarS_N( reintento );

        if(strcmp( reintento, "n" ) == 0){
            agregarOtro = false;
        }

    }
    printf("\n================= VOLVIENDO AL MENU ==================\n");
    system("pause");
    return;
}

void buscarTipo(HashMap* MapTipo){
    if( MapTipo->size == 0 ){
        system("cls");
        printf("\n========================= BUSCANDO PRODUCTOS POR TIPO ========================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }
    
    char tipoDeProducto[31];
    system("cls");
    printf("\n========================= BUSCANDO PRODUCTOS POR TIPO ========================\n");
    printf("\nIngrese el tipo de producto: ");
    fflush(stdin);
    gets(tipoDeProducto);

    HashMap* mapaAux ;
    Pair* aux ;
    Producto* prdctoAux ;

    if( (aux = searchMap( MapTipo, tipoDeProducto)) ){
        printf("\n==================== PRODUCTO ====================");
        printf("== CANTIDAD ==");
        printf("== PRECIO ====\n");
        mapaAux = (HashMap *)aux->value;
        aux = firstMap(mapaAux);

        while(aux){

            prdctoAux = (Producto*) aux->value;
            printf("\n%50s    ", prdctoAux->nombre);
            printf("%7i", prdctoAux->stock);
            printf("%12i", prdctoAux->valor);
            aux = nextMap(mapaAux);

        }
        printf("\n\n============================== BUSQUEDA EXITOSA ==============================\n");
    }
    else{
        printf("\n                       NO EXISTEN PRODUCTOS DE ESE TIPO\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
    }

    system("pause");
}

void buscarMarca(HashMap *MapMarca) {
    HashMap *productos;
    Pair *objeto;
    char accion[1];
    char marca[30];
    size_t cont;

    system("cls");

    if(MapMarca->size == 0){
        printf("\n======================== BUSCANDO PRODUCTOS POR MARCA ========================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }

    printf("\n========================= BUSCANDO PRODUCTOS POR MARCA ========================\n");
    printf("\nIngrese la marca de los productos: ");
    fflush(stdin);
    gets(marca);
    printf("\n");

    if (searchMap(MapMarca, marca) == NULL) {
        printf("Marca escogida no existe, desea intentarlo nuevamente? (s/n): ");
        fflush(stdin);
        scanf("%s", accion);
        accion[0] = tolower(accion[0]);

        if (strcmp(accion, "s") == 0) buscarMarca(MapMarca);
        return;
    }

    system("cls");
    productos = searchMap(MapMarca, marca)->value;
    objeto = firstMap(productos);
    printf("================================================== PRODUCTO(S) =======================================================\n");
    printf("| Producto |      Marca     |                        Nombre                     |  Stock  |      Tipo      |  Valor  |\n");
    printf("|==========|================|===================================================|=========|================|=========|\n");
    for (cont = 0; cont < productos->size; cont++) {
        printf("|%6d    |", cont + 1);
        printf("%14s  |", ((Producto *)objeto->value)->marca);
        printf("%50s |", ((Producto *)objeto->value)->nombre);
        printf("%6d   |", ((Producto *)objeto->value)->stock);
        printf("%13s   |", ((Producto *)objeto->value)->tipo);
        printf("%8d |\n", ((Producto *)objeto->value)->valor);
        printf("|==========|================|===================================================|=========|================|=========|\n");
        objeto = nextMap(productos);
    }

    printf("=============================================== VOLVIENDO AL MENU ====================================================\n");
    system("pause");
}

void buscarNombre(HashMap *MapNombre) {
    Pair *producto;
    char accion[1];
    char nombre[35];
    size_t cont;
    
    system("cls");

    if(MapNombre->size == 0){
        printf("\n======================== BUSCANDO PRODUCTOS POR NOMBRE ========================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }

    printf("\n========================= BUSCANDO PRODUCTOS POR NOMBRE ========================\n");
    printf("\nIngrese el nombre del producto: ");
    fflush(stdin);
    gets(nombre);
    printf("\n");

    if (searchMap(MapNombre, nombre) == NULL) {
        printf("Nombre escogido no existe, desea intentarlo nuevamente? (s/n): ");
        fflush(stdin);
        scanf("%s", accion);
        accion[0] = tolower(accion[0]);

        if (strcmp(accion, "s") == 0) buscarNombre(MapNombre);
        return;
    }

    system("cls");
    producto = searchMap(MapNombre, nombre);
    printf("=========================================== PRODUCTO ======================================================\n");
    printf("|      Marca     |                        Nombre                     |  Stock  |      Tipo      |  Valor  |\n");
    printf("|================|===================================================|=========|================|=========|\n");
    printf("|%14s  |", ((Producto *)producto->value)->marca);
    printf("%49s  |", ((Producto *)producto->value)->nombre);
    printf("%6d   |", ((Producto *)producto->value)->stock);
    printf("%15s |", ((Producto *)producto->value)->tipo);
    printf("%8d |\n", ((Producto *)producto->value)->valor);
    printf("|================|===================================================|=========|================|=========|\n");   
    printf("========================================= VOLVIENDO AL MENU ===============================================\n");
    system("pause");

}

int main() {
    //system("color 7c"); Sugerencia cambio de color
    Stock *almacen = createStock();
    char opcion[2];
    int auxOpcion;
    //HashMap* carritosDeCompras = createMap(15);

    while(true) {
        system("cls");
        printf("\n========== MENU DE COMPRAS ==========\n\n");
        printf("1.-  Importar productos\n");
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

        while (true) {
            gets(opcion);
            if (esNumero(opcion)) break;
            else printf("Opcion ingresada incorrecta, intentelo de nuevo: ");
        }

        auxOpcion = convertirNum(opcion);
        fflush(stdin);

        switch(auxOpcion)
        {
            case 1: 
                importar( almacen );
                break;
            case 2: 
                exportar( almacen->nombre );
                break;
            case 3: 
                menuAgregarProducto( almacen );
                break;
            case 4: 
                buscarTipo( almacen->tipo );
                break;
            case 5: 
                buscarMarca( almacen->marca );
                break;
            case 6: 
                buscarNombre( almacen->nombre );
                break;
            /*case 7: 
                mostrarProductos( almacen->nombre );
                break;*/
            //case 8: 
                //agregarProductoCarrito( almacen, carritosDeCompras );
              //  break;
            /*case 9: 
                eliminarProductoCarrito( almacen, carritosDeCompras );
                break;
            case 10:*/
                //comprarCarrito( carritosDeCompras );
                //break;
            /*case 11:
                mostrarCarritos( carritosDeCompras );
                break;*/
            case 12:
                return EXIT_SUCCESS;
        }
    }
    

    return EXIT_SUCCESS;
}
