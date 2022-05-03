#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "hashmap.c"
#include "list.c"


typedef struct{
    char nombre[51];
    char marca[31];
    char tipo[31];
    size_t stock;
    size_t valor;
} Producto;

typedef struct{
    Producto* referencia; //Como idea es que el producto que se agrega al carrito tenga un puntero de referencia al producto real,
    size_t cantidad;      //para conseguir todos los datos y facilitar posteriormente funciones como "eliminar del carrito"
}ProductoCarrito;

// Struct Carrito // 
/* 
   Esta struct es la que almacena las variables del carrito a crear o ya creado y el que guarda 
   los datos de los productos que se van ingresando en este 
*/
typedef struct{
    char nombre[31];
    List* productos;
    size_t cantProductos;
    size_t cantTotalProductos;
}Carrito;  

typedef struct{
    HashMap* tipo;
    HashMap* marca;
    HashMap* nombre;
} Stock;

//Función que reserva memoria para un tipo stock y tamién para los mapas del almacen
//Luego retorna la variable de tipo stock al main
Stock *createStock(){
    Stock* stock = (Stock*) malloc( sizeof(Stock) );
    stock->nombre = createMap(150);
    stock->marca = createMap(10);
    stock->tipo = createMap(10);
    return stock;
}

// Funcion que reserva memoria para un tipo Carrito y tambien para la lista que almacenara los productos
// Luego retorna la variable de tipo Carrito.
Carrito* createCarrito( char* nombre ){
    Carrito* carrito = (Carrito*) malloc( sizeof(Carrito) );
    strcpy(carrito->nombre , nombre);
    carrito->cantProductos = 0;
    carrito->productos = createList();
    carrito->cantTotalProductos = 0;
    return carrito;
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

//Función para validar que la palabra ingresada sea un número
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

//Función para convertir un string en un número
size_t convertirNum(char *string) {
    size_t cont;
    size_t num = 0;

    for (cont = 0; cont < strlen(string); cont++) {
        num = (num * 10) + (string[cont] - '0');
    }
    
    return num;
}

//Función que recibe un char* y valida que este sea una s o un n, en caso contrario,
//se pide volver a ingresarlo hasta que se cumpla la condición
void validarS_N(char* opcionS_N){

    while( strcmp( opcionS_N, "n" ) != 0  && strcmp( opcionS_N, "s" ) != 0){
        printf("Ingrese una opcion valida en minuscula [s/n]:");
        fflush(stdin);
        scanf("%s", opcionS_N);
    }

}

//Función que ingresa un producto al mapa nombre del almacen, 
//en caso de existir previamente aumenta el stock
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

//Función que ingresa un producto al mapa marca del almacen.
//Para hacer esto primero busca en el mapa marca, la marca correspondiente al producto,
//obtiene el mapa exclusivo para esa marca y ahí inserta el producto con clave nombre
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

//Función que ingresa un producto al mapa tipo del almacen.
//Para hacer esto primero busca en el mapa tipo, el tipo correspondiente al producto,
//obtiene el mapa exclusivo para esa tipo y ahí inserta el producto con clave nombre
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

//Función para ordenar todo los tipos de agregar producto
void agregarProducto(Stock* almacen, Producto* nuevoPrdcto ){
    agregarXNombre(almacen->nombre, nuevoPrdcto);
    agregarXMarca(almacen->marca, nuevoPrdcto);
    agregarXTipo(almacen->tipo, nuevoPrdcto);
}

//Función que lee el archivo csv y por cada linea leida, 
//guarda los datos del producto en una variable de tipo producto
//para luego ingresarla al almacen con la función agregar producto
void importar(Stock* almacen){
    FILE* fp = NULL;
    bool errorArchivo = true;
    system("cls");
    while(errorArchivo){ //valida que se repita el proceso en caso de que el 
        system("cls");  //usuario ingresa s y ocurrio un error al abrir el archivo
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

//Función que exporta todos los productos del almacen a un archivo csv, siendo un producto por cada linea,
//la función exporta del mapa nombre ya que este es el más fácil de acceder
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
    while(errorArchivo){ //valida que se repita el proceso en caso de que el 
        system("cls");   //usuario ingresa s y ocurrio un error al abrir el archivo
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

//Función que recibe del usuario todos los datos de un producto para asignarlos a una variable de este tipo
//con el fin de agregar este producto al almacen
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

//Función que informa al usuario de como se agrega el producto, o si desea agregar otro
//valida tambien que si esta repetido se aumenta el stock, o si tiene distinta marca o tipo del que exista no se agregue
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

//Función que busca todos los productos de un cierto tipo ingresado por el usuario
//cuando el usuario ingresa un tipo, se abre el mapa exclusivo de ese tipo (en caso de existir)
//y muestra todos los productos que se encuentran en este
void buscarTipo(HashMap* MapTipo){
    if( MapTipo->size == 0 ){
        system("cls");
        printf("\n========================= BUSCANDO PRODUCTOS POR TIPO ========================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================= VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }
    char accion[1];
    char tipoDeProducto[31];
    system("cls");
    printf("\n========================= BUSCANDO PRODUCTOS POR TIPO ========================\n");
    printf("\nIngrese el tipo de producto: ");
    fflush(stdin);
    gets(tipoDeProducto);

    HashMap* mapProductos ;
    Pair* aux ;
    Producto* producto;

    if( (aux = searchMap( MapTipo, tipoDeProducto)) ){
        system("cls");
        mapProductos = (HashMap*) searchMap(MapTipo, tipoDeProducto)->value;
        producto = (Producto*) firstMap(mapProductos)->value;
        printf("================================================== PRODUCTO(S) =======================================================\n");
        printf("| Producto |      Marca     |                        Nombre                     |  Stock  |      Tipo      |  Valor  |\n");
        printf("|==========|================|===================================================|=========|================|=========|\n");
        for (int cont = 0; cont < mapProductos->size; cont++) {
            printf("|%6d    |", cont + 1);
            printf("%14s  |", producto->marca);
            printf("%50s |", producto->nombre);
            printf("%6d   |", producto->stock);
            printf("%13s   |", producto->tipo);
            printf("%8d |\n", producto->valor);
            printf("|==========|================|===================================================|=========|================|=========|\n");
            if(cont != mapProductos->size - 1) producto = (Producto*) nextMap(mapProductos)->value;
        }

        printf("=============================================== VOLVIENDO AL MENU ====================================================\n");
        system("pause");
    }
    else{
        printf("\nTipo escogido no existe, desea intentarlo nuevamente? (s/n): ");
        fflush(stdin);
        scanf("%s", accion);
        accion[0] = tolower(accion[0]);

        if (strcmp(accion, "s") == 0) buscarTipo(MapTipo);
        return;
    }
}

//Función que busca todos los productos de una cierta marca ingresada por el usuario
//cuando el usuario ingresa una marca, se abre el mapa exclusivo de esa marca (en caso de existir)
//y muestra todos los productos que se encuentran en este
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

//Función que busca todos los productos en el mapa nombre y los muestra por pantalla
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

// Funcion mostrarProductos //
/* 
   Esta funcion se encarga de mostrar todos los productos que estan en el almacen y mostrar cada
   dato de dicho producto como el tipo, marca y nombre de este.
   Funcion de tipo void por lo que no retorna nada
*/ 
void mostrarProductos(Stock* almacen, size_t num)
{
    if(almacen->nombre->size == 0)
    {
        printf("\n======================== MOSTRANDO TODOS LOS PRODUCTOS ========================\n");
        printf("\n                 NO HAY PRODUCTOS EN EL ALMACEN PARA MOSTRAR\n");
        printf("\n============================== VOLVIENDO AL MENU ==============================\n");
        system("pause");
        return;
    }
    Pair* variable;
    Producto* prAux;

    system("cls");
    printf("================================================== PRODUCTO(S) =======================================================\n");
    printf("| Producto |      Marca     |                        Nombre                     |  Stock  |      Tipo      |  Valor  |\n");
    printf("|==========|================|===================================================|=========|================|=========|\n");
    variable = firstMap(almacen->nombre);
    for (int cont = 0; cont < almacen->nombre->size; cont++)
    {
        prAux = (Producto*) variable->value;    
        printf("|%6d    |", cont + 1);
        printf("%14s  |", prAux->marca);
        printf("%50s |", prAux->nombre);
        printf("%6d   |", prAux->stock);
        printf("%13s   |", prAux->tipo);
        printf("%8d |\n", prAux->valor);
        printf("|==========|================|===================================================|=========|================|=========|\n");

        variable = nextMap(almacen->nombre);
        if(variable == NULL)
            break;
    }
    if (num == 0) {
        printf("\n================================================= VOLVIENDO AL MENU ==================================================\n");
        system("pause");
    }
}

// Funcion agregarProductoCarrito //
/* 
   Esta funcion se encarga de agregar uno o varios productos a un carrito que de no existir crea uno nuevo,
   en caso contrario ocupa el ya creado. 
   Funcion tipo void no retorna nada. 
*/
void agregarProductoCarrito(Stock* almacen, HashMap* MapCarritos)
{
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
        mostrarProductos(almacen, 1);
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

//Función que elimina el ultimo producto ingresado del carrito que indicó el usuario
//luego de eliminarlo del carrito le devuelve la misma cantidad al stock del producto original
//si ya no existia lo volvia a ingresar al mapa
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

//Función que recorre los productos del carrito por orden de ingreso y calcula el total a pagar por el usuario
//dependiendo del precio y cantidad de productos
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

/*
Mostrar carritos de compra: Se muestran los nombres de los carritos 
de compra creados y la cantidad de productos que tiene cada uno de ellos. 
*/
void mostrarCarritos(HashMap* carritosDeCompra){
    system("cls");
    size_t i=1;
    Pair * aux = firstMap(carritosDeCompra);
    if (aux == NULL){
        printf("\n======================== MOSTRANDO TODOS LOS CARRITOS ========================\n");
        printf("\n                     NO HAY CARRITOS VALIDOS PARA MOSTRAR\n");
        printf("\n============================== VOLVIENDO AL MENU ==============================\n");
    }
    else 
    {
        printf("|================================================== Carritos de Compra =======================================================|\n");
        printf("|                        Nombre                           |                        Cantidad de Productos                      |\n");
        printf("|=========================================================|===================================================================|\n");
        while (aux != NULL){
            printf("   %54s |",((Carrito*)aux->value)->nombre);
            printf(" %65zd |\n", ((Carrito*)aux->value)->cantTotalProductos);
            printf("\n");
            aux = nextMap(carritosDeCompra);
            i++;
        printf("|=========================================================|===================================================================|\n");
        }

    printf("======================================================= [FIN DE LA LECTURA] =======================================================\n");
    }

    system("pause");
}


//Función principal donde se desplegará el menú de la tienda
int main() {
    system("color 0d");
    Stock *almacen = createStock();
    char opcion[2];
    int auxOpcion;
    HashMap* carritosDeCompras = createMap(15);

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
            case 7: 
                mostrarProductos( almacen, 0);
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
            case 12:
                return EXIT_SUCCESS;
        }
    }
    

    return EXIT_SUCCESS;
}
