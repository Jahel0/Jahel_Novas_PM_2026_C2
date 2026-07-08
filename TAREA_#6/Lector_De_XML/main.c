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

void extraerDato(const char* linea, const char* etiqueta, char* destino)
{
    char inicio[60], fin[60];
    sprintf(inicio, "<%s>", etiqueta);
    sprintf(fin, "</%s>", etiqueta);
    char* posInicio = strstr(linea, inicio);
    char* posFin = strstr(linea, fin);
    if (posInicio && posFin)
        {
        posInicio += strlen(inicio);
        int longitud = posFin - posInicio;
        strncpy(destino, posInicio, longitud);
        destino[longitud] = '\0';
    } else
    {
        strcpy(destino, "");
    }
}
int main()
 {
    const char* nombreArchivo = "Archivo_XML_Tarea_6_.xml";
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
        if (strstr(linea, "<alumno>") != NULL)
        {
            cantidadElementos++;
        }
    }
    fclose(archivo);
    printf("Se detectaron %d alumnos en el archivo XML.\n", cantidadElementos);
    if (cantidadElementos == 0)
    {
        printf("El archivo esta vacio o no tiene el formato correcto.\n");
        return 0;
    }
    Alumno* listaAlumnos = (Alumno*) malloc(cantidadElementos * sizeof(Alumno));
    if (listaAlumnos == NULL)
     {
        printf("Error: No se pudo asignar memoria dinamica.\n");
        return 1;
    }
    archivo = fopen(nombreArchivo, "r");
    int indice = 0;
    char tempPromedio[20];
    while (fgets(linea, sizeof(linea), archivo))
    {
        if (strstr(linea, "<alumno>") != NULL)
         {
            while (fgets(linea, sizeof(linea), archivo) && strstr(linea, "</alumno>") == NULL)
            {

                if (strstr(linea, "<nombre>") != NULL)
                 {
                    extraerDato(linea, "nombre", listaAlumnos[indice].nombre);
                }
                else if (strstr(linea, "<apellido>") != NULL)
                {
                    extraerDato(linea, "apellido", listaAlumnos[indice].apellido);
                }
                else if (strstr(linea, "<promedio>") != NULL)
                 {
                    extraerDato(linea, "promedio", tempPromedio);
                    listaAlumnos[indice].promedio = atof(tempPromedio); // atof convierte texto a float
                }
                else if (strstr(linea, "<materia>") != NULL)
                {
                    extraerDato(linea, "materia", listaAlumnos[indice].materia);
                }
            }
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
