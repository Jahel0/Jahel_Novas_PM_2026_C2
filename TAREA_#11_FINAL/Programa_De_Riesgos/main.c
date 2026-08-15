#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

typedef struct {
    int  id;
    char nombre[100];
    char descripcion[200];
    int  F, S, P, E, A, V;
    long ID_dano;
    int  PR;
    long GR;
    char nivel[25];
    int  calculado;
} Riesgo;

static Riesgo *riesgos     = NULL;
static int     total       = 0;
static int     capacidad   = 0;
static int     siguienteId = 1;
static char    archivoActual[260] = "riesgos.json";

static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static int leer_entero(const char *msg, int min, int max) {
    int valor;
    while (1) {
        printf("%s", msg);
        if (scanf("%d", &valor) == 1) {
            limpiar_buffer();
            if (valor >= min && valor <= max) return valor;
        } else {
            limpiar_buffer();
        }
        printf("  -> Valor invalido. Debe estar entre %d y %d.\n", min, max);
    }
}

static void leer_linea(const char *msg, char *destino, int tam) {
    printf("%s", msg);
    if (fgets(destino, tam, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') destino[len - 1] = '\0';
    }
}

/* Copia segura: SIEMPRE deja el destino terminado en '\0',
   a diferencia de strncpy "a secas" cuando src es igual o mas largo que dest. */
static void copiar_seguro(char *dest, const char *src, size_t tamDest) {
    if (tamDest == 0) return;
    strncpy(dest, src, tamDest - 1);
    dest[tamDest - 1] = '\0';
}

static void asegurar_capacidad(void) {
    if (total >= capacidad) {
        int nuevaCap = (capacidad == 0) ? 4 : capacidad * 2;
        Riesgo *tmp = (Riesgo *) realloc(riesgos, (size_t) nuevaCap * sizeof(Riesgo));
        if (tmp == NULL) {
            fprintf(stderr, "ERROR: no se pudo reservar memoria dinamica.\n");
            exit(EXIT_FAILURE);
        }
        riesgos = tmp;
        capacidad = nuevaCap;
    }
}

static Riesgo *buscar_por_id(int id) {
    for (int i = 0; i < total; i++)
        if (riesgos[i].id == id) return &riesgos[i];
    return NULL;
}

static void clasificar(Riesgo *r) {
    long gr = r->GR;
    if      (gr <=  600) strcpy(r->nivel, "MUY BAJO");
    else if (gr <= 1200)  strcpy(r->nivel, "BAJO");
    else if (gr <= 1800)  strcpy(r->nivel, "MEDIO");
    else if (gr <= 2400)  strcpy(r->nivel, "ALTO");
    else                   strcpy(r->nivel, "MUY ALTO / CRITICO");
}

static void calcular_riesgo(Riesgo *r) {
    r->ID_dano = (long) r->F * r->S * r->P * r->E;
    r->PR      = r->A * r->V;
    r->GR      = r->ID_dano * r->PR;
    clasificar(r);
    r->calculado = 1;
}

static void calcular_todos(void) {
    if (total == 0) { printf("\nNo hay riesgos cargados.\n"); return; }
    for (int i = 0; i < total; i++) calcular_riesgo(&riesgos[i]);
    printf("\nSe calcularon %d riesgo(s) con el metodo Mosler.\n", total);
}

static void crear_riesgo(void) {
    asegurar_capacidad();
    Riesgo *r = &riesgos[total];
    memset(r, 0, sizeof(Riesgo));
    r->id = siguienteId++;

    printf("\n--- CREAR NUEVO RIESGO (id=%d) ---\n", r->id);
    leer_linea("Nombre del riesgo: ", r->nombre, sizeof(r->nombre));
    leer_linea("Descripcion breve: ", r->descripcion, sizeof(r->descripcion));

    printf("\nIngrese los factores Mosler (escala 1 a 5):\n");
    r->F = leer_entero("  F - Funcion (importancia de lo afectado): ", 1, 5);
    r->S = leer_entero("  S - Sustitucion (dificultad de reemplazo): ", 1, 5);
    r->P = leer_entero("  P - Profundidad (repercusion/perturbacion): ", 1, 5);
    r->E = leer_entero("  E - Extension (alcance del dano): ", 1, 5);
    r->A = leer_entero("  A - Agresion (probabilidad de ocurrencia): ", 1, 5);
    r->V = leer_entero("  V - Vulnerabilidad (probabilidad de dano): ", 1, 5);

    total++;
    printf("\nRiesgo #%d creado correctamente.\n", r->id);
}

static void modificar_riesgo(void) {
    if (total == 0) { printf("\nNo hay riesgos registrados.\n"); return; }
    int id = leer_entero("\nID del riesgo a modificar: ", 1, 1000000);
    Riesgo *r = buscar_por_id(id);
    if (!r) { printf("No existe un riesgo con ese ID.\n"); return; }

    printf("\nRiesgo actual: %s (F=%d S=%d P=%d E=%d A=%d V=%d)\n",
           r->nombre, r->F, r->S, r->P, r->E, r->A, r->V);
    printf("Deje el nombre/descripcion vacios para no modificarlos.\n");

    char buffer[200];
    leer_linea("Nuevo nombre: ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) copiar_seguro(r->nombre, buffer, sizeof(r->nombre));

    leer_linea("Nueva descripcion: ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) copiar_seguro(r->descripcion, buffer, sizeof(r->descripcion));

    r->F = leer_entero("F (1-5): ", 1, 5);
    r->S = leer_entero("S (1-5): ", 1, 5);
    r->P = leer_entero("P (1-5): ", 1, 5);
    r->E = leer_entero("E (1-5): ", 1, 5);
    r->A = leer_entero("A (1-5): ", 1, 5);
    r->V = leer_entero("V (1-5): ", 1, 5);
    r->calculado = 0;

    printf("\nRiesgo #%d actualizado. Recuerde recalcular (opcion 3).\n", r->id);
}

static void imprimir_riesgos(void) {
    if (total == 0) { printf("\nNo hay riesgos para mostrar.\n"); return; }

    printf("\n%-4s %-22s %-4s %-4s %-4s %-4s %-4s %-4s %-8s %-19s\n",
           "ID", "Nombre", "F", "S", "P", "E", "A", "V", "GR", "Nivel");
    printf("--------------------------------------------------------------------------------\n");
    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        if (r->calculado)
            printf("%-4d %-22.22s %-4d %-4d %-4d %-4d %-4d %-4d %-8ld %-19s\n",
                   r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V, r->GR, r->nivel);
        else
            printf("%-4d %-22.22s %-4d %-4d %-4d %-4d %-4d %-4d %-8s %-19s\n",
                   r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V, "(sin calc)", "-");
    }
}

static void borrar_riesgo(void) {
    if (total == 0) { printf("\nNo hay riesgos registrados.\n"); return; }
    int id = leer_entero("\nID del riesgo a borrar: ", 1, 1000000);
    int idx = -1;
    for (int i = 0; i < total; i++) if (riesgos[i].id == id) { idx = i; break; }
    if (idx == -1) { printf("No existe un riesgo con ese ID.\n"); return; }

    for (int i = idx; i < total - 1; i++) riesgos[i] = riesgos[i + 1];
    total--;
    printf("Riesgo #%d eliminado.\n", id);
}

static void seleccionar_archivo(void) {
    char nombre[260];
    leer_linea("\nNombre/ruta del archivo a usar (ej: riesgos.json, riesgos.xml o riesgos.csv): ", nombre, sizeof(nombre));
    if (strlen(nombre) == 0) { printf("Nombre invalido.\n"); return; }
    copiar_seguro(archivoActual, nombre, sizeof(archivoActual));
    printf("Archivo activo ahora: %s\n", archivoActual);
}

typedef enum { FMT_JSON, FMT_XML, FMT_CSV } Formato;

static Formato detectar_formato(const char *nombre) {
    const char *ext = strrchr(nombre, '.');
    if (ext && strcasecmp(ext, ".xml") == 0) return FMT_XML;
    if (ext && strcasecmp(ext, ".csv") == 0) return FMT_CSV;
    return FMT_JSON;
}

static const char *nombre_formato(Formato fmt) {
    switch (fmt) {
        case FMT_XML: return "XML";
        case FMT_CSV: return "CSV";
        default:      return "JSON";
    }
}

static void crear_archivo(void) {
    char nombre[260];
    leer_linea("\nNombre del nuevo archivo a crear (ej: nuevo.json, nuevo.xml o nuevo.csv): ", nombre, sizeof(nombre));
    if (strlen(nombre) == 0) { printf("Nombre invalido.\n"); return; }
    FILE *f = fopen(nombre, "w");
    if (!f) { printf("No se pudo crear el archivo.\n"); return; }

    Formato fmt = detectar_formato(nombre);
    if (fmt == FMT_XML)
        fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<riesgos>\n</riesgos>\n");
    else if (fmt == FMT_CSV)
        fprintf(f, "id,nombre,descripcion,F,S,P,E,A,V,GR,nivel\n");
    else
        fprintf(f, "{\n  \"riesgos\": []\n}\n");

    fclose(f);
    copiar_seguro(archivoActual, nombre, sizeof(archivoActual));
    printf("Archivo '%s' creado y establecido como archivo activo.\n", nombre);
}

static void guardar_json(FILE *f) {
    fprintf(f, "{\n  \"riesgos\": [\n");
    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f,
            "    {\n"
            "      \"id\": %d,\n"
            "      \"nombre\": \"%s\",\n"
            "      \"descripcion\": \"%s\",\n"
            "      \"F\": %d, \"S\": %d, \"P\": %d, \"E\": %d, \"A\": %d, \"V\": %d,\n"
            "      \"GR\": %ld,\n"
            "      \"nivel\": \"%s\"\n"
            "    }%s\n",
            r->id, r->nombre, r->descripcion,
            r->F, r->S, r->P, r->E, r->A, r->V, r->GR, r->nivel,
            (i < total - 1) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");
}

static void guardar_xml(FILE *f) {
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<riesgos>\n");
    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f,
            "  <riesgo id=\"%d\">\n"
            "    <nombre>%s</nombre>\n"
            "    <descripcion>%s</descripcion>\n"
            "    <F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>\n"
            "    <GR>%ld</GR>\n"
            "    <nivel>%s</nivel>\n"
            "  </riesgo>\n",
            r->id, r->nombre, r->descripcion,
            r->F, r->S, r->P, r->E, r->A, r->V, r->GR, r->nivel);
    }
    fprintf(f, "</riesgos>\n");
}

/* Escribe un campo CSV entre comillas, duplicando las comillas internas
   (asi soporta nombres/descripciones que contengan comas o comillas). */
static void escribir_campo_csv(FILE *f, const char *campo) {
    fputc('"', f);
    for (const char *p = campo; *p; p++) {
        if (*p == '"') fputc('"', f);
        fputc(*p, f);
    }
    fputc('"', f);
}

static void guardar_csv(FILE *f) {
    fprintf(f, "id,nombre,descripcion,F,S,P,E,A,V,GR,nivel\n");
    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f, "%d,", r->id);
        escribir_campo_csv(f, r->nombre);
        fputc(',', f);
        escribir_campo_csv(f, r->descripcion);
        fprintf(f, ",%d,%d,%d,%d,%d,%d,%ld,", r->F, r->S, r->P, r->E, r->A, r->V, r->GR);
        escribir_campo_csv(f, r->nivel);
        fputc('\n', f);
    }
}

static void guardar_riesgos(void) {
    if (total == 0) { printf("\nNo hay riesgos para guardar.\n"); return; }
    FILE *f = fopen(archivoActual, "w");
    if (!f) { printf("No se pudo abrir '%s' para escritura.\n", archivoActual); return; }

    Formato fmt = detectar_formato(archivoActual);
    if      (fmt == FMT_XML) guardar_xml(f);
    else if (fmt == FMT_CSV) guardar_csv(f);
    else                     guardar_json(f);

    fclose(f);
    printf("Guardado en '%s' (formato %s).\n", archivoActual, nombre_formato(fmt));
}

static void agregar_riesgo_cargado(Riesgo nuevo) {
    asegurar_capacidad();
    riesgos[total] = nuevo;
    total++;
    if (nuevo.id >= siguienteId) siguienteId = nuevo.id + 1;
}

static void extraer_valor_texto(const char *linea, const char *clave, char *destino, int tam) {
    const char *p = strstr(linea, clave);
    if (!p) { destino[0] = '\0'; return; }
    p = strchr(p + strlen(clave), ':');
    if (!p) { destino[0] = '\0'; return; }
    p++;
    while (*p == ' ' || *p == '"') p++;
    int i = 0;
    while (*p && *p != '"' && *p != ',' && i < tam - 1) { destino[i++] = *p++; }
    destino[i] = '\0';
}

static void cargar_json(FILE *f) {
    char linea[1024];
    Riesgo r; memset(&r, 0, sizeof(r));
    int enObjeto = 0;
    int dentroArreglo = 0;

    while (fgets(linea, sizeof(linea), f)) {
        if (!dentroArreglo) {
            if (strstr(linea, "\"riesgos\"")) dentroArreglo = 1;
            continue;
        }
        if (!enObjeto && strstr(linea, "]")) { dentroArreglo = 0; continue; }
        if (strstr(linea, "{")) { memset(&r, 0, sizeof(r)); enObjeto = 1; continue; }
        if (strstr(linea, "}")) {
            if (enObjeto) { r.calculado = 1; agregar_riesgo_cargado(r); }
            enObjeto = 0;
            continue;
        }
        if (!enObjeto) continue;

        char valor[300];
        if (strstr(linea, "\"id\"")) {
            extraer_valor_texto(linea, "\"id\"", valor, sizeof(valor));
            r.id = atoi(valor);
        } else if (strstr(linea, "\"nombre\"")) {
            extraer_valor_texto(linea, "\"nombre\"", valor, sizeof(valor));
            copiar_seguro(r.nombre, valor, sizeof(r.nombre));
        } else if (strstr(linea, "\"descripcion\"")) {
            extraer_valor_texto(linea, "\"descripcion\"", valor, sizeof(valor));
            copiar_seguro(r.descripcion, valor, sizeof(r.descripcion));
        } else if (strstr(linea, "\"GR\"")) {
            extraer_valor_texto(linea, "\"GR\"", valor, sizeof(valor));
            r.GR = atol(valor);
        } else if (strstr(linea, "\"nivel\"")) {
            extraer_valor_texto(linea, "\"nivel\"", valor, sizeof(valor));
            copiar_seguro(r.nivel, valor, sizeof(r.nivel));
        } else if (strstr(linea, "\"F\"")) {
            sscanf(linea, " \"F\": %d, \"S\": %d, \"P\": %d, \"E\": %d, \"A\": %d, \"V\": %d",
                   &r.F, &r.S, &r.P, &r.E, &r.A, &r.V);
        }
    }
}

static void cargar_xml(FILE *f) {
    char linea[1024];
    Riesgo r; memset(&r, 0, sizeof(r));
    int enObjeto = 0;

    while (fgets(linea, sizeof(linea), f)) {
        if (strstr(linea, "<riesgo ")) {
            memset(&r, 0, sizeof(r));
            char *p = strstr(linea, "id=\"");
            if (p) r.id = atoi(p + 4);
            enObjeto = 1;
            continue;
        }
        if (strstr(linea, "</riesgo>")) {
            if (enObjeto) { r.calculado = 1; agregar_riesgo_cargado(r); }
            enObjeto = 0;
            continue;
        }
        if (!enObjeto) continue;

        char *ini, *fin;
        if ((ini = strstr(linea, "<nombre>"))) {
            ini += strlen("<nombre>"); fin = strstr(ini, "</nombre>");
            if (fin) { int n = (int)(fin - ini); if (n >= (int)sizeof(r.nombre)) n = sizeof(r.nombre) - 1; memcpy(r.nombre, ini, n); r.nombre[n] = '\0'; }
        } else if ((ini = strstr(linea, "<descripcion>"))) {
            ini += strlen("<descripcion>"); fin = strstr(ini, "</descripcion>");
            if (fin) { int n = (int)(fin - ini); if (n >= (int)sizeof(r.descripcion)) n = sizeof(r.descripcion) - 1; memcpy(r.descripcion, ini, n); r.descripcion[n] = '\0'; }
        } else if ((ini = strstr(linea, "<F>"))) {
            sscanf(ini, "<F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>",
                   &r.F, &r.S, &r.P, &r.E, &r.A, &r.V);
        } else if ((ini = strstr(linea, "<GR>"))) {
            r.GR = atol(ini + strlen("<GR>"));
        } else if ((ini = strstr(linea, "<nivel>"))) {
            ini += strlen("<nivel>"); fin = strstr(ini, "</nivel>");
            if (fin) { int n = (int)(fin - ini); if (n >= (int)sizeof(r.nivel)) n = sizeof(r.nivel) - 1; memcpy(r.nivel, ini, n); r.nivel[n] = '\0'; }
        }
    }
}

/* Lee un campo CSV desde *p, respetando comillas ("campo con, comas" y "" -> ").
   Avanza *p mas alla del separador. */
static void leer_campo_csv(const char **p, char *destino, int tam) {
    const char *s = *p;
    int i = 0;

    if (*s == '"') {
        s++;
        while (*s) {
            if (*s == '"') {
                if (*(s + 1) == '"') {
                    if (i < tam - 1) destino[i++] = '"';
                    s += 2;
                } else {
                    s++;
                    break;
                }
            } else {
                if (i < tam - 1) destino[i++] = *s;
                s++;
            }
        }
    } else {
        while (*s && *s != ',' && *s != '\n' && *s != '\r') {
            if (i < tam - 1) destino[i++] = *s;
            s++;
        }
    }
    destino[i] = '\0';

    if (*s == ',') s++;
    *p = s;
}

static void cargar_csv(FILE *f) {
    char linea[2048];

    if (!fgets(linea, sizeof(linea), f)) return; /* descarta encabezado */

    while (fgets(linea, sizeof(linea), f)) {
        size_t len = strlen(linea);
        while (len > 0 && (linea[len - 1] == '\n' || linea[len - 1] == '\r')) linea[--len] = '\0';
        if (len == 0) continue;

        const char *p = linea;
        Riesgo r; memset(&r, 0, sizeof(r));
        char campo[300];

        leer_campo_csv(&p, campo, sizeof(campo)); r.id = atoi(campo);
        leer_campo_csv(&p, r.nombre, sizeof(r.nombre));
        leer_campo_csv(&p, r.descripcion, sizeof(r.descripcion));
        leer_campo_csv(&p, campo, sizeof(campo)); r.F = atoi(campo);
        leer_campo_csv(&p, campo, sizeof(campo)); r.S = atoi(campo);
        leer_campo_csv(&p, campo, sizeof(campo)); r.P = atoi(campo);
        leer_campo_csv(&p, campo, sizeof(campo)); r.E = atoi(campo);
        leer_campo_csv(&p, campo, sizeof(campo)); r.A = atoi(campo);
        leer_campo_csv(&p, campo, sizeof(campo)); r.V = atoi(campo);
        leer_campo_csv(&p, campo, sizeof(campo)); r.GR = atol(campo);
        leer_campo_csv(&p, r.nivel, sizeof(r.nivel));

        r.calculado = 1;
        agregar_riesgo_cargado(r);
    }
}

static void cargar_riesgos(void) {
    FILE *f = fopen(archivoActual, "r");
    if (!f) { printf("No se pudo abrir '%s' para lectura.\n", archivoActual); return; }

    total = 0;

    Formato fmt = detectar_formato(archivoActual);
    if      (fmt == FMT_XML) cargar_xml(f);
    else if (fmt == FMT_CSV) cargar_csv(f);
    else                     cargar_json(f);

    fclose(f);
    printf("Se cargaron %d riesgo(s) desde '%s' (formato %s, memoria dinamica, capacidad=%d).\n",
           total, archivoActual, nombre_formato(fmt), capacidad);
}

static void mostrar_menu(void) {
    printf("\n==================== METODO MOSLER - GESTOR DE RIESGOS ====================\n");
    printf(" Archivo activo: %s (%s)\n", archivoActual, nombre_formato(detectar_formato(archivoActual)));
    printf(" 1. Crear riesgo\n");
    printf(" 2. Modificar parametros del riesgo\n");
    printf(" 3. Calcular riesgos (aplicar formula Mosler)\n");
    printf(" 4. Imprimir riesgos\n");
    printf(" 5. Borrar riesgo\n");
    printf(" 6. Seleccionar un archivo especifico (.json, .xml o .csv)\n");
    printf(" 7. Crear un archivo especifico (.json, .xml o .csv)\n");
    printf(" 8. Guardar riesgos en el archivo activo\n");
    printf(" 9. Cargar riesgos desde el archivo activo\n");
    printf(" 0. Salir\n");
    printf("=============================================================================\n");
}

int main(void) {
    int opcion;
    do {
        mostrar_menu();
        opcion = leer_entero("Seleccione una opcion: ", 0, 9);
        switch (opcion) {
            case 1: crear_riesgo(); break;
            case 2: modificar_riesgo(); break;
            case 3: calcular_todos(); break;
            case 4: imprimir_riesgos(); break;
            case 5: borrar_riesgo(); break;
            case 6: seleccionar_archivo(); break;
            case 7: crear_archivo(); break;
            case 8: guardar_riesgos(); break;
            case 9: cargar_riesgos(); break;
            case 0: printf("\nSaliendo... liberando memoria dinamica.\n"); break;
        }
    } while (opcion != 0);

    free(riesgos);
    riesgos = NULL;
    return 0;
}
