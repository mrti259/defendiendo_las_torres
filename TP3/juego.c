#define AUTOR "MG"
#define FECHA "Julio2020"

#include "defendiendo_torres.h"
#include "configuracion.h"
#include "caminos.h"
#include "animos.h"
#include "utiles.h"
#include "juego.h"
#include <stdbool.h>
#include <string.h>

#define ERROR -1

#define ENANOS 'G'
#define ELFOS 'L'

#define PRIMER_NIVEL 1
#define SEGUND_NIVEL 2
#define TERCER_NIVEL 3
#define CUARTO_NIVEL 4

#define JUGANDO 0
#define GANADO 1
#define AGREGADO 0

const char IGNORAR = 'X';
const float DELAY = 0.2f, DELAY_REPRODUCCION = 1.0f;

const int ENANOS_INICIALES_PRIMER_NIVEL = 5,
          ELFOS_INICIALES_PRIMER_NIVEL = 0,
          ENANOS_INICIALES_SEGUND_NIVEL = 0,
          ELFOS_INICIALES_SEGUND_NIVEL = 3,
          ENANOS_INICIALES_TERCER_NIVEL = 3,
          ELFOS_INICIALES_TERCER_NIVEL = 3,
          ENANOS_INICIALES_CUARTO_NIVEL = 3,
          ELFOS_INICIALES_CUARTO_NIVEL = 3,
          COSTO_ENANOS_TORRE_UNO = 50,
          COSTO_ENANOS_TORRE_DOS = 0,
          COSTO_ELFOS_TORRE_UNO = 0,
          COSTO_ELFOS_TORRE_DOS = 50,
          COSTO_INVALIDO = 0,
          ORCOS_PARA_AGREGAR_DEFENSOR = 50,
          ORCOS_PARA_AGREGAR_PRIMERO = 25,
          TORRE_UNO = 1, TORRE_DOS = 2;

const int MAX_ENEMIGOS_PRIMER_NIVEL = 100, MAX_ENEMIGOS_SEGUND_NIVEL = 200,
          MAX_ENEMIGOS_TERCER_NIVEL = 300, MAX_ENEMIGOS_CUARTO_NIVEL = 500;

const int SIN_DEFENSORES = 0, SIN_ENEMIGOS = 0;

//~ Detiene el tiempo, limpia la consola y muestra el juego.
void limpiar_y_mostrar(juego_t juego, float velocidad) {
    if (velocidad == INDEFINIDO) velocidad = DELAY;
    detener_el_tiempo(velocidad);
    system("clear");
    mostrar_juego(juego);
}

//~ Se queda en espera hasta que el usuario ingrese una nueva línea.
void esperar() {
    printf("\n%c para seguir\n", IGNORAR);
    char nada_util;
    scanf(" %c", &nada_util);
}

//~ Primer mensaje. Le explica el juego al usuario.
void mostrar_ayuda() {
    system("clear");
    printf("\nVamos a jugar!\n");
    printf("\nContamos con enanos (G) y elfos (L) para defender las torres.\n");
    printf("\nLos Orcos se dirigen desde una Entrada a una sola de las Torres por un camino.\n");
    printf("\nUn G puede atacar a un enemigo por turno y solo aquellos que lo rodean.\n");
    printf("\nUn L puede atacar a varios enemigos por turno si están a 3 o menos celdas de distancia.\n");
    printf("\nLos ataques pueden fallar, como también pueden ser críticos o no.\n");
    printf("\nLos O no se detienen a luchar. Irán contra una de las torres y la atacarán con toda la vida restante que tengan.\n");
    printf("\nEl juego te habilitará momentos para pensar tu estrategia. Actúa sabiamente.\n");
    esperar();
}

//~ Pre: Recibe un juego inicializado.
//~ Pos: Devuelve true cuando se esta jugando el PRIMER_NIVEL.
bool primer_nivel(juego_t juego) {
    return juego.nivel_actual == PRIMER_NIVEL;
}

//~ Pre: Recibe un juego inicializado.
//~ Pos: Devuelve true cuando se esta jugando el SEGUND_NIVEL.
bool segundo_nivel(juego_t juego) {
    return juego.nivel_actual == SEGUND_NIVEL;
}

//~ Pre: Recibe un juego inicializado.
//~ Pos: Devuelve true cuando se esta jugando el TERCER_NIVEL.
bool tercer_nivel(juego_t juego) {
    return juego.nivel_actual == TERCER_NIVEL;
}

//~ Pre: Recibe el numero de nivel y un rango sin inicializar.
//~ Pos: Obtiene los mínimos y máximos de los valores que puede tomar una coordenada en determinado nivel.
void conseguir_rango(int *min, int *max, int nivel) {
    *min = MINIMO_MATRIZ;
    if (nivel == PRIMER_NIVEL || nivel == SEGUND_NIVEL)
        *max = MAXIMO_MATRIZ_MENOR;
    else
        *max = MAXIMO_MATRIZ_MAYOR;
}

//~ Pre: Recibe un numero ingresado por el usuario y un rango adecuado al nivel.
//~ Pos: Devuelve true si se encuentra entre los valores válidos que puede tomar una coordenada.
bool esta_en_rango(int num, int min, int max) {
    return (num >= min && num <= max);
}

//~ Pre: Recibe un numero de nivel y una coordenada sin inicializar.
//~ Pos: Inicializa la coordenada con valores válidos ingresados por el usuario, adecuados al nivel del juego.
void pedir_coordenada(coordenada_t *coordenada, int nivel) {
    int fil, col, min, max;
    conseguir_rango(&min, &max, nivel);
    printf("\nIngrese fila: ");
    scanf("%i", &fil);
    while (!esta_en_rango(fil, min, max)) {
        printf("No esta en rango! Vuleva a ingresar una fila: ");
        scanf("%i", &fil);
    }
    printf("Ingrese columna: ");
    scanf("%i", &col);
    while (!esta_en_rango(col, min, max)) {
        printf("No esta en rango! Vuleva a ingresar una columna: ");
        scanf("%i", &col);
    }
    ubicar(coordenada, fil, col);
}

/*
 * Pre: Recibe el número de nivel que se va a jugar, el tipo de defensores que se desea agregar
 *      y la configuración elegida para el juego.
 * Pos: Devuelve la cantidad de defensores que se deberán agregar según la configuración. Cuando
 *      tiene valor INDEFINIDO, se toman los valores por default.
 */
int defensores_segun_configuracion(int nivel_actual, char tipo, configuracion_t configuracion) {
    int defensores;
    if (tipo == ENANOS) {
        defensores = configuracion.enanos_inicio[nivel_actual-1];
        if (defensores == INDEFINIDO) {
          if (nivel_actual == PRIMER_NIVEL) defensores = ENANOS_INICIALES_PRIMER_NIVEL;
          else if (nivel_actual == SEGUND_NIVEL) defensores = ENANOS_INICIALES_SEGUND_NIVEL;
          else if (nivel_actual == TERCER_NIVEL) defensores = ENANOS_INICIALES_TERCER_NIVEL;
          else defensores = ENANOS_INICIALES_CUARTO_NIVEL;
        }
    } else  {
        defensores = configuracion.elfos_inicio[nivel_actual-1];
        if (defensores == INDEFINIDO) {
          if (nivel_actual == PRIMER_NIVEL) defensores = ELFOS_INICIALES_PRIMER_NIVEL;
          else if (nivel_actual == SEGUND_NIVEL) defensores = ELFOS_INICIALES_SEGUND_NIVEL;
          else if (nivel_actual == TERCER_NIVEL) defensores = ELFOS_INICIALES_TERCER_NIVEL;
          else defensores = ELFOS_INICIALES_CUARTO_NIVEL;
        }
    }
    return defensores;
}

//~ Pre: Recibe un juego en un nuevo nivel por inicializar, la cantidad de defensores de un tipo y el tipo de defensores que deben añadirse.
//~ Pos: Le permite al usuario elegir dónde ubicar a los defensores hasta cumplir con el número de defensores iniciales.
void agregar_defensores_iniciales(juego_t *juego, int cantidad, char tipo) {
    int i = SIN_DEFENSORES;
    coordenada_t posicion;
    while (i < cantidad) {
        limpiar_y_mostrar(*juego, DELAY);
        printf("\nVa a agregar un %c\n", tipo);
        pedir_coordenada(&posicion, juego->nivel_actual);
        if (agregar_defensor(&(juego->nivel), posicion, tipo) == AGREGADO) i++;
        else printf("\nLa posición está ocupada\n");
    }
}

//~ Pre: Recibe un juego en nuevo nivel por inicializar y las indicaciones de defensores iniciales para añadirse.
//~ Pos: Inicializa todos los defensores iniciales.
void cargar_defensores_iniciales(juego_t *juego, configuracion_t configuracion) {
    juego->nivel.tope_defensores = SIN_DEFENSORES;
    int enanos_iniciales = defensores_segun_configuracion(juego->nivel_actual, ENANOS, configuracion);
    int elfos_iniciales = defensores_segun_configuracion(juego->nivel_actual, ELFOS, configuracion);
    agregar_defensores_iniciales(juego, enanos_iniciales, ENANOS);
    agregar_defensores_iniciales(juego, elfos_iniciales, ELFOS);
}

/*
 * Pre: Recibe el tipo de defensores, el número de torre y la configuración elegida.
 * Pos: Devuelve el costo que tendría para la torre, usar un defensor, según la configuración.
 *      Los valores INDEFINIDOs toman los valores por default.
 */
int costo_defensor_segun_configuracion(char tipo, int torre, configuracion_t configuracion){
    int costo;
    if (tipo == ENANOS && torre == TORRE_UNO) {
        costo = configuracion.costo_enanos_torre_1;
        if (costo == INDEFINIDO) costo = COSTO_ENANOS_TORRE_UNO;
    } else if (tipo == ENANOS && torre == TORRE_DOS) {
        costo = configuracion.costo_enanos_torre_2;
        if (costo == INDEFINIDO) costo = COSTO_ENANOS_TORRE_DOS;
    } else if (tipo == ELFOS && torre == TORRE_UNO) {
        costo = configuracion.costo_elfos_torre_1;
        if (costo == INDEFINIDO) costo = COSTO_ELFOS_TORRE_UNO;
    } else {
        costo = configuracion.costo_elfos_torre_2;
        if (costo == INDEFINIDO) costo = COSTO_ELFOS_TORRE_DOS;
    }
    return costo;
}

//~ Pre: Recibe la instancia de un juego y la de sus torres, y la configuración.
//~ Pos: Si el nivel lo permite y las torres tienen suficientes recursos, permite agregar un enano al juego.
bool puede_agregar_enanos(torres_t torres, configuracion_t configuracion) {
    int costo_torre_1 = costo_defensor_segun_configuracion(ENANOS, TORRE_UNO, configuracion);
    int costo_torre_2 = costo_defensor_segun_configuracion(ENANOS, TORRE_DOS, configuracion);
    return (
        (costo_torre_1 > COSTO_INVALIDO || costo_torre_2 > COSTO_INVALIDO)
        && (torres.resistencia_torre_1 > costo_torre_1)
        && (torres.resistencia_torre_2 > costo_torre_2)
        && (torres.enanos_extra > SIN_DEFENSORES)
    );
}

//~ Pre: Recibe la instancia de un juego y la de sus torres, y la configuración.
//~ Pos: Si el nivel lo permite y las torres tienen suficientes recursos, permite agregar un elfo al juego.
bool puede_agregar_elfos(torres_t torres, configuracion_t configuracion) {
    int costo_torre_1 = costo_defensor_segun_configuracion(ELFOS, TORRE_UNO, configuracion);
    int costo_torre_2 = costo_defensor_segun_configuracion(ELFOS, TORRE_DOS, configuracion);
    return (
        (costo_torre_1 > COSTO_INVALIDO || costo_torre_2 > COSTO_INVALIDO)
        && (torres.resistencia_torre_1 > costo_torre_1)
        && (torres.resistencia_torre_2 > costo_torre_2)
        && (torres.elfos_extra > SIN_DEFENSORES)
    );
}

//~ Pre: Recibe la instancia de un juego, la de sus torres y la configuracion.
//~ Pos: Devuelve verdadero si se permite agregar un elfo o un enano al juego.
bool hay_extra_disponible(torres_t torres, configuracion_t configuracion) {
    return puede_agregar_enanos(torres, configuracion) || puede_agregar_elfos(torres, configuracion);
}

//~ Pre: Recibe la cantidad de enemigos que salieron en el nivel, si quedan defensores extras
//~      y la configuración.
//~ Pos: Devuelve verdadero si se dan todas las condiciones para agregar un defensor extra al juego.
bool se_puede_agregar_extra(juego_t juego, configuracion_t configuracion) {
    bool primera_condicion = hay_extra_disponible(juego.torres, configuracion);
    bool segunda_condicion = juego.nivel.tope_enemigos > 0 && juego.nivel.tope_enemigos < juego.nivel.max_enemigos_nivel;
    bool tercera_condicion;
    if (primer_nivel(juego))
        tercera_condicion = juego.nivel.tope_enemigos % ORCOS_PARA_AGREGAR_PRIMERO == 0;
    else
        tercera_condicion = juego.nivel.tope_enemigos % ORCOS_PARA_AGREGAR_DEFENSOR == 0;

    return (primera_condicion && segunda_condicion && tercera_condicion);
}

//~ Pre: Recibe un tipo ingresado por el usuario y las cantidades de defensores extras que
//~      hay disponibles y su costo por configuración.
//~ Pos: Devuelve verdadero si puede agregar el tipo pedido o el usuario quiere IGNORAR el mensaje.
bool es_tipo_valido(char tipo, torres_t torres, configuracion_t configuracion) {
    return (
        (tipo == ENANOS && puede_agregar_enanos(torres, configuracion))
        || (tipo == ELFOS && puede_agregar_enanos(torres, configuracion))
        || (tipo == IGNORAR)
    );
}

//~ Pre: Recibe una instancia de juego y un tipo de defensor sin inicializar, y la configuración.
//~ Pos: El usuario ingresa un tipo valido, para agregar un defensor o ignorar el mensaje.
void pedir_tipo(torres_t torres, char *tipo, configuracion_t configuracion) {
    bool hay_enanos_disponibles = puede_agregar_enanos(torres, configuracion);
    bool hay_elfos_disponibles = puede_agregar_elfos(torres, configuracion);

    printf("\nPodes agregar un defensor de las tropas de");
    if (hay_enanos_disponibles)
        printf(" %c (%i)", ENANOS, torres.enanos_extra);

    if (hay_elfos_disponibles) {
        if (hay_enanos_disponibles)
            printf(" o de las tropas de");
        printf(" %c (%i)", ELFOS, torres.elfos_extra);
    }
    printf("\nRecuerda que agregar un %c costará %i a la Torre1 y %i a la Torre 2.\n", ENANOS,
            costo_defensor_segun_configuracion(ENANOS, TORRE_UNO, configuracion),
            costo_defensor_segun_configuracion(ENANOS, TORRE_DOS, configuracion));
    printf("\nAgregar un %c costará %i a la Torre1 y %i a la Torre 2. \n", ELFOS,
            costo_defensor_segun_configuracion(ELFOS, TORRE_UNO, configuracion),
            costo_defensor_segun_configuracion(ELFOS, TORRE_DOS, configuracion));
    printf("\nAgregar ");
    if (hay_enanos_disponibles) printf("%c/", ENANOS);
    if (hay_elfos_disponibles) printf("%c/", ELFOS);
    printf("%c(ignorar): ", IGNORAR);

    scanf(" %c", tipo);
    while (!es_tipo_valido(*tipo, torres, configuracion)) {
        printf("Intente de nuevo: ");
        if (hay_enanos_disponibles) printf("%c/", ENANOS);
        if (hay_elfos_disponibles) printf("%c/", ELFOS);
        printf("%c(ignorar): ", IGNORAR);
        scanf(" %c", tipo);
    }
}

//~ Elimina un enano extra y resta los puntos de las torres.
void descontar_enano(torres_t *torres, configuracion_t configuracion) {
    torres->resistencia_torre_1 -= costo_defensor_segun_configuracion(ENANOS, TORRE_UNO, configuracion);
    torres->resistencia_torre_2 -= costo_defensor_segun_configuracion(ENANOS, TORRE_DOS, configuracion);
    torres->enanos_extra--;
}

//~ Elimina un elfo extra y resta los puntos de las torres.
void descontar_elfo(torres_t *torres, configuracion_t configuracion) {
    torres->resistencia_torre_1 -= costo_defensor_segun_configuracion(ELFOS, TORRE_DOS, configuracion);
    torres->resistencia_torre_2 -= costo_defensor_segun_configuracion(ELFOS, TORRE_UNO, configuracion);
    torres->elfos_extra--;
}

//~ Pre: Recibe un juego cuando es permitido agregar un defensor extra de las torres.
//~ Pos: Agrega un defensor desde las torres al juego y lo descuenta de estas.
void agregar_defensor_extra(juego_t *juego, configuracion_t configuracion) {
    char tipo;
    coordenada_t posicion;
    pedir_tipo(juego->torres, &tipo, configuracion);
    if (tipo == IGNORAR) {
        printf("\nNo se han agregado defensores.\n");
        return;
    }
    pedir_coordenada(&posicion, juego->nivel_actual);
    while (agregar_defensor(&(juego->nivel), posicion, tipo) != AGREGADO) {
        printf("\nLa posición está ocupada\n");
        pedir_coordenada(&posicion, juego->nivel_actual);
    }
    if (tipo == ENANOS) descontar_enano(&(juego->torres), configuracion);
    else descontar_elfo(&(juego->torres), configuracion);
}

//~ Pre: Recibe un juego en un nuevo nivel por inicializar.
//~ Pos: Elimina los enemigos de una instancia anterior y define max_enemigos_nivel de nivel.
void cargar_enemigos(juego_t *juego){
    juego->nivel.tope_enemigos = SIN_ENEMIGOS;
    if (primer_nivel(*juego)) juego->nivel.max_enemigos_nivel = MAX_ENEMIGOS_PRIMER_NIVEL;
    else if (segundo_nivel(*juego)) juego->nivel.max_enemigos_nivel = MAX_ENEMIGOS_SEGUND_NIVEL;
    else if (tercer_nivel(*juego)) juego->nivel.max_enemigos_nivel = MAX_ENEMIGOS_TERCER_NIVEL;
    else juego->nivel.max_enemigos_nivel = MAX_ENEMIGOS_CUARTO_NIVEL;
}

//~ Selección de mensajes para mostrar al inicio de cada nivel.
void mostrar_intro_nivel(configuracion_t configuracion) {
    int defensores = defensores_segun_configuracion(configuracion.juego.nivel_actual, ENANOS, configuracion);
    defensores += defensores_segun_configuracion(configuracion.juego.nivel_actual, ELFOS, configuracion);
    if (primer_nivel(configuracion.juego)) {
        esperar();
        mostrar_ayuda(configuracion.juego);
        printf("\nLos enemigos vienen por el este! \nPlanean atacar la torre 1. \nSe necesitan %i defensores para protegerla\n", defensores);
    } else if (segundo_nivel(configuracion.juego)) {
        printf("\nVienen más orcos! \nAhora se acercan por el oeste para atacar la torre 2! \nPosicione %i defensores para detenerlos\n", defensores);
    } else if (tercer_nivel(configuracion.juego)) {
        printf("\nOh no! Los enemigos siguen acercándose, esta vez por el norte. \nPueden atacar las dos torres! Ubique %i defensores para protegerlas!\n", defensores);
    } else {
        printf("\nTodavía hay más?!!! Los enemigos vienen por el sur! \nSe dirigen a ambas torres de nuevo. Ubique %i defensores para defenderlas\n", defensores);
    }
    esperar();
}

//~ Pre: Recibe el juego de una configuración junto a toda configuración.
//~ Pos: Inicializa un nuevo nivel modificando caminos, renovando defensores y
//~      eliminando enemigos previos. Introduce el nivel con un mensaje.
int cargar_nivel(configuracion_t *configuracion) {
    mostrar_intro_nivel(*configuracion);
    if (cargar_caminos(&(configuracion->juego), configuracion->caminos) == ERROR) return ERROR;
    cargar_enemigos(&(configuracion->juego));
    cargar_defensores_iniciales(&(configuracion->juego), *configuracion);
    return !ERROR;
}

//~ Pre: Recibe el estado del juego una vez finalizado.
//~ Pos: Muestra un mensaje según el estado GANADO o PERDIDO.
void mostrar_mensaje_final(int estado_juego) {
    for (int i=MAXIMO_MATRIZ_MAYOR; i>=0; i--) {
        detener_el_tiempo(DELAY);
        system("clear");
        for (int j = i; j>=0; j--)
            printf("\n");
        if (estado_juego == GANADO)
            printf("FELICIDADES! HAS COMPLETADO EL JUEGO CON ÉXITO\n");
        else
            printf("Una de las torres ha caído...\nGame over\n");

        printf("\n\n\nGracias por jugar!!\n");
        printf("\n\nEscrito, producido y guionado por %s ;)\n", AUTOR);
        printf("\n%s.\n", FECHA);
    }
}

/*
 * Pide el nombre del jugador para guardar su puntaje.
 */
void pedir_nombre(char nombre[MAX_NOMBRE]) {
    printf("Ingrese su nombre: ");
    scanf("%s", nombre);
    while (strlen(nombre) >= MAX_NOMBRE) {
        printf("Ingresa un nombre más corto: ");
        scanf("%s", nombre);
    }
}

/*
 * Pre: Recibe el juego terminado.
 * Pos: Devuelve la cantidad de enemigos que están muertos.
 */
int cantidad_de_enemigos_muertos(juego_t juego) {
    int i, orcos_muertos = SIN_ENEMIGOS;

    for (i = 0; i < juego.nivel.tope_enemigos; i++)
        if (juego.nivel.enemigos[i].vida <= 0) orcos_muertos++;

    switch(juego.nivel_actual) {
        case CUARTO_NIVEL:
            orcos_muertos += MAX_ENEMIGOS_TERCER_NIVEL;
        case TERCER_NIVEL:
            orcos_muertos += MAX_ENEMIGOS_SEGUND_NIVEL;
        case SEGUND_NIVEL:
            orcos_muertos += MAX_ENEMIGOS_PRIMER_NIVEL;
    }
    return orcos_muertos;
}

/*
 * Pre: Recibe el valor de las torres después de ser inicializadas.
 * Pos: Devuelve la suma de diferentes valores de la configuración
 *      que facilitan el juego.
 */
int recursos_usados (torres_t torres, configuracion_t configuracion) {
    int i, recursos = torres.resistencia_torre_1;
    recursos += torres.resistencia_torre_2;
    recursos += torres.enanos_extra;
    recursos += 2 * torres.elfos_extra;
    for (i = 0; i < MAX_NIVELES; i++) {
        recursos += defensores_segun_configuracion(i+1, ENANOS, configuracion);
        recursos += 2 * defensores_segun_configuracion(i+1, ELFOS, configuracion);
    }
    return recursos;
}

/*
 * Pre: Recibe el juego en su estado final y los recursos que se contabilizaron de
 *      la configuración.
 * Pos: Se obtiene la puntuación final y el usuario guarda su nombre en el rank.
 */
void obtener_rank(rank_t *rank, juego_t juego, int recursos) {
    rank->puntaje = (cantidad_de_enemigos_muertos(juego) * 1000) / recursos;
    printf("\nLograste %i puntos\n", rank->puntaje);
    pedir_nombre(rank->nombre);
}

/*
 * Definido en juego.h
 */
int reproducir_juego(char grabacion[], float velocidad) {
    juego_t juego;
    FILE* archivo;
    archivo = fopen(grabacion, LECTURA);
    if (!archivo) {
        printf("No se pudo abrir la grabación.\n");
        return ERROR;
    }
    if (velocidad == INDEFINIDO) velocidad = DELAY_REPRODUCCION;
    fread(&juego, sizeof(juego_t), 1, archivo);
    while (!feof(archivo)) {
        limpiar_y_mostrar(juego, velocidad);
        fread(&juego, sizeof(juego_t), 1, archivo);
    }
    fclose(archivo);
    return !ERROR;
}

/*
 * Pre: Recibe el juego de una configuración.
 * Pos: Devuelve true si alguno de los valores quedaron indefinidos.
 */
bool hay_valores_indefinidos(juego_t juego) {
    return (
        juego.torres.resistencia_torre_1 == INDEFINIDO
        || juego.torres.resistencia_torre_2 == INDEFINIDO
        || juego.torres.enanos_extra == INDEFINIDO
        || juego.torres.elfos_extra == INDEFINIDO
        || juego.critico_gimli == INDEFINIDO
        || juego.critico_legolas == INDEFINIDO
        || juego.fallo_gimli == INDEFINIDO
        || juego.fallo_legolas ==   INDEFINIDO
    );
}

/*
 * Pre: Recibe el juego de una configuración.
 * Pos: Comprueba si quedó alguno de los valores indefinidos para ejecutar el
 *      juego normal y obtener los datos faltantes.
 */
void inicializar_config_indefinida(juego_t *juego) {
    int viento, humedad;
    char animo_legolas, animo_gimli;
    if (hay_valores_indefinidos(*juego)) {
        animos(&viento, &humedad, &animo_legolas, &animo_gimli);
        inicializar_juego(juego, viento, humedad, animo_legolas, animo_gimli);
    }
    juego->nivel_actual = PRIMER_NIVEL;
}

/*
 * Pre: Recibe la configuración y el juego inicializado. Puede recibir
 *      la ruta de un archivo para guardar la grabación de la partida.
 * Pos: Trascurre la ejecución del juego. Devuelve 0 si no hay errores.
 */
int jugar(configuracion_t *configuracion, char grabacion[MAX_ARCHIVO]) {
    FILE* archivo = NULL;
    if (strcmp(grabacion, SIN_ARCHIVO)) {
        archivo = fopen(grabacion, ESCRITURA);
        if (!archivo) {
            printf("No se pudo iniciar la grabacion\n");
            return ERROR;
        }
    }
    cargar_nivel(configuracion);
    if (archivo) fwrite(&(configuracion->juego), sizeof(juego_t), 1, archivo);
    while (estado_juego(configuracion->juego) == JUGANDO) {
        if (estado_nivel(configuracion->juego.nivel) == GANADO) {
            (configuracion->juego.nivel_actual)++;
            cargar_nivel(configuracion);
        }
        limpiar_y_mostrar(configuracion->juego, configuracion->velocidad);
        if (se_puede_agregar_extra(configuracion->juego, *configuracion)) {
            agregar_defensor_extra(&(configuracion->juego), *configuracion);
            detener_el_tiempo(DELAY);
            limpiar_y_mostrar(configuracion->juego, configuracion->velocidad);
            }
        jugar_turno(&(configuracion->juego));
        if (archivo) fwrite(&(configuracion->juego), sizeof(juego_t), 1, archivo);
    }
    if (archivo) fclose(archivo);
    return !ERROR;
}

/*
 * Muestra mensaje al terminar el juego y obtiene la puntuación y el nombre del jugador.
 */
int terminar_juego(rank_t *rank, juego_t juego, int recursos) {
    mostrar_mensaje_final(estado_juego(juego));
    obtener_rank(rank, juego, recursos);
    return !ERROR;
}

/*
 * Definido en juego.h
 */
int iniciar_juego(configuracion_t configuracion, char grabacion[MAX_ARCHIVO], rank_t *rank) {
    int recursos;
    inicializar_config_indefinida(&(configuracion.juego));
    recursos = recursos_usados(configuracion.juego.torres, configuracion);
    return (
        (jugar(&configuracion, grabacion) == ERROR)
        || (terminar_juego(rank, configuracion.juego, recursos) == ERROR)
    );
}
