#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "hashmap.c"



char * _strdup(const char * str) {
    char * aux = (char *)malloc(strlen(str) + 1);
    strcpy(aux, str);
    return aux;
}

char msg[300];
int test_id = -1;


int success(){
  printf("SUCCESS\n");
  exit(0);
}

void err_msg(char* msg){
    //if(test_id!=-1) return;
    printf("   [\033[31m FAILED \033[0m] ");
    printf("%s\n",msg);
    //print_trace();
}

void ok_msg(char* msg){
    //if(test_id!=-1) return;
    printf ("   [\033[32;1m OK \033[0m] ");
    printf("%s\n",msg);
}

void info_msg(char* msg){
    //if(test_id!=-1) return;
    printf ("   [ INFO ] %s\n",msg);
}

HashMap* initializeMap(){
    info_msg("inicializando la tabla...");
    HashMap * map = (HashMap *)malloc(sizeof(HashMap));
    map->buckets = (Pair **) calloc (10,sizeof(Pair *));
    map->capacity = 10;

    char words[6][8]={"casa","carro","saco","olla","cesa","case"};
    int hashs[6]={8,7,6,0,4,2}; //using function in hashmap.c
    int i;
    for(i=0;i<6;i++){
        sprintf(msg, "value%d",i);
        map->buckets[hashs[i]] = 
        createPair(_strdup(words[i]),_strdup(msg));
    }
    
    map->size=6; 
    

    return map;
}



int createMap_test1(){ //Retorna el mapa
    HashMap *map= createMap(10);

    if(map==NULL) {
        err_msg("createMap retorna NULL"); 
        return 0;
    }

    return 1;
}

int createMap_test2(){ //Buckets nulos
    HashMap *map = createMap(10);
    int i=0;
    for(;i<10;i++)
      if(map->buckets[i]!=NULL) {err_msg("bucket no nulo"); return 0;}
    return 1;
}

int createMap_test3(){ //variables
    HashMap *map = createMap(10);
    if(map->size !=0 || map->capacity!=10 || map->current!=-1){
        err_msg("Varibales no fueron inicializadas correctamente");
        return 0;
    }

    map = createMap(5);
    if(map->size !=0 || map->capacity!=5 || map->current!=-1){
        err_msg("Varibales no fueron inicializadas correctamente");
        return 0;
    }

    ok_msg("Mapa inicializado correctamente");
    return 1;
}

void hash_test1(){ // posición válida
    long pos=hash("computador",10);
    if(pos<0 || pos>=10) printf("posicion invalida (computador -> %ld)\n",pos);

    pos=hash("silla",50);
    if(pos<0 || pos>=50) printf("posicion invalida (silla -> %ld)\n",pos);

    pos=hash("mesa",5);
    if(pos<0 || pos>=5) printf("posicion invalida (mesa -> %ld)\n",pos);
    }

void hash_test2(){ // colisiones
    int i, p;
    char words[8][5] = {"caso","cosa","cesa","cae","casa","saca","saco","case"};
    int rep[10]; //rep[p] veces que hash ha retornado el valor p
    for(p=0;p<10;p++) rep[p]=0;

    for(i=0;i<8;i++){
        int p = hash(words[i],10);
        rep[p]++;
        if(rep[p]>=3){
            printf("demasiadas colisiones\n"); return;}
    }
}

void hash_test3(){ //no random
    if(hash(_strdup("hola"),10)!=hash(_strdup("hola"),10)) printf("error: valor diferente para misma clave!\n");
    if(hash(_strdup("chao"),10)!=hash(_strdup("chao"),10)) printf("error: valor diferente para misma clave!\n");
    if(hash(_strdup("casa"),10)!=hash(_strdup("casa"),10)) printf("error: valor diferente para misma clave!\n");
}

void compare_test(){
    if(is_equal("hola","hola")!=1) printf("error: \"hola\"!=\"hola\"\n");
    if(is_equal("hola","chao")!=0) printf("error: \"hola\"==\"chao\"\n");
    if(is_equal("hola",NULL)!=0) printf("error: si key es nula retornar NULL\n");
    if(is_equal(NULL,NULL)!=0) printf("error: si key es nula retornar NULL\n");
}

int insert_test1(){ //elemento con hash=3
    HashMap *map = initializeMap();
    info_msg("se inserta elemento con hash=3");
    insertMap(map, _strdup(":key"),_strdup("value")); 

    if(map->buckets[3] == NULL){
        err_msg("bucket[3]==NULL");
        return 0;
    }

    if( strcmp (map->buckets[3]->key, ":key")!=0 ||
             strcmp (map->buckets[3]->value, "value")!=0 ){
        err_msg("elemento no fue insertado correctamente (hash -> 3)");
        return 0;
    }

    if(map->size != 7){
        err_msg("no modifico variable size");
        return 0;
    }

    if(enlarge_called == 1){
        err_msg("llamo a funcion enlarge");
        return 0;
    }
    ok_msg("insercion exitosa");

    return 1;
}

int insert_test2(){ //no se agregan repetidos
    HashMap *map = initializeMap();
    info_msg("se intenta insertar elemento repetido (key=case)");
    insertMap(map, _strdup("case"),_strdup("repetido"));
    if(map->buckets[1] != NULL){
        err_msg("no se deben agregar elementos repetidos");
        return 0;
    }
    return 1;
}

int insert_test3(){ //colisión
    HashMap *map = initializeMap();
    info_msg("se intenta insertar elemento con hash=7");
    insertMap(map, _strdup("key"),_strdup("value"));
    if(map->buckets[9]==NULL || !is_equal(map->buckets[9]->value,"value") ){
        err_msg("elemento no fue insertado correctamente (en casilla 9)");
        return 0;
    }

    ok_msg("insercion exitosa");;
    return 1;
}

int insert_test4(){ //colisión
    HashMap *map = initializeMap();
    info_msg("se inserta elemento con hash=7");
    insertMap(map, _strdup("key"),_strdup("value"));
    if(map->buckets[9]==NULL || !is_equal(map->buckets[9]->value,"value") ){
        err_msg("elemento no fue insertado correctamente (en casilla 9)");
        return 0;
    }
    ok_msg("insercion exitosa");

    info_msg("se falsea tamanno de tabla a 6 (par evitar enlarge)");
    map->size=6;
    info_msg("se inserta elemento con hash=6");
    insertMap(map, _strdup("holi"),_strdup("value"));
    if(map->buckets[1]==NULL || !is_equal(map->buckets[1]->value,"value") ){
        err_msg("elemento no fue insertado correctamente (en casilla 1)");
        return 0;
    }


    ok_msg("insercion exitosa");
    return 1;
}


int insert_test5(){ //factor de carga>0.7, se llama a función enlarge
    HashMap *map = initializeMap();
    map->size = 7; //se fuerza size=7
    info_msg("se setea size en 7 de 10");
    info_msg("se inserta nuevo elemento");
    insertMap(map, _strdup("key"),_strdup("value"));
    if(enlarge_called == 0){
        err_msg("insertMap no llama a funcion enlarge");
        return 0;
    }
    
    ok_msg("insertMap llama correctamente a la funcion enlarge");
    return 1; 
}


int search_test1(){ //busqueda de elemento insertado
    HashMap *map = initializeMap();
    info_msg("buscando elemento con key=carro");
    void* ret =  searchMap(map, "carro");
    if(ret == NULL){
        err_msg("searchMap retorna NULL");
        return 0;
    }

    if(ret != map->buckets[7]) {
        err_msg("elemento retornado por search no coincide");
        return 0;
    }
    ok_msg("elemento encontrado correctamente");

    if(map->current!=7){
        err_msg("recuerda actualizar el current");
        return 0;
    }

    return 1;
}

int search_test2(){ //busqueda de elemento colisionado
    HashMap *map = initializeMap();
    info_msg("se inserta par (key,value) con hash=7 en posicion 9");
    map->buckets[9]=createPair(_strdup("key"),_strdup("value"));

    info_msg("buscando elemento con key=key");
    Pair* ret =  searchMap(map, "key");
    if(ret == NULL){
        err_msg("searchMap retorna NULL");
        return 0;
    }

    if(ret != map->buckets[9]) {
        err_msg("elemento retornado por search no coincide");
        return 0;
    }
    ok_msg("elemento encontrado correctamente");
    return 1;
}

int search_test3(){ //busqueda de elemento que no existe
    HashMap *map = initializeMap();
    info_msg("se busca clave:\"holo\" con hash=2");
    if(searchMap(map, "holo")!=NULL) {
        sprintf(msg,"searchMap retorna un elemento con clave %s", map->buckets[map->current]->key);
        err_msg(msg);
        return 0;
    }
    ok_msg("clave no encontrada");
    return 1;
}


int erase_test1(){  //elimina dato
    HashMap *map = initializeMap();
    info_msg("se elimina dato con key=olla");
    eraseMap(map, "olla");

    if(map->buckets[0]==NULL){
        err_msg("se elimino el Pair (buckets[0])");
        return 0;
    }  
    
    if(map->buckets[0]->key !=NULL) {
        err_msg("no se elimino la key: buckets[0]->key!=NULL");
        return 0;
    }

    if(map->size != 5){
        err_msg("recuerda reducir size en 1");
        return 0;
    }


    ok_msg("el dato se elimino correctamente");
    return 1;
}

int erase_test2(){  //elimina dato colisionado
    HashMap *map = initializeMap();
    info_msg("se inserta par (key,value) con hash=7 en posicion 9");
    map->buckets[9]=createPair(_strdup("key"),_strdup("value"));

    info_msg("se intenta eliminar el mismo dato");
    eraseMap(map, "key");
    
    if(map->buckets[9]->key!=NULL){
        err_msg("no se eliminó el dato");
        return 0;
    }
    ok_msg("el dato se elimino correctamente");
    return 1;
}

int erase_test3(){  //elimina dato que no existe
    HashMap *map = initializeMap();
    info_msg("se intenta eliminar dato que no existe (key:holo con hash=2)");
    eraseMap(map, "holo");

    if(map->buckets[2]->key==NULL){
        err_msg("se eliminó dato case");
        return 0;
    }

    ok_msg("eraseMap no hace nada");
    return 1;

}

int first_test(){
    HashMap *map = initializeMap();

    info_msg("llamando a firstMap");
    Pair* aux = firstMap(map);
    if(!aux){
        err_msg("firstMap retorna NULL");
        return 0;
    }

    if(aux != map->buckets[0]){
        err_msg("firstMap retorna no retorna el primer dato");
        return 0;
    }

    if(map->current != 0){
        err_msg("recuerda actualizar el current");
        return 0;
    }

    ok_msg("firstMap retorna el primer dato de la tabla");

    info_msg("eliminando el primer dato de la tabla");
    map->buckets[0]->key=NULL;

    info_msg("llamando a firstMap");
    aux = firstMap(map);   
    
    if(aux != map->buckets[2]){
        err_msg("firstMap no retorna el primer valor");
        return 0;
    }

    if(map->current != 2){
        err_msg("recuerda actualizar el current");
        return 0;
    }

    ok_msg("firstMap retorna el primer dato de la tabla");

    return 1;
}


int next_test(){
    HashMap *map = initializeMap();

    info_msg("posicionando a current en la primera casilla");
    map->current=0;
    if(nextMap(map)!=map->buckets[2]){
        err_msg("nextMap no retorna bucket[2]->value");
        return 0;
    }else
        ok_msg("nextMap retorna bucket[2]");

    if(map->current!=2){
        err_msg("recuerda actualizar el current");
        return 0;
    }

    info_msg("posicionando a current en la casilla 7");
    map->current=7;
    if(nextMap(map)!=map->buckets[8]){
        err_msg("nextMap no retorna bucket[8]->value");
        return 0;
    }else
        ok_msg("nextMap retorna bucket[8]");


    
    info_msg("llamando a nextMap (deberia retornar NULL)");
    if(nextMap(map)!=NULL){
        err_msg("nextMap no retorna NULL");
        return 0;
    }else
        ok_msg("nextMap retorna NULL");
    
    return 1;
}

int enlarge_test(){
    HashMap *map = initializeMap();
    int i;
    
    void* value3 = map->buckets[0]->value;
    void* value5 = map->buckets[2]->value;
    void* value4 = map->buckets[4]->value;

    enlarge(map);

    if(map->buckets[0]->value!=value3 || map->buckets[2]!=NULL || map->buckets[12]->value != value5 || map->buckets[4]->value!=value4){
        // 1. Guardamos cuántos caracteres escribimos inicialmente
        int offset = sprintf(msg, "enlarged table is:\n");

        // 2. Sumamos al offset en cada iteración escribiendo a partir de msg + offset
        for(i = 0; i < map->capacity; i++){
            if(map->buckets[i] == NULL) {
                offset += sprintf(msg + offset, "(null)  ");
            } else {
                offset += sprintf(msg + offset, "%s  ", (char*)map->buckets[i]->value);
            }
        }   

        // 3. Escribimos el final de la cadena
        sprintf(msg + offset, "\nbut should be: \nvalue3  (null)  (null)  (null)  value4  (null)  value2  value1  value0  (null)  (null)  (null)  value5  (null)  (null)  (null)  (null)  (null)  (null)  (null)");

        err_msg(msg);
        return 0;
    }

    ok_msg("enlarge realizado exitosamente");
    return 1;

}

// --- AGRUPACIÓN DE PRUEBAS (SUITES) ---

int test_create() {
    return (createMap_test1() && createMap_test2() && createMap_test3())*5;
}

int test_insert_1() {
    return (insert_test1() && insert_test2())*5;
}

int test_insert_2() {
    return (insert_test3() && insert_test4())*10;
}

int test_search() {
    return (search_test1() && search_test2() && search_test3())*10;
}

int test_erase() {
    return (erase_test1() && erase_test2() && erase_test3())*10;
}

int test_first_next() {
    return (first_test() && next_test())*10;
}

int test_enlarge() {
    return enlarge_test()*10;
}


int test_suite(int(*test)(), char* msg, int max_score, int id, int req_id){
    if(test_id==-1 || test_id==id){
        printf("\n%s\n", msg);
        int score=test();

        if(id==req_id && score == max_score) success();

        printf("   partial_score: %d/%d\n", score, max_score); 
        
        return score;
    }
    return 0;
}

// --- MAIN NUEVO ---

int main( int argc, char *argv[] ) {
    
    if(argc>1) test_id=atoi(argv[1]);
    srand(time(NULL));

    int total_score=0;
    
    // Evaluamos las funciones agrupadas para HashMap basándonos en tu nuevo formato
    // Se reparten los 70 puntos totales originales en las 7 funciones.
    total_score+=test_suite(test_create,     "Test Create Map...",    5, 1, test_id);
    total_score+=test_suite(test_insert_1,   "Test Insert Part 1...", 5, 2, test_id);
    total_score+=test_suite(test_insert_2,   "Test Insert Part 2...", 10, 3, test_id);
    total_score+=test_suite(test_search,     "Test Search Map...",    10, 4, test_id);
    total_score+=test_suite(test_erase,      "Test Erase Map...",     10, 5, test_id);
    total_score+=test_suite(test_first_next, "Test First y Next...",  10, 6, test_id);
    total_score+=test_suite(test_enlarge,    "Test Enlarge...",       10, 7, test_id);
    
    if(argc==1)
      printf("\ntotal_score: %d/60\n", total_score);

    return 0;
}
