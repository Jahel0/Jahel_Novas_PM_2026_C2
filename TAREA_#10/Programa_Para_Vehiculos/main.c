/* =========================================================================
   PROYECTO FINAL: GESTOR DE COSTOS DE VEHICULOS
   ---------------------------------------------------------------------
   Este programa permite:
     1. Crear vehiculos con todos sus parametros y guardarlos en un archivo.
     2. Listar los vehiculos guardados.
     3. Modificar los parametros de un vehiculo existente.
     4. Modificar unicamente el precio del combustible de un vehiculo.
     5. Borrar un vehiculo.
     6. Calcular el costo real por km (en ciudad y en autopista) de un
        vehiculo, considerando amortizacion, mantenimiento, neumaticos,
        seguro y combustible.
     7. Calcular el costo real de un viaje especifico, segun los km
        recorridos en ciudad y en autopista.

   El mantenimiento se calcula de forma desglosada, ya que cada vehiculo
   puede necesitar (o no) cada uno de los siguientes servicios:
     - Cambio de aceite de motor y filtro (varia segun litros de aceite)
     - Filtro de aire
     - Filtro de gasoil (solo vehiculos diesel)
     - Filtro de aire de cabina
     - Cambio de bandas/correas
     - Servicio de frenos
     - Revision general (luces y otros chequeos menores)
   ========================================================================= */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------- CONSTANTES DEL PROGRAMA ---------------------- */
#define MAX_VEHICULOS           100
#define MAX_NOMBRE                50
#define MAX_LINEA               1024
#define ARCHIVO_VEHICULOS       "vehiculos.txt"
#define INTERVALO_ACEITE_SUGERIDO 5000

/* ------------------------- ESTRUCTURAS DE DATOS ------------------------ */

/* Representa un item de mantenimiento individual: si aplica al vehiculo,
   cuanto cuesta, y cada cuantos km se repite. */
typedef struct {
    int   aplica;        /* 1 = el vehiculo necesita este servicio, 0 = no */
    float costo;         /* costo del servicio o repuesto                  */
    long  intervaloKm;    /* cada cuantos km se repite este servicio        */
} ItemMantenimiento;

/* Agrupa todos los items de mantenimiento de un vehiculo. */
typedef struct {
    /* Datos propios del cambio de aceite (varian segun el motor) */
    float litrosAceite;
    float precioLitroAceite;
    float costoFiltroAceite;

    ItemMantenimiento aceiteFiltro;
    ItemMantenimiento filtroAire;
    ItemMantenimiento filtroGasoil;
    ItemMantenimiento filtroCabina;
    ItemMantenimiento bandas;
    ItemMantenimiento frenos;
    ItemMantenimiento revisionGeneral;
} Mantenimiento;

/* Representa un vehiculo completo con todos sus parametros. */
typedef struct {
    int   id;
    char  nombre[MAX_NOMBRE];
    float costoInicial;
    float valorResidual;
    int   vidaUtilAnios;
    long  vidaUtilKm;
    float consumoCiudad;       /* km recorridos por galon/litro en ciudad    */
    float consumoAutopista;    /* km recorridos por galon/litro en autopista */
    float precioCombustible;
    float seguroAnual;
    float costoNeumaticos;
    long  duracionNeumaticosKm;
    Mantenimiento mant;
} Vehiculo;

/* ---------------------------- PROTOTIPOS -------------------------------- */

/* Entrada validada de datos */
void  limpiarBuffer(void);
int   leerEntero(const char *mensaje, int minimo);
long  leerLong(const char *mensaje, long minimo);
float leerFlotante(const char *mensaje, float minimo);
void  leerCadena(const char *mensaje, char *destino, int tam);
int   leerSiNo(const char *mensaje);

/* Manejo de vehiculos en memoria */
int   obtenerNuevoId(Vehiculo lista[], int cantidad);
int   buscarVehiculoPorId(Vehiculo lista[], int cantidad, int id);

/* Manejo de archivo */
int   cargarVehiculos(Vehiculo lista[], int max);
int   parsearLinea(char *linea, Vehiculo *v);
void guardarTodos(Vehiculo lista[], int cantidad);
void guardarVehiculoEnArchivo(FILE *f, Vehiculo v);

/* Captura de datos de mantenimiento */
ItemMantenimiento leerItemMantenimiento(const char *nombreItem);
void              leerMantenimientoAceite(Mantenimiento *m);
Mantenimiento leerMantenimiento(void);
Vehiculo      leerDatosVehiculo(int id);

/* Operaciones principales del menu */
void crearVehiculo(Vehiculo lista[], int *cantidad);
void listarVehiculos(Vehiculo lista[], int cantidad);
void modificarVehiculo(Vehiculo lista[], int cantidad);
void modificarPrecioCombustible(Vehiculo lista[], int cantidad);
void borrarVehiculo(Vehiculo lista[], int *cantidad);

/* Calculos de costos */
float calcularAmortizacionPorKm(Vehiculo v);
float calcularMantenimientoPorKm(Vehiculo v);
float calcularNeumaticosPorKm(Vehiculo v);
float calcularSeguroPorKm(Vehiculo v);
float calcularCombustiblePorKmCiudad(Vehiculo v);
float calcularCombustiblePorKmAutopista(Vehiculo v);
void  calcularCostoViaje(Vehiculo v, float kmCiudad, float kmAutopista,
                         float *costoCombustible, float *costoTotal);
void  mostrarCostoPorKm(Vehiculo lista[], int cantidad);
void  menuCalcularViaje(Vehiculo lista[], int cantidad);

/* Menu */
void mostrarMenu(void);


/* ========================================================================
   FUNCIONES DE ENTRADA VALIDADA
   ======================================================================== */

void limpiarBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        /* descarta caracteres sobrantes */
    }
}

int leerEntero(const char *mensaje, int minimo) {
    int valor;
    int valido;
    do {
        printf("%s", mensaje);
        valido = scanf("%d", &valor);
        limpiarBuffer();
        if (!valido) {
            printf("  Entrada invalida. Debe ingresar un numero entero.\n");
            continue;
        }
        if (valor < minimo) {
            printf("  El valor debe ser mayor o igual a %d.\n", minimo);
        }
    } while (!valido || valor < minimo);
    return valor;
}

long leerLong(const char *mensaje, long minimo) {
    long valor;
    int  valido;
    do {
        printf("%s", mensaje);
        valido = scanf("%ld", &valor);
        limpiarBuffer();
        if (!valido) {
            printf("  Entrada invalida. Debe ingresar un numero entero.\n");
            continue;
        }
        if (valor < minimo) {
            printf("  El valor debe ser mayor o igual a %ld.\n", minimo);
        }
    } while (!valido || valor < minimo);
    return valor;
}

float leerFlotante(const char *mensaje, float minimo) {
    float valor;
    int   valido;
    do {
        printf("%s", mensaje);
        valido = scanf("%f", &valor);
        limpiarBuffer();
        if (!valido) {
            printf("  Entrada invalida. Debe ingresar un numero.\n");
            continue;
        }
        if (valor < minimo) {
            printf("  El valor debe ser mayor o igual a %.2f.\n", minimo);
        }
    } while (!valido || valor < minimo);
    return valor;
}

void leerCadena(const char *mensaje, char *destino, int tam) {
    int valido;
    do {
        printf("%s", mensaje);
        if (fgets(destino, tam, stdin) == NULL) {
            destino[0] = '\0';
        }
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') {
            destino[len - 1] = '\0';
        }

        valido = 1;
        if (strlen(destino) == 0) {
            printf("  El texto no puede estar vacio.\n");
            valido = 0;
        } else if (strchr(destino, ';') != NULL) {
            printf("  El texto no puede contener el caracter ';'.\n");
            valido = 0;
        }
    } while (!valido);
}

int leerSiNo(const char *mensaje) {
    char respuesta[10];
    int  valido;
    do {
        printf("%s", mensaje);
        if (fgets(respuesta, sizeof(respuesta), stdin) == NULL) {
            respuesta[0] = '\0';
        }
        valido = 1;
        if (respuesta[0] == 's' || respuesta[0] == 'S') {
            return 1;
        } else if (respuesta[0] == 'n' || respuesta[0] == 'N') {
            return 0;
        } else {
            printf("  Responda con 's' o 'n'.\n");
            valido = 0;
        }
    } while (!valido);
    return 0;
}


/* ========================================================================
   MANEJO DE VEHICULOS EN MEMORIA
   ======================================================================== */

int obtenerNuevoId(Vehiculo lista[], int cantidad) {
    int maxId = 0;
    int i;
    for (i = 0; i < cantidad; i++) {
        if (lista[i].id > maxId) {
            maxId = lista[i].id;
        }
    }
    return maxId + 1;
}

int buscarVehiculoPorId(Vehiculo lista[], int cantidad, int id) {
    int i;
    for (i = 0; i < cantidad; i++) {
        if (lista[i].id == id) {
            return i;
        }
    }
    return -1;
}


/* ========================================================================
   MANEJO DE ARCHIVO (PERSISTENCIA)
   ======================================================================== */

void guardarVehiculoEnArchivo(FILE *f, Vehiculo v) {
    fprintf(f, "%d;%s;%.2f;%.2f;%d;%ld;%.2f;%.2f;%.2f;%.2f;%.2f;%ld;",
            v.id, v.nombre, v.costoInicial, v.valorResidual,
            v.vidaUtilAnios, v.vidaUtilKm, v.consumoCiudad, v.consumoAutopista,
            v.precioCombustible, v.seguroAnual,
            v.costoNeumaticos, v.duracionNeumaticosKm);

    fprintf(f, "%.2f;%.2f;%.2f;",
            v.mant.litrosAceite, v.mant.precioLitroAceite, v.mant.costoFiltroAceite);

    ItemMantenimiento items[7];
    items[0] = v.mant.aceiteFiltro;
    items[1] = v.mant.filtroAire;
    items[2] = v.mant.filtroGasoil;
    items[3] = v.mant.filtroCabina;
    items[4] = v.mant.bandas;
    items[5] = v.mant.frenos;
    items[6] = v.mant.revisionGeneral;

    int i;
    for (i = 0; i < 7; i++) {
        fprintf(f, "%d;%.2f;%ld", items[i].aplica, items[i].costo, items[i].intervaloKm);
        if (i < 6) {
            fprintf(f, ";");
        }
    }
    fprintf(f, "\n");
}

void guardarTodos(Vehiculo lista[], int cantidad) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "w");
    if (f == NULL) {
        printf("Error: no se pudo abrir el archivo para guardar los datos.\n");
        return;
    }
    int i;
    for (i = 0; i < cantidad; i++) {
        guardarVehiculoEnArchivo(f, lista[i]);
    }
    fclose(f);
}

/* Parseo corregido */
int parsearLinea(char *linea, Vehiculo *v) {
    char *tok;

    tok = strtok(linea, ";\r\n");
    if (tok == NULL) return 0;
    v->id = atoi(tok);

    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0;
    strncpy(v->nombre, tok, MAX_NOMBRE - 1);
    v->nombre[MAX_NOMBRE - 1] = '\0';

    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->costoInicial = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->valorResidual = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->vidaUtilAnios = atoi(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->vidaUtilKm = atol(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->consumoCiudad = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->consumoAutopista = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->precioCombustible = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->seguroAnual = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->costoNeumaticos = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->duracionNeumaticosKm = atol(tok);

    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->mant.litrosAceite = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->mant.precioLitroAceite = (float)atof(tok);
    tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; v->mant.costoFiltroAceite = (float)atof(tok);

    /* Arreglado: solo asignamos punteros a los 7 items sin repetir la lectura de aceite */
    ItemMantenimiento *items[7];
    items[0] = &v->mant.aceiteFiltro;
    items[1] = &v->mant.filtroAire;
    items[2] = &v->mant.filtroGasoil;
    items[3] = &v->mant.filtroCabina;
    items[4] = &v->mant.bandas;
    items[5] = &v->mant.frenos;
    items[6] = &v->mant.revisionGeneral;

    int i;
    for (i = 0; i < 7; i++) {
        tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; items[i]->aplica = atoi(tok);
        tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; items[i]->costo = (float)atof(tok);
        tok = strtok(NULL, ";\r\n"); if (tok == NULL) return 0; items[i]->intervaloKm = atol(tok);
    }

    return 1;
}

int cargarVehiculos(Vehiculo lista[], int max) {
    FILE *f = fopen(ARCHIVO_VEHICULOS, "r");
    if (f == NULL) {
        return 0;
    }

    char linea[MAX_LINEA];
    int  cantidad = 0;

    while (fgets(linea, sizeof(linea), f) != NULL && cantidad < max) {
        if (strlen(linea) < 2) {
            continue;
        }
        Vehiculo v;
        if (parsearLinea(linea, &v)) {
            lista[cantidad] = v;
            cantidad++;
        } else {
            printf("Aviso: se encontro una linea incompleta en el archivo y fue ignorada.\n");
        }
    }

    fclose(f);
    return cantidad;
}


/* ========================================================================
   CAPTURA DE DATOS DE MANTENIMIENTO
   ======================================================================== */

ItemMantenimiento leerItemMantenimiento(const char *nombreItem) {
    ItemMantenimiento item;
    char pregunta[150];

    printf("\n-- %s --\n", nombreItem);
    snprintf(pregunta, sizeof(pregunta), "  Este vehiculo necesita este servicio? (s/n): ");
    item.aplica = leerSiNo(pregunta);

    if (item.aplica) {
        item.costo = leerFlotante("  Costo del servicio/repuesto: ", 0.0f);
        item.intervaloKm = leerLong("  Cada cuantos km se repite (sugerido 5000): ", 1);
    } else {
        item.costo = 0.0f;
        item.intervaloKm = 1;
    }
    return item;
}

void leerMantenimientoAceite(Mantenimiento *m) {
    printf("\n-- Cambio de aceite de motor y filtro --\n");
    m->aceiteFiltro.aplica = leerSiNo("  Este vehiculo necesita cambio de aceite y filtro? (s/n): ");

    if (m->aceiteFiltro.aplica) {
        m->litrosAceite = leerFlotante("  Litros de aceite que usa el motor: ", 0.1f);
        m->precioLitroAceite = leerFlotante("  Precio por litro de aceite: ", 0.0f);
        m->costoFiltroAceite = leerFlotante("  Costo del filtro de aceite: ", 0.0f);
        m->aceiteFiltro.intervaloKm =
            leerLong("  Cada cuantos km se cambia (sugerido 5000): ", 1);
        m->aceiteFiltro.costo =
            m->litrosAceite * m->precioLitroAceite + m->costoFiltroAceite;
    } else {
        m->litrosAceite = 0.0f;
        m->precioLitroAceite = 0.0f;
        m->costoFiltroAceite = 0.0f;
        m->aceiteFiltro.costo = 0.0f;
        m->aceiteFiltro.intervaloKm = 1;
    }
}

Mantenimiento leerMantenimiento(void) {
    Mantenimiento m;

    leerMantenimientoAceite(&m);
    m.filtroAire       = leerItemMantenimiento("Filtro de aire del motor");
    m.filtroGasoil     = leerItemMantenimiento("Filtro de gasoil (solo vehiculos diesel)");
    m.filtroCabina     = leerItemMantenimiento("Filtro de aire de cabina");
    m.bandas           = leerItemMantenimiento("Cambio de bandas/correas");
    m.frenos           = leerItemMantenimiento("Servicio de frenos (pastillas/discos)");
    m.revisionGeneral = leerItemMantenimiento("Revision general (luces y otros chequeos)");

    return m;
}

Vehiculo leerDatosVehiculo(int id) {
    Vehiculo v;
    v.id = id;

    printf("\n===== DATOS DEL VEHICULO =====\n");
    leerCadena("Nombre del vehiculo: ", v.nombre, MAX_NOMBRE);
    v.costoInicial      = leerFlotante("Costo de compra: ", 0.0f);
    v.valorResidual     = leerFlotante("Valor de reventa al final de la vida util: ", 0.0f);
    v.vidaUtilAnios     = leerEntero("Vida util en anios: ", 1);
    v.vidaUtilKm        = leerLong("Vida util en kilometros: ", 1);
    v.consumoCiudad     = leerFlotante("Consumo en ciudad (km por galon/litro): ", 0.1f);
    v.consumoAutopista  = leerFlotante("Consumo en autopista (km por galon/litro): ", 0.1f);
    v.precioCombustible = leerFlotante("Precio del combustible: ", 0.01f);
    v.seguroAnual       = leerFlotante("Costo del seguro anual: ", 0.0f);
    v.costoNeumaticos   = leerFlotante("Costo de un juego de neumaticos: ", 0.0f);
    v.duracionNeumaticosKm = leerLong("Duracion de los neumaticos en km: ", 1);

    printf("\n===== MANTENIMIENTO DEL VEHICULO =====\n");
    printf("(Responda 's' o 'n' segun si el vehiculo necesita cada servicio)\n");
    v.mant = leerMantenimiento();

    return v;
}


/* ========================================================================
   OPERACIONES PRINCIPALES DEL MENU (CRUD DE VEHICULOS)
   ======================================================================== */

void crearVehiculo(Vehiculo lista[], int *cantidad) {
    if (*cantidad >= MAX_VEHICULOS) {
        printf("\nNo se pueden agregar mas vehiculos (limite de %d alcanzado).\n", MAX_VEHICULOS);
        return;
    }

    printf("\n========== CREAR NUEVO VEHICULO ==========\n");
    int nuevoId = obtenerNuevoId(lista, *cantidad);
    Vehiculo v = leerDatosVehiculo(nuevoId);

    lista[*cantidad] = v;
    (*cantidad)++;

    guardarTodos(lista, *cantidad);
    printf("\nVehiculo '%s' creado con id %d y guardado correctamente.\n", v.nombre, v.id);
}

void listarVehiculos(Vehiculo lista[], int cantidad) {
    if (cantidad == 0) {
        printf("\nNo hay vehiculos guardados todavia.\n");
        return;
    }

    printf("\n===================== VEHICULOS GUARDADOS =====================\n");
    printf("%-4s %-20s %-12s %-14s\n", "ID", "Nombre", "Costo", "Combustible");
    printf("-----------------------------------------------------------------\n");
    int i;
    for (i = 0; i < cantidad; i++) {
        printf("%-4d %-20s %-12.2f %-14.2f\n",
               lista[i].id, lista[i].nombre, lista[i].costoInicial, lista[i].precioCombustible);
    }
    printf("-----------------------------------------------------------------\n");
}

void modificarVehiculo(Vehiculo lista[], int cantidad) {
    listarVehiculos(lista, cantidad);
    if (cantidad == 0) return;

    int id = leerEntero("\nIngrese el id del vehiculo a modificar: ", 0);
    int idx = buscarVehiculoPorId(lista, cantidad, id);
    if (idx == -1) {
        printf("No existe ningun vehiculo con ese id.\n");
        return;
    }

    int opcion;
    do {
        printf("\n--- Modificando: %s (id %d) ---\n", lista[idx].nombre, lista[idx].id);
        printf("1. Nombre\n");
        printf("2. Costo inicial\n");
        printf("3. Valor residual\n");
        printf("4. Vida util (anios y km)\n");
        printf("5. Consumo en ciudad y autopista\n");
        printf("6. Precio del combustible\n");
        printf("7. Seguro anual\n");
        printf("8. Neumaticos (costo y duracion)\n");
        printf("9. Mantenimiento (reconfigurar todos los items)\n");
        printf("0. Terminar y guardar cambios\n");
        opcion = leerEntero("Opcion: ", 0);

        switch (opcion) {
            case 1:
                leerCadena("Nuevo nombre: ", lista[idx].nombre, MAX_NOMBRE);
                break;
            case 2:
                lista[idx].costoInicial = leerFlotante("Nuevo costo inicial: ", 0.0f);
                break;
            case 3:
                lista[idx].valorResidual = leerFlotante("Nuevo valor residual: ", 0.0f);
                break;
            case 4:
                lista[idx].vidaUtilAnios = leerEntero("Nueva vida util en anios: ", 1);
                lista[idx].vidaUtilKm = leerLong("Nueva vida util en km: ", 1);
                break;
            case 5:
                lista[idx].consumoCiudad = leerFlotante("Nuevo consumo en ciudad: ", 0.1f);
                lista[idx].consumoAutopista = leerFlotante("Nuevo consumo en autopista: ", 0.1f);
                break;
            case 6:
                lista[idx].precioCombustible = leerFlotante("Nuevo precio de combustible: ", 0.01f);
                break;
            case 7:
                lista[idx].seguroAnual = leerFlotante("Nuevo seguro anual: ", 0.0f);
                break;
            case 8:
                lista[idx].costoNeumaticos = leerFlotante("Nuevo costo de neumaticos: ", 0.0f);
                lista[idx].duracionNeumaticosKm = leerLong("Nueva duracion en km: ", 1);
                break;
            case 9:
                printf("\nSe volveran a configurar todos los items de mantenimiento.\n");
                lista[idx].mant = leerMantenimiento();
                break;
            case 0:
                break;
            default:
                printf("Opcion invalida.\n");
        }
    } while (opcion != 0);

    guardarTodos(lista, cantidad);
    printf("\nCambios guardados correctamente.\n");
}

void modificarPrecioCombustible(Vehiculo lista[], int cantidad) {
    listarVehiculos(lista, cantidad);
    if (cantidad == 0) return;

    int id = leerEntero("\nIngrese el id del vehiculo: ", 0);
    int idx = buscarVehiculoPorId(lista, cantidad, id);
    if (idx == -1) {
        printf("No existe ningun vehiculo con ese id.\n");
        return;
    }

    printf("Precio actual del combustible: %.2f\n", lista[idx].precioCombustible);
    float nuevoPrecio = leerFlotante("Nuevo precio del combustible: ", 0.01f);
    lista[idx].precioCombustible = nuevoPrecio;

    guardarTodos(lista, cantidad);
    printf("Precio de combustible actualizado correctamente.\n");
}

void borrarVehiculo(Vehiculo lista[], int *cantidad) {
    listarVehiculos(lista, *cantidad);
    if (*cantidad == 0) return;

    int id = leerEntero("\nIngrese el id del vehiculo a borrar: ", 0);
    int idx = buscarVehiculoPorId(lista, *cantidad, id);
    if (idx == -1) {
        printf("No existe ningun vehiculo con ese id.\n");
        return;
    }

    char pregunta[150];
    snprintf(pregunta, sizeof(pregunta),
             "Seguro que desea borrar '%s'? (s/n): ", lista[idx].nombre);
    if (!leerSiNo(pregunta)) {
        printf("Operacion cancelada.\n");
        return;
    }

    int i;
    for (i = idx; i < *cantidad - 1; i++) {
        lista[i] = lista[i + 1];
    }
    (*cantidad)--;

    guardarTodos(lista, *cantidad);
    printf("Vehiculo eliminado correctamente.\n");
}


/* ========================================================================
   CALCULOS DE COSTOS
   ======================================================================== */

float calcularAmortizacionPorKm(Vehiculo v) {
    return (v.costoInicial - v.valorResidual) / (float)v.vidaUtilKm;
}

float calcularMantenimientoPorKm(Vehiculo v) {
    float total = 0.0f;

    ItemMantenimiento items[7];
    items[0] = v.mant.aceiteFiltro;
    items[1] = v.mant.filtroAire;
    items[2] = v.mant.filtroGasoil;
    items[3] = v.mant.filtroCabina;
    items[4] = v.mant.bandas;
    items[5] = v.mant.frenos;
    items[6] = v.mant.revisionGeneral;

    int i;
    for (i = 0; i < 7; i++) {
        if (items[i].aplica && items[i].intervaloKm > 0) {
            total += items[i].costo / (float)items[i].intervaloKm;
        }
    }
    return total;
}

float calcularNeumaticosPorKm(Vehiculo v) {
    return v.costoNeumaticos / (float)v.duracionNeumaticosKm;
}

float calcularSeguroPorKm(Vehiculo v) {
    return (v.seguroAnual * (float)v.vidaUtilAnios) / (float)v.vidaUtilKm;
}

float calcularCombustiblePorKmCiudad(Vehiculo v) {
    return v.precioCombustible / v.consumoCiudad;
}

float calcularCombustiblePorKmAutopista(Vehiculo v) {
    return v.precioCombustible / v.consumoAutopista;
}

void calcularCostoViaje(Vehiculo v, float kmCiudad, float kmAutopista,
                         float *costoCombustible, float *costoTotal) {
    float amort = calcularAmortizacionPorKm(v);
    float mant  = calcularMantenimientoPorKm(v);
    float neum  = calcularNeumaticosPorKm(v);
    float seg   = calcularSeguroPorKm(v);
    float combCiudad    = calcularCombustiblePorKmCiudad(v);
    float combAutopista = calcularCombustiblePorKmAutopista(v);

    float costoRealCiudad    = amort + mant + neum + seg + combCiudad;
    float costoRealAutopista = amort + mant + neum + seg + combAutopista;

    *costoCombustible = (combCiudad * kmCiudad) + (combAutopista * kmAutopista);
    *costoTotal = (costoRealCiudad * kmCiudad) + (costoRealAutopista * kmAutopista);
}

void mostrarCostoPorKm(Vehiculo lista[], int cantidad) {
    listarVehiculos(lista, cantidad);
    if (cantidad == 0) return;

    int id = leerEntero("\nIngrese el id del vehiculo: ", 0);
    int idx = buscarVehiculoPorId(lista, cantidad, id);
    if (idx == -1) {
        printf("No existe ningun vehiculo con ese id.\n");
        return;
    }

    Vehiculo v = lista[idx];
    float amort = calcularAmortizacionPorKm(v);
    float mant  = calcularMantenimientoPorKm(v);
    float neum  = calcularNeumaticosPorKm(v);
    float seg   = calcularSeguroPorKm(v);
    float combCiudad    = calcularCombustiblePorKmCiudad(v);
    float combAutopista = calcularCombustiblePorKmAutopista(v);

    printf("\n============ COSTO POR KM: %s ============\n", v.nombre);
    printf("Amortizacion:            %8.4f por km\n", amort);
    printf("Mantenimiento:           %8.4f por km\n", mant);
    printf("Neumaticos:              %8.4f por km\n", neum);
    printf("Seguro:                  %8.4f por km\n", seg);
    printf("Combustible (ciudad):    %8.4f por km\n", combCiudad);
    printf("Combustible (autopista): %8.4f por km\n", combAutopista);
    printf("-----------------------------------------------\n");
    printf("COSTO REAL EN CIUDAD:    %8.4f por km\n", amort + mant + neum + seg + combCiudad);
    printf("COSTO REAL EN AUTOPISTA: %8.4f por km\n", amort + mant + neum + seg + combAutopista);
}

void menuCalcularViaje(Vehiculo lista[], int cantidad) {
    listarVehiculos(lista, cantidad);
    if (cantidad == 0) return;

    int id = leerEntero("\nIngrese el id del vehiculo: ", 0);
    int idx = buscarVehiculoPorId(lista, cantidad, id);
    if (idx == -1) {
        printf("No existe ningun vehiculo con ese id.\n");
        return;
    }

    float kmCiudad    = leerFlotante("Kilometros a recorrer en ciudad: ", 0.0f);
    float kmAutopista = leerFlotante("Kilometros a recorrer en autopista: ", 0.0f);

    float costoCombustible, costoTotal;
    calcularCostoViaje(lista[idx], kmCiudad, kmAutopista, &costoCombustible, &costoTotal);

    printf("\n============ RESULTADO DEL VIAJE: %s ============\n", lista[idx].nombre);
    printf("Km en ciudad:                    %.2f\n", kmCiudad);
    printf("Km en autopista:                 %.2f\n", kmAutopista);
    printf("Costo de combustible del viaje:  %.2f\n", costoCombustible);
    printf("Costo total real del viaje:      %.2f\n", costoTotal);
    printf("(incluye combustible, amortizacion, mantenimiento, neumaticos y seguro)\n");
}


/* ========================================================================
   MENU PRINCIPAL
   ======================================================================== */

void mostrarMenu(void) {
    printf("\n=====================================\n");
    printf("   GESTOR DE COSTOS DE VEHICULOS\n");
    printf("=====================================\n");
    printf("1. Crear vehiculo\n");
    printf("2. Listar vehiculos\n");
    printf("3. Modificar parametros de un vehiculo\n");
    printf("4. Modificar precio de combustible\n");
    printf("5. Borrar vehiculo\n");
    printf("6. Calcular costo por km (ciudad/autopista)\n");
    printf("7. Calcular costo de un viaje\n");
    printf("0. Salir\n");
}

int main(void) {
    Vehiculo lista[MAX_VEHICULOS];
    int cantidad = cargarVehiculos(lista, MAX_VEHICULOS);
    int opcion;

    printf("Se cargaron %d vehiculo(s) desde el archivo '%s'.\n",
           cantidad, ARCHIVO_VEHICULOS);

    do {
        mostrarMenu();
        opcion = leerEntero("Seleccione una opcion: ", 0);

        switch (opcion) {
            case 1: crearVehiculo(lista, &cantidad);            break;
            case 2: listarVehiculos(lista, cantidad);           break;
            case 3: modificarVehiculo(lista, cantidad);         break;
            case 4: modificarPrecioCombustible(lista, cantidad); break;
            case 5: borrarVehiculo(lista, &cantidad);           break;
            case 6: mostrarCostoPorKm(lista, cantidad);         break;
            case 7: menuCalcularViaje(lista, cantidad);         break;
            case 0: printf("\nSaliendo del programa...\n");     break;
            default: printf("Opcion invalida.\n");              break;
        }
    } while (opcion != 0);

    return 0;
}
