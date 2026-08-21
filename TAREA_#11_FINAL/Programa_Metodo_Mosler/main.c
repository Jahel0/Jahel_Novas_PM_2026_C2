#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>

#ifdef _WIN32
  #include <windows.h>
  #define strcasecmp _stricmp
#else
  #include <strings.h>
#endif

/* === SECUENCIAS DE COLOR Y ESTILOS ANSI === */
#define RESET       "\x1b[0m"
#define BOLD        "\x1b[1m"
#define DIM         "\x1b[2m"
#define ITALIC      "\x1b[3m"
#define UNDERLINE   "\x1b[4m"

#define RED         "\x1b[31m"
#define GREEN       "\x1b[32m"
#define YELLOW      "\x1b[33m"
#define BLUE        "\x1b[34m"
#define MAGENTA     "\x1b[35m"
#define CYAN        "\x1b[36m"
#define WHITE       "\x1b[37m"
#define BRIGHT_CYAN "\x1b[96m"
#define BRIGHT_WHT  "\x1b[97m"
#define BG_RED      "\x1b[41m"

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

static Riesgo *riesgos       = NULL;
static int     total         = 0;
static int     capacidad     = 0;
static int     siguienteId   = 1;
static char    archivoActual[260] = "riesgos.json";

/* === UTILIDADES Y CONFIGURACIÓN === */
static void configurar_consola(void) {
#ifdef _WIN32
    // Habilitar soporte de secuencias ANSI y UTF-8 en la consola de Windows
    SetConsoleOutputCP(65001);
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    if (hOut != INVALID_HANDLE_VALUE && GetConsoleMode(hOut, &dwMode)) {
        SetConsoleMode(hOut, dwMode | 0x0004); // ENABLE_VIRTUAL_TERMINAL_PROCESSING
    }
#endif
}

static void limpiar_pantalla(void) {
    printf("\033[H\033[J");
    fflush(stdout);
}

static void repetir_utf8(const char *pieza, int n) {
    for (int i = 0; i < n; i++) printf("%s", pieza);
}

static void ok(const char *fmt, ...) {
    va_list ap;
    printf(GREEN BOLD "  [✓] " RESET GREEN);
    va_start(ap, fmt); vprintf(fmt, ap); va_end(ap);
    printf(RESET "\n");
}

static void error_msg(const char *fmt, ...) {
    va_list ap;
    printf(RED BOLD "  [✗] " RESET RED);
    va_start(ap, fmt); vprintf(fmt, ap); va_end(ap);
    printf(RESET "\n");
}

static void info(const char *fmt, ...) {
    va_list ap;
    printf(CYAN BOLD "  [i] " RESET CYAN);
    va_start(ap, fmt); vprintf(fmt, ap); va_end(ap);
    printf(RESET "\n");
}

static void pausar(void) {
    printf("\n" DIM "  Presione ENTER para continuar..." RESET);
    fflush(stdout);
    getchar();
}

static const char *color_por_nivel(const char *nivel) {
    if (strcmp(nivel, "MUY BAJO") == 0) return CYAN;
    if (strcmp(nivel, "BAJO") == 0)     return GREEN;
    if (strcmp(nivel, "MEDIO") == 0)    return YELLOW;
    if (strcmp(nivel, "ALTO") == 0)     return RED;
    if (strcmp(nivel, "MUY ALTO / CRITICO") == 0) return BG_RED BRIGHT_WHT BOLD;
    return WHITE;
}

static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

static int leer_entero(const char *msg, int min, int max) {
    int valor;
    while (1) {
        printf(YELLOW "%s" RESET, msg);
        if (scanf("%d", &valor) == 1) {
            limpiar_buffer();
            if (valor >= min && valor <= max) return valor;
        } else {
            limpiar_buffer();
        }
        error_msg("Valor inválido. Debe estar entre %d y %d.", min, max);
    }
}

static void leer_linea(const char *msg, char *destino, int tam) {
    printf(YELLOW "%s" RESET, msg);
    if (fgets(destino, tam, stdin) != NULL) {
        size_t len = strlen(destino);
        if (len > 0 && destino[len - 1] == '\n') destino[len - 1] = '\0';
    }
}

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
            fprintf(stderr, RED "ERROR: No se pudo reservar memoria dinámica.\n" RESET);
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

/* === ESCAPAR CADENAS DE TEXTO (CORRECCIÓN DE SEGURIDAD EN ARCHIVOS) === */
static void escribir_texto_escapado_json(FILE *f, const char *str) {
    for (const char *p = str; *p; p++) {
        if (*p == '"' || *p == '\\') fputc('\\', f);
        fputc(*p, f);
    }
}

static void escribir_texto_escapado_xml(FILE *f, const char *str) {
    for (const char *p = str; *p; p++) {
        switch (*p) {
            case '<':  fputs("&lt;", f); break;
            case '>':  fputs("&gt;", f); break;
            case '&':  fputs("&amp;", f); break;
            case '"':  fputs("&quot;", f); break;
            default:   fputc(*p, f); break;
        }
    }
}

/* === LÓGICA DEL MÉTODO MOSLER === */
static void clasificar(Riesgo *r) {
    long gr = r->GR;
    if      (gr <=  600) strcpy(r->nivel, "MUY BAJO");
    else if (gr <= 1200) strcpy(r->nivel, "BAJO");
    else if (gr <= 1800) strcpy(r->nivel, "MEDIO");
    else if (gr <= 2400) strcpy(r->nivel, "ALTO");
    else                 strcpy(r->nivel, "MUY ALTO / CRITICO");
}

static void calcular_riesgo(Riesgo *r) {
    r->ID_dano = (long) r->F * r->S * r->P * r->E;
    r->PR      = r->A * r->V;
    r->GR      = r->ID_dano * r->PR;
    clasificar(r);
    r->calculado = 1;
}

static void calcular_todos(void) {
    if (total == 0) { info("No hay riesgos cargados."); return; }
    for (int i = 0; i < total; i++) calcular_riesgo(&riesgos[i]);
    ok("Se calcularon %d riesgo(s) con el Método Mosler.", total);
}

/* === OPERACIONES CRUD === */
static void crear_riesgo(void) {
    asegurar_capacidad();
    Riesgo *r = &riesgos[total];
    memset(r, 0, sizeof(Riesgo));
    r->id = siguienteId++;

    printf("\n" BOLD BRIGHT_CYAN "┌─ Crear Nuevo Riesgo [ID: %d] ──────────────────────────┐" RESET "\n", r->id);
    leer_linea("  Nombre del riesgo : ", r->nombre, sizeof(r->nombre));
    leer_linea("  Descripción breve : ", r->descripcion, sizeof(r->descripcion));

    printf("\n" DIM "  Evaluación Factores Mosler (escala 1 a 5):" RESET "\n");
    r->F = leer_entero("   • F - Función (importancia de lo afectado)   : ", 1, 5);
    r->S = leer_entero("   • S - Sustitución (dificultad de reemplazo)  : ", 1, 5);
    r->P = leer_entero("   • P - Profundidad (repercusion/impacto)      : ", 1, 5);
    r->E = leer_entero("   • E - Extensión (alcance del daño)            : ", 1, 5);
    r->A = leer_entero("   • A - Agresión (probabilidad de ocurrencia)  : ", 1, 5);
    r->V = leer_entero("   • V - Vulnerabilidad (probabilidad daño)     : ", 1, 5);

    total++;
    ok("Riesgo #%d creado correctamente.", r->id);
}

static void modificar_riesgo(void) {
    if (total == 0) { info("No hay riesgos registrados."); return; }
    int id = leer_entero("\nID del riesgo a modificar: ", 1, 1000000);
    Riesgo *r = buscar_por_id(id);
    if (!r) { error_msg("No existe un riesgo con ese ID."); return; }

    printf("\n" DIM "  Actual: %s (F=%d S=%d P=%d E=%d A=%d V=%d)" RESET "\n",
           r->nombre, r->F, r->S, r->P, r->E, r->A, r->V);
    printf(DIM "  (Presione ENTER sin escribir nada para conservar el nombre/descripción)" RESET "\n\n");

    char buffer[200];
    leer_linea("  Nuevo nombre      : ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) copiar_seguro(r->nombre, buffer, sizeof(r->nombre));

    leer_linea("  Nueva descripción : ", buffer, sizeof(buffer));
    if (strlen(buffer) > 0) copiar_seguro(r->descripcion, buffer, sizeof(r->descripcion));

    r->F = leer_entero("  F (1-5): ", 1, 5);
    r->S = leer_entero("  S (1-5): ", 1, 5);
    r->P = leer_entero("  P (1-5): ", 1, 5);
    r->E = leer_entero("  E (1-5): ", 1, 5);
    r->A = leer_entero("  A (1-5): ", 1, 5);
    r->V = leer_entero("  V (1-5): ", 1, 5);
    r->calculado = 0;

    ok("Riesgo #%d actualizado. Recuerde recalcular (Opción 3).", r->id);
}

static void imprimir_riesgos(void) {
    if (total == 0) { info("No hay riesgos para mostrar."); return; }

    printf("\n" BOLD BRIGHT_WHT);
    printf("┌──────┬────────────────────────┬───┬───┬───┬───┬───┬───┬──────────┬────────────────────┐\n");
    printf("│ %-4s │ %-22s │ %-1s │ %-1s │ %-1s │ %-1s │ %-1s │ %-1s │ %-8s │ %-18s │\n",
           "ID", "Nombre", "F", "S", "P", "E", "A", "V", "GR", "Nivel de Riesgo");
    printf("├──────┼────────────────────────┼───┼───┼───┼───┼───┼───┼──────────┼────────────────────┤\n");
    printf(RESET);

    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        const char *color = r->calculado ? color_por_nivel(r->nivel) : DIM;

        printf(BOLD "|" RESET " %-4d " BOLD "|" RESET " %-22.22s " BOLD "|" RESET " %d " BOLD "|" RESET " %d " BOLD "|" RESET " %d " BOLD "|" RESET " %d " BOLD "|" RESET " %d " BOLD "|" RESET " %d " BOLD "|" RESET,
               r->id, r->nombre, r->F, r->S, r->P, r->E, r->A, r->V);

        if (r->calculado) {
            printf(" %-8ld " BOLD "|" RESET "%s %-18s " RESET BOLD "|\n" RESET, r->GR, color, r->nivel);
        } else {
            printf(" %-8s " BOLD "|" RESET DIM " %-18s " RESET BOLD "|\n" RESET, "-", "(Sin calcular)");
        }
    }

    printf(BOLD BRIGHT_WHT);
    printf("└──────┴────────────────────────┴───┴───┴───┴───┴───┴───┴──────────┴────────────────────┘\n");
    printf(RESET);
}

static void borrar_riesgo(void) {
    if (total == 0) { info("No hay riesgos registrados."); return; }
    int id = leer_entero("\nID del riesgo a borrar: ", 1, 1000000);
    int idx = -1;
    for (int i = 0; i < total; i++) if (riesgos[i].id == id) { idx = i; break; }
    if (idx == -1) { error_msg("No existe un riesgo con ese ID."); return; }

    for (int i = idx; i < total - 1; i++) riesgos[i] = riesgos[i + 1];
    total--;
    ok("Riesgo #%d eliminado.", id);
}

/* === PERSISTENCIA Y ARCHIVOS === */
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

static void seleccionar_archivo(void) {
    char nombre[260];
    leer_linea("\nNombre/ruta del archivo (.json, .xml o .csv): ", nombre, sizeof(nombre));
    if (strlen(nombre) == 0) { error_msg("Nombre inválido."); return; }
    copiar_seguro(archivoActual, nombre, sizeof(archivoActual));
    ok("Archivo activo: %s", archivoActual);
}

static void crear_archivo(void) {
    char nombre[260];
    leer_linea("\nNombre del nuevo archivo a crear: ", nombre, sizeof(nombre));
    if (strlen(nombre) == 0) { error_msg("Nombre inválido."); return; }
    FILE *f = fopen(nombre, "w");
    if (!f) { error_msg("No se pudo crear el archivo."); return; }

    Formato fmt = detectar_formato(nombre);
    if (fmt == FMT_XML)
        fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<riesgos>\n</riesgos>\n");
    else if (fmt == FMT_CSV)
        fprintf(f, "id,nombre,descripcion,F,S,P,E,A,V,GR,nivel\n");
    else
        fprintf(f, "{\n  \"riesgos\": []\n}\n");

    fclose(f);
    copiar_seguro(archivoActual, nombre, sizeof(archivoActual));
    ok("Archivo '%s' creado y seleccionado.", nombre);
}

static void guardar_json(FILE *f) {
    fprintf(f, "{\n  \"riesgos\": [\n");
    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f, "    {\n");
        fprintf(f, "      \"id\": %d,\n", r->id);
        fprintf(f, "      \"nombre\": \""); escribir_texto_escapado_json(f, r->nombre); fprintf(f, "\",\n");
        fprintf(f, "      \"descripcion\": \""); escribir_texto_escapado_json(f, r->descripcion); fprintf(f, "\",\n");
        fprintf(f, "      \"F\": %d, \"S\": %d, \"P\": %d, \"E\": %d, \"A\": %d, \"V\": %d,\n",
                r->F, r->S, r->P, r->E, r->A, r->V);
        fprintf(f, "      \"GR\": %ld,\n", r->GR);
        fprintf(f, "      \"nivel\": \"%s\"\n", r->nivel);
        fprintf(f, "    }%s\n", (i < total - 1) ? "," : "");
    }
    fprintf(f, "  ]\n}\n");
}

static void guardar_xml(FILE *f) {
    fprintf(f, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<riesgos>\n");
    for (int i = 0; i < total; i++) {
        Riesgo *r = &riesgos[i];
        fprintf(f, "  <riesgo id=\"%d\">\n", r->id);
        fprintf(f, "    <nombre>"); escribir_texto_escapado_xml(f, r->nombre); fprintf(f, "</nombre>\n");
        fprintf(f, "    <descripcion>"); escribir_texto_escapado_xml(f, r->descripcion); fprintf(f, "</descripcion>\n");
        fprintf(f, "    <F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>\n",
                r->F, r->S, r->P, r->E, r->A, r->V);
        fprintf(f, "    <GR>%ld</GR>\n", r->GR);
        fprintf(f, "    <nivel>%s</nivel>\n", r->nivel);
        fprintf(f, "  </riesgo>\n");
    }
    fprintf(f, "</riesgos>\n");
}

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
        escribir_campo_csv(f, r->nombre); fputc(',', f);
        escribir_campo_csv(f, r->descripcion);
        fprintf(f, ",%d,%d,%d,%d,%d,%d,%ld,", r->F, r->S, r->P, r->E, r->A, r->V, r->GR);
        escribir_campo_csv(f, r->nivel);
        fputc('\n', f);
    }
}

static void guardar_riesgos(void) {
    if (total == 0) { info("No hay riesgos para guardar."); return; }
    FILE *f = fopen(archivoActual, "w");
    if (!f) { error_msg("No se pudo abrir '%s' para escritura.", archivoActual); return; }

    Formato fmt = detectar_formato(archivoActual);
    if      (fmt == FMT_XML) guardar_xml(f);
    else if (fmt == FMT_CSV) guardar_csv(f);
    else                     guardar_json(f);

    fclose(f);
    ok("Guardado en '%s' (%s).", archivoActual, nombre_formato(fmt));
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

    while (fgets(linea, sizeof(linea), f)) {
        if (strstr(linea, "{")) {
            memset(&r, 0, sizeof(r));
            enObjeto = 1;
        }
        if (strstr(linea, "}")) {
            if (enObjeto && r.id > 0) {
                r.calculado = (r.GR > 0);
                agregar_riesgo_cargado(r);
            }
            enObjeto = 0;
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
            sscanf(linea, " %*[\"]F%*[\"]: %d, %*[\"]S%*[\"]: %d, %*[\"]P%*[\"]: %d, %*[\"]E%*[\"]: %d, %*[\"]A%*[\"]: %d, %*[\"]V%*[\"]: %d",
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
            if (enObjeto) { r.calculado = (r.GR > 0); agregar_riesgo_cargado(r); }
            enObjeto = 0;
            continue;
        }
        if (!enObjeto) continue;

        char *ini, *fin;
        if ((ini = strstr(linea, "<nombre>"))) {
            ini += 8; fin = strstr(ini, "</nombre>");
            if (fin) { int n = (int)(fin - ini); if (n >= (int)sizeof(r.nombre)) n = sizeof(r.nombre) - 1; memcpy(r.nombre, ini, n); r.nombre[n] = '\0'; }
        } else if ((ini = strstr(linea, "<descripcion>"))) {
            ini += 13; fin = strstr(ini, "</descripcion>");
            if (fin) { int n = (int)(fin - ini); if (n >= (int)sizeof(r.descripcion)) n = sizeof(r.descripcion) - 1; memcpy(r.descripcion, ini, n); r.descripcion[n] = '\0'; }
        } else if ((ini = strstr(linea, "<F>"))) {
            sscanf(ini, "<F>%d</F><S>%d</S><P>%d</P><E>%d</E><A>%d</A><V>%d</V>",
                   &r.F, &r.S, &r.P, &r.E, &r.A, &r.V);
        } else if ((ini = strstr(linea, "<GR>"))) {
            r.GR = atol(ini + 4);
        } else if ((ini = strstr(linea, "<nivel>"))) {
            ini += 7; fin = strstr(ini, "</nivel>");
            if (fin) { int n = (int)(fin - ini); if (n >= (int)sizeof(r.nivel)) n = sizeof(r.nivel) - 1; memcpy(r.nivel, ini, n); r.nivel[n] = '\0'; }
        }
    }
}

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
    if (!fgets(linea, sizeof(linea), f)) return;

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

        r.calculado = (r.GR > 0);
        agregar_riesgo_cargado(r);
    }
}

static void cargar_riesgos(void) {
    FILE *f = fopen(archivoActual, "r");
    if (!f) { error_msg("No se pudo abrir '%s' para lectura.", archivoActual); return; }

    total = 0;
    siguienteId = 1;

    Formato fmt = detectar_formato(archivoActual);
    if      (fmt == FMT_XML) cargar_xml(f);
    else if (fmt == FMT_CSV) cargar_csv(f);
    else                     cargar_json(f);

    fclose(f);
    ok("Se cargaron %d riesgo(s) desde '%s' (%s).", total, archivoActual, nombre_formato(fmt));
}

/* === DISEÑO DE INTERFAZ Y MENÚ === */
static void imprimir_banner(void) {
    printf(BOLD BRIGHT_CYAN);
    printf("╔═════════════════════════════════════════════════════════════════════════╗\n");
    printf("║                 GESTOR DE RIESGOS - MÉTODO MOSLER                      ║\n");
    printf("╚═════════════════════════════════════════════════════════════════════════╝\n");
    printf(RESET);
}

static void mostrar_menu(void) {
    limpiar_pantalla();
    imprimir_banner();

    Formato fmt = detectar_formato(archivoActual);
    printf(DIM "  Archivo activo: " RESET BOLD BRIGHT_WHT "%s" RESET DIM " [%s]" RESET "\n\n",
           archivoActual, nombre_formato(fmt));

    printf(BOLD BRIGHT_WHT "  ┌── GESTIÓN DE RIESGOS ──────────────────────────────────────────────┐\n" RESET);
    printf("  │ " GREEN BOLD "1" RESET "  Crear nuevo riesgo                                            │\n");
    printf("  │ " GREEN BOLD "2" RESET "  Modificar parámetros de un riesgo                             │\n");
    printf("  │ " GREEN BOLD "3" RESET "  Calcular niveles de riesgo (Fórmula Mosler)                  │\n");
    printf("  │ " GREEN BOLD "4" RESET "  Mostrar tabla de riesgos                                      │\n");
    printf("  │ " GREEN BOLD "5" RESET "  Borrar riesgo                                                 │\n");
    printf(BOLD BRIGHT_WHT "  ├── ARCHIVOS Y PERSISTENCIA ─────────────────────────────────────────┤\n" RESET);
    printf("  │ " YELLOW BOLD "6" RESET "  Seleccionar archivo de trabajo (.json / .xml / .csv)          │\n");
    printf("  │ " YELLOW BOLD "7" RESET "  Crear archivo nuevo                                           │\n");
    printf("  │ " YELLOW BOLD "8" RESET "  Guardar cambios en archivo activo                             │\n");
    printf("  │ " YELLOW BOLD "9" RESET "  Cargar datos desde archivo activo                             │\n");
    printf(BOLD BRIGHT_WHT "  ├── SALIDA ──────────────────────────────────────────────────────────┤\n" RESET);
    printf("  │ " RED BOLD "0" RESET "  Salir del programa                                            │\n");
    printf(BOLD BRIGHT_WHT "  └────────────────────────────────────────────────────────────────────┘\n" RESET);
}

int main(void) {
    configurar_consola();
    int opcion;

    do {
        mostrar_menu();
        opcion = leer_entero("\n  Seleccione una opción [0-9]: ", 0, 9);
        printf("\n");

        switch (opcion) {
            case 1: crear_riesgo();         pausar(); break;
            case 2: modificar_riesgo();     pausar(); break;
            case 3: calcular_todos();       pausar(); break;
            case 4: imprimir_riesgos();     pausar(); break;
            case 5: borrar_riesgo();        pausar(); break;
            case 6: seleccionar_archivo(); pausar(); break;
            case 7: crear_archivo();        pausar(); break;
            case 8: guardar_riesgos();      pausar(); break;
            case 9: cargar_riesgos();       pausar(); break;
            case 0:
                printf(CYAN "  Saliendo... Liberando memoria dinámica.\n" RESET);
                break;
        }
    } while (opcion != 0);

    free(riesgos);
    riesgos = NULL;
    return 0;
}
