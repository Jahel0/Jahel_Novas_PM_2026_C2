#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nombre[50];
    char apellido[50];
    float promedio;
    char materia[50];
} Alumno;

int main()
 {
    const char* nombreArchivo = "Archivo_CSV_Tarea_6_.csv";
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
    fgets(linea, sizeof(linea), archivo);

    while (fgets(linea, sizeof(linea), archivo))
        {
        if (strlen(linea) > 5)
        {
            cantidadElementos++;
        }
    }
    fclose(archivo);
    printf("Se detectaron %d alumnos en el archivo CSV (excluyendo la cabecera).\n", cantidadElementos);
    if (cantidadElementos == 0) {
        printf("El archivo esta vacio o no tiene datos validos.\n");
        return 0;
    }
    Alumno* listaAlumnos = (Alumno*) malloc(cantidadElementos * sizeof(Alumno));

    if (listaAlumnos == NULL)
    {
        printf("Error: No se pudo asignar memoria dinamica.\n");
        return 1;
    }
    archivo = fopen(nombreArchivo, "r");
    fgets(linea, sizeof(linea), archivo);
    int indice = 0;
    while (fgets(linea, sizeof(linea), archivo)) {
        if (strlen(linea) > 5)
        {
            sscanf(linea, "%[^,],%[^,],%f,%[^\n\r]",
                   listaAlumnos[indice].nombre,
                   listaAlumnos[indice].apellido,
                   &listaAlumnos[indice].promedio,
                   listaAlumnos[indice].materia);
            indice++;
        }
    }
    fclose(archivo);
    printf("\n=========================================================\n");
    printf("%-12s %-12s %-10s %s\n", "NOMBRE", "APELLIDO", "PROMEDIO", "MATERIA");
    printf("=========================================================\n");
    for (int i = 0; i < cantidadElementos; i++)
     {
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
