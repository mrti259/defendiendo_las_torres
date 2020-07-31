#include "archivos.h"
#include "defendiendo_torres.h"
#include "configuracion.h"
#include "utiles.h"
#include <string.h>
#include <stdbool.h>

#define ENTRADA 'E'
#define TORRE 'T'
#define FILA 'F'
#define COLUMNA 'C'
#define CAMINO_UNO 1
#define CAMINO_DOS 2
#define PRIMER_NIVEL 1
#define SEGUND_NIVEL 2
#define TERCER_NIVEL 3
#define CUARTO_NIVEL 4
#define CAMINOS_INDEFINIDOS "-1"

const char  IZQUIERDA = 'A', ABAJO = 'S', DERECHA = 'D', ARRIBA = 'W';

const char  FORMATO_NIVEL[] = "NIVEL=%i\n",
            FORMATO_CAMINO[] = "CAMINO=%i\n",
            FORMATO_COORDENADA[] = "%i;%i\n",
            ARCHIVO_TEMPORAL[] = "tmp";

const int   MINIMO_MATRIZ = 0, MAXIMO_MATRIZ_MENOR = 14, MAXIMO_MATRIZ_MAYOR = 19;

const int   AL_ESTE = 14, AL_OESTE = 5;

const int   SIN_CAMINO = 0;



//~ Pre: Recibe un numero de fila y un numero de columna para una matriz.
//~ Pos: Inicializa una coordenada.
void ubicar(coordenada_t *coordenada, int fil, int col){
    coordenada->fil = fil;
    coordenada->col = col;
}


//~ Posiciona la entrada de los enemigos que atacaran a la torre 1 y a la torre. 
void ubicar_extremos_1(juego_t *juego, coordenada_t *entrada_1, coordenada_t *torre_1) {
    if (juego->nivel_actual == PRIMER_NIVEL) {
        ubicar(entrada_1, MAXIMO_MATRIZ_MENOR / 2, MAXIMO_MATRIZ_MENOR);
        ubicar(torre_1, MAXIMO_MATRIZ_MENOR / 2, MINIMO_MATRIZ);

    } else if (juego->nivel_actual == TERCER_NIVEL) {
        ubicar(entrada_1, MINIMO_MATRIZ, AL_ESTE);
        ubicar(torre_1, MAXIMO_MATRIZ_MAYOR, AL_ESTE);

    } else {
        ubicar(entrada_1, MAXIMO_MATRIZ_MAYOR, AL_ESTE);
        ubicar(torre_1, MINIMO_MATRIZ, AL_ESTE);
    }
}


//~ Posiciona la entrada de los enemigos que atacaran a la torre 2 y a la torre. 
void ubicar_extremos_2(juego_t *juego, coordenada_t *entrada_2, coordenada_t *torre_2) {
    if (juego->nivel_actual == SEGUND_NIVEL) {
        ubicar(entrada_2, MAXIMO_MATRIZ_MENOR / 2, MINIMO_MATRIZ);
        ubicar(torre_2, MAXIMO_MATRIZ_MENOR / 2, MAXIMO_MATRIZ_MENOR);

    } else if (juego->nivel_actual == TERCER_NIVEL) {
        ubicar(entrada_2, MINIMO_MATRIZ, AL_OESTE);
        ubicar(torre_2, MAXIMO_MATRIZ_MAYOR, AL_OESTE);

    } else {
        ubicar(entrada_2, MAXIMO_MATRIZ_MAYOR, AL_OESTE);
        ubicar(torre_2, MINIMO_MATRIZ, AL_OESTE);
    }
}



bool es_misma_coordenada(coordenada_t coord_1, coordenada_t coord_2) {
    return (coord_1.fil == coord_2.fil && coord_1.col == coord_2.col);
}



bool es_movimiento_valido(coordenada_t coord_1, coordenada_t coord_2) {
    /*
     * comprobar si no vuelve atras, si no se va de rango (necesita nivel)
     */
}



/*
 *
 */
void avanzar(coordenada_t *coordenada) {
    char movimiento;
    printf("\nUse [%c]izquierda [%c]abajo [%c]derecha [%c]arriba para moverse:", IZQUIERDA, ABAJO, DERECHA, ARRIBA);
    scanf(" %c", &movimiento);
    if (movimiento == IZQUIERDA) {
        ubicar(coordenada, coordenada->fil, coordenada->col-1);
    } else if (movimiento == DERECHA) {
        ubicar(coordenada, coordenada->fil, coordenada->col+1);
    } else if (movimiento == ABAJO) {
        ubicar(coordenada, coordenada->fil+1, coordenada->col);
    } else if (movimiento == ARRIBA){
        ubicar(coordenada, coordenada->fil-1, coordenada->col);
    }
}



void intercambiar_coordenadas(coordenada_t *coord_1, coordenada_t *coord_2) {
    coordenada_t coord_aux = *coord_1;
    *coord_1 = *coord_2;
    *coord_2 = coord_aux;
}



void imprimir_camino(coordenada_t camino[], int tope) {
    for (int i = 0; i < tope; i++)
        printf("Coord %i: %i %i | ", tope, camino[i].fil, camino[i].col);
}


/*
 *
 */
void construir_camino(juego_t *juego, int camino) {
    int i;
    if (camino == CAMINO_UNO) {
        i = juego->nivel.tope_camino_1;
        do {
            mostrar_juego(*juego);
            imprimir_camino(juego->nivel.camino_1, juego->nivel.tope_camino_1);
            juego->nivel.camino_1[i] = juego->nivel.camino_1[i-2];
            avanzar(&(juego->nivel.camino_1[i]));
            intercambiar_coordenadas(&(juego->nivel.camino_1[i]), &(juego->nivel.camino_1[i-1]));
            juego->nivel.tope_camino_1 = 1 + i++;
        }
        while (!es_misma_coordenada(juego->nivel.camino_1[i-1], juego->nivel.camino_1[i-2]));

    } else {
        i = juego->nivel.tope_camino_2;
        do {
            mostrar_juego(*juego);
            imprimir_camino(juego->nivel.camino_2, juego->nivel.tope_camino_2);
            juego->nivel.camino_2[i] = juego->nivel.camino_2[i-2];
            avanzar(&(juego->nivel.camino_2[i]));
            intercambiar_coordenadas(&(juego->nivel.camino_2[i]), &(juego->nivel.camino_2[i-1]));
            juego->nivel.tope_camino_2 = 1 + i++;
        }
        while (!es_misma_coordenada(juego->nivel.camino_2[i-1], juego->nivel.camino_2[i-2]));
    }
}




/*
 *
 */
void crear_camino(juego_t *juego) {
    if (juego->nivel_actual != SEGUND_NIVEL) {
        ubicar_extremos_1(juego, &(juego->nivel.camino_1[0]), &(juego->nivel.camino_1[1]));
        juego->nivel.tope_camino_1 = 2;
        construir_camino(juego, CAMINO_UNO);
    } else juego->nivel.tope_camino_1 = 0;

    if (juego->nivel_actual != PRIMER_NIVEL) {
        ubicar_extremos_2(juego, &(juego->nivel.camino_2[0]), &(juego->nivel.camino_2[1]));
        juego->nivel.tope_camino_2 = 2;
        construir_camino(juego, CAMINO_DOS);
    } else juego->nivel.tope_camino_2 = 0;
}



/*
 * Escribe los caminos de un nivel en un archivo temporal.
 * En cada nivel el archivo de abre al final y adjunta la información.
 */
int escribir_camino(juego_t juego) {
    FILE* archivo = fopen(ARCHIVO_TEMPORAL, ADJUNTO);
    if (!archivo) {
        printf("No se pueden grabar los caminos acá.");
        return -1;
    }

    fprintf(archivo, FORMATO_NIVEL, juego.nivel_actual);
    if (juego.nivel.tope_camino_1)
        fprintf(archivo, FORMATO_CAMINO, CAMINO_UNO);
    for (int i = 0; i < juego.nivel.tope_camino_1; i++)
        fprintf(archivo, FORMATO_COORDENADA, juego.nivel.camino_1[i].fil, juego.nivel.camino_1[i].col);

    if (juego.nivel.tope_camino_2)
        fprintf(archivo, FORMATO_CAMINO, CAMINO_DOS);
    for (int i = 0; i < juego.nivel.tope_camino_2; i++)
        fprintf(archivo, FORMATO_COORDENADA, juego.nivel.camino_2[i].fil, juego.nivel.camino_2[i].col);

    fclose(archivo);
    return 0;
}



/*
 * Pre: Recibe el nombre del archivo donde se encuentran los caminos a utilizar,
 *      y un juego que inicializa un nuevo nivel.
 * Pos: Se inicializan los caminos del nivel de que se va a jugar.
 */
int leer_camino(char caminos_config[MAX_ARCHIVO], juego_t *juego) {
    int nivel = 0, num_camino, tope;
    FILE *archivo = fopen(caminos_config, LECTURA);
    if (!archivo) return -1;

    char linea[MAX_LINEA];
    fscanf(archivo, "%s", linea);
    while (!feof(archivo) && (nivel <= juego->nivel_actual)) {
        if (tienen_misma_etiqueta(linea, FORMATO_NIVEL)) {
            sscanf(linea, FORMATO_NIVEL, &nivel);
            if (juego->nivel_actual == nivel) {
                juego->nivel.tope_camino_1 = 0;
                juego->nivel.tope_camino_2 = 0;
            }
        } else if (tienen_misma_etiqueta(linea, FORMATO_CAMINO) && juego->nivel_actual == nivel) {
            sscanf(linea, FORMATO_CAMINO, &num_camino);
            tope = 0;
        } else {            
            if (num_camino == CAMINO_UNO && juego->nivel_actual == nivel) {
                sscanf(linea, FORMATO_COORDENADA, &(juego->nivel.camino_1[tope].fil), &(juego->nivel.camino_1[tope].col));
                juego->nivel.tope_camino_1 = tope+1;
            } else if (juego->nivel_actual == nivel){
                sscanf(linea, FORMATO_COORDENADA, &(juego->nivel.camino_2[tope].fil), &(juego->nivel.camino_2[tope].col));
                juego->nivel.tope_camino_2 = tope+1;
            }
            tope++;
        }
        fscanf(archivo, "%s", linea);            
    }
    fclose(archivo);
    return 0;
}


//~ Pre: Recibe un juego en un nuevo nivel por inicializar sin caminos predefinidos.
//~ Pos: Genera el camino 1.
void obtener_camino_1(juego_t *juego) {
    coordenada_t entrada_1, torre_1;
    ubicar_extremos_1(juego, &entrada_1, &torre_1);
    if (juego->nivel_actual != SEGUND_NIVEL)
        obtener_camino(juego->nivel.camino_1, &(juego->nivel.tope_camino_1), entrada_1, torre_1);
    else juego->nivel.tope_camino_1 = 0;
}



//~ Pre: Recibe un juego en un nuevo nivel por inicializar sin caminos predefinidos.
//~ Pos: Genera el camino 2.
void obtener_camino_2(juego_t *juego) {
    coordenada_t entrada_2, torre_2;
    ubicar_extremos_2(juego, &entrada_2, &torre_2);
    if (juego->nivel_actual != PRIMER_NIVEL)
        obtener_camino(juego->nivel.camino_2, &(juego->nivel.tope_camino_2), entrada_2, torre_2);
    else juego->nivel.tope_camino_2 = 0;
}



/*
 * Definido en caminos.h
 */
void cargar_caminos(juego_t *juego, char caminos_config[MAX_ARCHIVO]) {
    if (!strcmp(caminos_config, CAMINOS_INDEFINIDOS)) {
        obtener_camino_1(juego);
        obtener_camino_2(juego);
        return;
    }
    leer_camino(caminos_config, juego);
}



/* PARA PROBAR LAS FUNCIONES*/
/*
void imprimir_camino(juego_t juego) {
    printf(FORMATO_NIVEL, juego.nivel_actual);
    if (juego.nivel.tope_camino_1) printf(FORMATO_CAMINO, CAMINO_UNO);
    for (int i = 0; i < juego.nivel.tope_camino_1; i++) {
        printf(FORMATO_COORDENADA, juego.nivel.camino_1[i].fil, juego.nivel.camino_1[i].col);
    }
    if (juego.nivel.tope_camino_2) printf(FORMATO_CAMINO, CAMINO_DOS);
    for (int i = 0; i < juego.nivel.tope_camino_2; i++) {
        printf(FORMATO_COORDENADA, juego.nivel.camino_2[i].fil, juego.nivel.camino_2[i].col);
    }
}
*/

int main () {
    FILE* archivo = fopen("nuevos_caminos.txt", "w");
    if (!archivo) return -1;
    juego_t juego;
    configuracion_t configuracion;
    cargar_configuracion(&configuracion, "");
    configuracion.juego.torres.resistencia_torre_1 = 0;
    configuracion.juego.torres.resistencia_torre_2 = 0;
    inicializar_juego(&juego, (int) 0, (int) 0, (char) 0, (char) 0, configuracion);
    for (int i = 1; i < 5; i++) {
        juego.nivel_actual = i;
        crear_camino(&juego);
        //~ cargar_caminos(&juego, CAMINOS_INDEFINIDOS);
        escribir_camino(juego);
    }
    fclose(archivo);
    rename(ARCHIVO_TEMPORAL, "nuevos_caminos.txt");
    return 0;
}
