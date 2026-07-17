#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char nombre[50];
    char apellido[50];
    float promedio;
    char materia[50];
} Alumno;
void extraerDatoJSON(const char* linea, const char* clave, char* destino)
{
    char patron[60];
    sprintf(patron, "\"%s\":", clave);
    char* posClave = strstr(linea, patron);
    if (posClave)
        {
        posClave += strlen(patron);
        while (*posClave == ' ' || *posClave == '\t' || *posClave == '"')
            {
            posClave++;
        }
        int i = 0;
        while (*posClave != '"' && *posClave != ',' && *posClave != '\n' && *posClave != '\r' && *posClave != '}')
        {
            destino[i++] = *posClave++;
        }
        destino[i] = '\0';
    } else
    {
        strcpy(destino, "");
    }
}

int main()
{
    const char* nombreArchivo = "Archivo_JSON_Tarea_6_.json";
    FILE* archivo;
    char linea[256];
    int cantidadElementos = 0;
    archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL)
        {
        printf("Error: No se pudo abrir el archivo %s\n", nombreArchivo);
        printf("Asegurate de que este en la misma carpeta que el ejecutable.\n");
        return 1;
    }

    while (fgets(linea, sizeof(linea), archivo))
        {
        if (strstr(linea, "\"nombre\":") != NULL)
        {
            cantidadElementos++;
        }
    }
    fclose(archivo);
    printf("Se detectaron %d alumnos en el archivo JSON.\n", cantidadElementos);
    if (cantidadElementos == 0)
    {
        printf("El archivo esta vacio o no tiene el formato correcto.\n");
        return 0;
    }
    Alumno* listaAlumnos = (Alumno*) malloc(cantidadElementos * sizeof(Alumno));
    if (listaAlumnos == NULL) {
        printf("Error: No se pudo asignar memoria dinamica.\n");
        return 1;
    }
    archivo = fopen(nombreArchivo, "r");
    int indice = -1;
    char tempPromedio[20];
    while (fgets(linea, sizeof(linea), archivo))
    {
        if (strstr(linea, "\"nombre\":") != NULL)
        {
            indice++;
            extraerDatoJSON(linea, "nombre", listaAlumnos[indice].nombre);
        }
        else if (strstr(linea, "\"apellido\":") != NULL)
        {
            extraerDatoJSON(linea, "apellido", listaAlumnos[indice].apellido);
        }
        else if (strstr(linea, "\"promedio\":") != NULL)
        {
            extraerDatoJSON(linea, "promedio", tempPromedio);
            listaAlumnos[indice].promedio = atof(tempPromedio);
        }
        else if (strstr(linea, "\"materia\":") != NULL)
        {
            extraerDatoJSON(linea, "materia", listaAlumnos[indice].materia);
        }
    }
    fclose(archivo);
    printf("\n=========================================================\n");
    printf("%-12s %-12s %-10s %s\n", "NOMBRE", "APELLIDO", "PROMEDIO", "MATERIA");
    printf("=========================================================\n");

    for (int i = 0; i < cantidadElementos; i++) {
        printf("%-12s %-12s %-10.1f %s\n",
               listaAlumnos[i].nombre,
               listaAlumnos[i].apellido,
               listaAlumnos[i].promedio,
               listaAlumnos[i].materia);
    }
    printf("=========================================================\n");
    free(listaAlumnos);
    listaAlumnos = NULL;

    return 0;
}
