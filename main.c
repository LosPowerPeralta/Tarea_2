#include <stdio.h>
#include <stdbool.h>
#include <conio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.c"
#include "list.c"

typedef struct{
    char nombre[31];
    List* productos;
    size_t cantProductos;
    size_t cantTotalProductos;
}Carrito;  

typedef struct{
    char nombre[51];
    char marca[21];
    char tipo[21];
    size_t stock;
    size_t valor;
} Producto;

typedef struct{
    size_t cantidad;
    Producto* referencia;
}ProductoCarrito;

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
    printf("\n================================== BUSCANDO PRODUCTOS POR TIPO ==================================\n");
    printf("\nIngrese el tipo de producto: ");
    fflush(stdin);
    gets(tipoDeProducto);

    HashMap* mapaAux ;
    Pair* aux ;
    Producto* prdctoAux ;

    if( (aux = searchMap( MapTipo, tipoDeProducto)) ){
        printf("\n==================== PRODUCTO ========================= MARCA =========== CANTIDAD ==== PRECIO ==\n");
        mapaAux = (HashMap *)aux->value;
        aux = firstMap(mapaAux);

        while(aux){

            prdctoAux = (Producto*) aux->value;
            printf("\n%50s", prdctoAux->nombre);
            printf("%20s", prdctoAux->marca);
            printf("%12i", prdctoAux->stock);
            printf("%12i", prdctoAux->valor);
            aux = nextMap(mapaAux);

        }
        printf("\n\n======================================= BUSQUEDA EXITOSA ========================================\n");
    }
    else{
        printf("\n                               NO EXISTEN PRODUCTOS DE ESE TIPO\n");
        
        printf("\n====================================== VOLVIENDO AL MENU ========================================\n");
    }

    system("pause");
}

void buscarMarca(HashMap* MapMarca){
    if( MapMarca->size == 0 ){
        system("cls");
        printf("\n========================= BUSCANDO PRODUCTOS POR MARCA =======================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }

    char marcaDelProducto[31];
    system("cls");
    printf("\n================================== BUSCANDO PRODUCTOS POR MARCA =================================\n");
    printf("\nIngrese la marca de producto: ");
    fflush(stdin);
    gets(marcaDelProducto);

    HashMap* mapaAux ;
    Pair* aux ;
    Producto* prdctoAux ;

    if( (aux = searchMap( MapMarca, marcaDelProducto)) ){
        printf("\n==================== PRODUCTO ========================= TIPO ============ CANTIDAD ==== PRECIO ==\n");
        mapaAux = (HashMap *)aux->value;
        aux = firstMap(mapaAux);

        while(aux){

            prdctoAux = (Producto*) aux->value;
            printf("\n%50s", prdctoAux->nombre);
            printf("%20s", prdctoAux->tipo);
            printf("%12i", prdctoAux->stock);
            printf("%12i", prdctoAux->valor);
            aux = nextMap(mapaAux);

        }
        printf("\n\n======================================= BUSQUEDA EXITOSA ========================================\n");
    }
    else{
        printf("\n                               NO EXISTEN PRODUCTOS DE ESA MARCA\n");
        printf("\n====================================== VOLVIENDO AL MENU ========================================\n");
    }

    system("pause");
}

void buscarNombre(HashMap* MapNombre){
    if( MapNombre->size == 0 ){
        system("cls");
        printf("\n=============== BUSCANDO PRODUCTO POR NOMBRE =============\n");
        printf("\n         NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n=================== VOLVIENDO AL MENU ====================\n");
        system("pause");
        return;
    }

    char nombreDelProducto[31];
    system("cls");
    printf("\n=============== BUSCANDO PRODUCTO POR NOMBRE =============\n");
    printf("\nIngrese el nombre del producto: ");
    fflush(stdin);
    gets(nombreDelProducto);

    Pair* aux ;
    Producto* prdctoAux ;

    if( (aux = searchMap( MapNombre, nombreDelProducto)) ){
        prdctoAux = (Producto*) aux->value;
        
        printf("\nNOMBRE: %s    ", prdctoAux->nombre);
        printf("\nMARCA:  %s    ", prdctoAux->marca);
        printf("\nTIPO:   %s    ", prdctoAux->tipo);
        printf("\nSTOCK: %6i", prdctoAux->stock);
        printf("\nPRECIO:%6i", prdctoAux->valor);
        printf("\n\n==================== BUSQUEDA EXITOSA ====================\n");
    }
    else{
        printf("\n           NO EXISTE UN PRODUCTO CON ESE NOMBRE\n");
        printf("\n=================== VOLVIENDO AL MENU ====================\n");
    }

    system("pause");
}

void mostrarProductos(HashMap* MapNombre){
    system("cls");
    if( MapNombre->size == 0 ){
        printf("\n======================= MOSTRANDO TODOS LOS PRODUCTOS ========================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }
    printf("\n========================================== MOSTRANDO TODOS LOS PRODUCTOS ============================================\n");
    printf("\n==================== PRODUCTO ========================= MARCA ============= TIPO ============ CANTIDAD ==== PRECIO ==\n");
    Pair* aux;
    Producto* prdctoAux;
    aux = firstMap(MapNombre);
    while(aux){
        prdctoAux = (Producto*) aux->value;
        printf("\n%50s", prdctoAux->nombre);
        printf("%20s", prdctoAux->marca);
        printf("%20s", prdctoAux->tipo);
        printf("%12i", prdctoAux->stock);
        printf("%12i", prdctoAux->valor);

        aux = nextMap(MapNombre);
    }
    printf("\n\n================================================ VOLVIENDO AL MENU ==================================================\n");
    system("pause");
}

Carrito* createCarrito( char* nombre ){
    Carrito* carrito = (Carrito*) malloc( sizeof(Carrito) );
    strcpy(carrito->nombre , nombre);
    carrito->cantProductos = 0;
    carrito->productos = createList();
    carrito->cantTotalProductos = 0;
    return carrito;
}

void agregarProductoCarrito(Stock* almacen, HashMap* MapCarritos){
     system("cls");
    if( almacen->nombre->size == 0 ){
        printf("\n========== AGREGAR PRODUCTO AL CARRITO ==========\n");
        printf("\n         NO HAY PRODUCTOS EN EL ALMACEN\n");
        printf("\n============== VOLVIENDO AL MENU ================\n");
        system("pause");
        return;
    }

    char reintento[2];
    char nombreAux[51];
    while( true ){
        system("cls");
        printf("\n========== AGREGAR PRODUCTO AL CARRITO ==========\n");
        printf("\nNombre del producto: ");
        fflush(stdin);
        gets(nombreAux);
        Pair* aux;
        Producto* productoAux;
        Carrito* carritoAux;
        if( aux = searchMap( almacen->nombre , nombreAux ) ){
            productoAux = ( Producto* ) aux->value;
            printf("\nNombre del carrito: ");
            fflush(stdin);
            gets(nombreAux);

            if( !(aux = searchMap( MapCarritos, nombreAux )) ){
                Carrito* nuevoCarrito = createCarrito(nombreAux);
                insertMap(MapCarritos, nuevoCarrito->nombre, nuevoCarrito);
                aux = searchMap( MapCarritos, nombreAux);
                printf("\nNUEVO CARRITO CREADO\n");
            }

            carritoAux = ( Carrito* ) aux->value;
            ProductoCarrito* prdctoCarrito = (ProductoCarrito*) malloc( sizeof(ProductoCarrito) );
            prdctoCarrito->referencia = productoAux;

            char numeroAux[11];
            do{
                printf("\nIngrese la cantidad de productos que agregara: ");
                fflush(stdin);
                scanf("%s", numeroAux);

                while(!esNumero(numeroAux)){
                    printf("Debe ser un numero: ");
                    fflush(stdin);
                    scanf("%s", numeroAux);
                }

                prdctoCarrito->cantidad = atoi(numeroAux);
                if(prdctoCarrito->cantidad > productoAux->stock){
                    printf("\nDEBE SER UNA CANTIDAD MENOR AL STOCK DEL PRODUCTO\n");
                }

            } while ( prdctoCarrito->cantidad > productoAux->stock );

            productoAux->stock -= prdctoCarrito->cantidad;
            if( productoAux->stock == 0){ 
                eraseMap(almacen->nombre, prdctoCarrito->referencia->nombre);
                aux = searchMap(almacen->tipo, prdctoCarrito->referencia->tipo);
                eraseMap (aux->value, prdctoCarrito->referencia->nombre);
                aux = searchMap(almacen->marca, prdctoCarrito->referencia->marca);
                eraseMap (aux->value, prdctoCarrito->referencia->nombre);
            }

            pushBack( carritoAux->productos, prdctoCarrito);
            carritoAux->cantProductos += 1;
            carritoAux->cantTotalProductos += prdctoCarrito->cantidad;
            printf("\nPRODUCTO AGREGADO CON EXITO\n");
        }
        else{
            printf("\nPRODUCTO NO ENCONTRADO\n");
        }
        
        printf("\nDESEA AGREGAR OTRO PRODUCTO? [s/n]: ");
        fflush(stdin);
        scanf("%s", reintento);
        validarS_N( reintento );

        if(strcmp( reintento, "n" ) == 0){
            break;
        }

    }
    printf("\n============== VOLVIENDO AL MENU ================\n");
    system("pause");
}

void eliminarProductoCarrito(Stock* almacen, HashMap* MapCarritos){
    
    if( MapCarritos->size == 0 ){
        system("cls");
        printf("\n======================= ELIMINAR PRODUCTO DEL CARRITO ========================\n");
        printf("\n                  ACTUALMENTE NO HAY CARRITOS DE COMPRAS\n");
        printf("\n============================ VOLVIENDO AL MENU ===============================\n");
        system("pause");
        return;
    }

    char reintento[2];
    char nombreAux[31];
    
    Carrito* carritoAux;
    ProductoCarrito* pdctCarritoAux;
    while( true ){
        system("cls");
        printf("\n======================= ELIMINAR PRODUCTO DEL CARRITO ========================\n");
        printf("\nNombre del carrito: ");
        fflush(stdin);
        gets(nombreAux);
        Pair* aux;
        if( (aux = searchMap(MapCarritos, nombreAux)) ){
            carritoAux = (Carrito*) aux->value;
            pdctCarritoAux = (ProductoCarrito*) popBack(carritoAux->productos);
            carritoAux->cantTotalProductos -= pdctCarritoAux->cantidad;
            carritoAux->cantProductos -= 1; 
            
            if( carritoAux->cantProductos == 0){
                printf("\nSE EXTRAJO EL ULTIMO PRODUCTO DEL CARRITO\n");
                printf("\nELIMINANDO EL CARRITO\n");
                eraseMap(MapCarritos, carritoAux->nombre);
            }
            else{
                printf("\nSE EXTRAJO EL PRODUCTO DEL CARRITO\n");
            }

            pdctCarritoAux->referencia->stock += pdctCarritoAux->cantidad;              //Devolver la cantidad del carrito al stock
            if( !(aux = searchMap(almacen->nombre, pdctCarritoAux->referencia->nombre)) ){      //Si el producto se habia eliminado, agregarlo denuevo
                agregarProducto(almacen , pdctCarritoAux->referencia);
            }

        }
        else{
            printf("\nCARRITO NO ENCONTRADO\n");
        }

        printf("\nDESEA ELIMINAR OTRO PRODUCTO? [s/n]: ");
        fflush(stdin);
        scanf("%s", reintento);
        validarS_N( reintento );

        if(strcmp( reintento, "n" ) == 0){
            break;
        }

    }
    printf("\n============================ VOLVIENDO AL MENU ===============================\n");
    system("pause");

}

void comprarCarrito( HashMap* MapCarritos ){
    system("cls");
    if( MapCarritos->size == 0 ){
        printf("\n======================= EFECTUANDO COMPRA DE CARRITO ========================\n");
        printf("\n                  ACTUALMENTE NO HAY CARRITOS DE COMPRAS\n");
        printf("\n============================ VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }
    
    char nombreAux[31];
    printf("\n======================= EFECTUANDO COMPRA DE CARRITO ========================\n");
    printf("\nNombre del carrito: ");
    fflush(stdin);
    gets(nombreAux);
    Pair* aux;
    if( aux = searchMap(MapCarritos, nombreAux) ){
        system("cls");
        printf("\n=========================================================== BOLETA =============================================================\n");
        printf("\n==================== PRODUCTO ========================= MARCA ============= TIPO ============ CANTIDAD ==== PRECIO ==== TOTAL ==\n");
        int total = 0;
        Carrito* carritoAux;
        ProductoCarrito* pdctCarritoAux;
        Producto* productoAux;
        carritoAux = (Carrito*) aux->value;
        pdctCarritoAux = popFront(carritoAux->productos);
        while(carritoAux->cantProductos > 0){

            productoAux = (Producto*) pdctCarritoAux->referencia;
            printf("\n%50s", productoAux->nombre);
            printf("%20s", productoAux->marca);
            printf("%20s", productoAux->tipo);
            printf("%12i", pdctCarritoAux->cantidad);
            printf("%12i", productoAux->valor);
            printf("%11i", (pdctCarritoAux->cantidad * productoAux->valor) );
            total += (pdctCarritoAux->cantidad * productoAux->valor);

            pdctCarritoAux = popFront(carritoAux->productos);
            carritoAux->cantProductos -= 1;
        }
        eraseMap(MapCarritos, carritoAux->nombre);
        printf("\n\n================================================================================================================================\n");
        printf("\n                                                  TOTAL A PAGAR : %c%i\n", 36, total);
        printf("\n====================================================== VOLVIENDO AL MENU =======================================================\n");
        system("pause");
    }
    else{
        printf("\nCARRITO NO ENCONTRADO\n");
        printf("\n============================ VOLVIENDO AL MENU ==============================\n");
        system("pause");
    }
}

void mostrarCarritos(HashMap* MapCarritos ){
    system("cls");
    if( MapCarritos->size == 0 ){
        printf("\n======================= MOSTRANDO TODOS LOS CARRITOS ========================\n");
        printf("\n                  ACTUALMENTE NO HAY CARRITOS DE COMPRAS\n");
        printf("\n============================ VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }
    printf("\n======================= MOSTRANDO TODOS LOS CARRITOS ========================\n");
    int cont = 1;
    Pair* aux;
    Carrito* carritoAux;
    aux = firstMap(MapCarritos);
    while(aux){
        carritoAux = (Carrito*) aux->value;
        printf("\nNombre carrito de compra %i: %s", cont, carritoAux->nombre);
        printf("\nCantidad de productos agregados: %i", carritoAux->cantProductos);
        printf("\nCantidad de productos totales que tiene:   %i\n", carritoAux->cantTotalProductos);
        aux = nextMap(MapCarritos);
        cont += 1;
    }
    printf("\n============================ VOLVIENDO AL MENU ==============================\n");
    system("pause");
}

int main() { 
    system("color 7c");
    Stock *almacen = createStock();
    HashMap* carritosDeCompras = createMap(15);

    int opcion = -1;
    char aux[51];
    while(opcion != 12) {
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
        scanf("%s", aux);
        while(!esNumero(aux)){
            printf("Debe ser un numero: ");
            fflush(stdin);
            scanf("%s", aux);
        }
        opcion = atoi(aux);

        switch(opcion)
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
            case 7: 
                mostrarProductos( almacen->nombre );
                break;
            case 8: 
                agregarProductoCarrito( almacen, carritosDeCompras );
                break;
            case 9: 
                eliminarProductoCarrito( almacen, carritosDeCompras );
                break;
            case 10:
                comprarCarrito( carritosDeCompras );
                break;
            case 11:
                mostrarCarritos( carritosDeCompras );
                break;
        }
    }
    

    return EXIT_SUCCESS;
}
