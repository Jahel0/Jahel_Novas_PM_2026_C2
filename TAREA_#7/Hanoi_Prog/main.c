#include <stdio.h>

int num_paso = 1;

void ejecutarHanoi(int discos, char desde, char hacia, char pivote, FILE *output)
{
    if (discos == 1)
    {
        fprintf(output, "Paso %d: Trasladar disco 1 desde la torre %c hasta la %c\n", num_paso, desde, hacia);
        num_paso++;
        return;
    }

    ejecutarHanoi(discos - 1, desde, pivote, hacia, output);

    fprintf(output, "Paso %d: Trasladar disco %d desde la torre %c hasta la %c\n", num_paso, discos, desde, hacia);
    num_paso++;

    ejecutarHanoi(discos - 1, pivote, hacia, desde, output);
}

int main()
{
    int total_discos = 3;
    const char *nombre_archivo = "resultado_hanoi.txt";

    FILE *archivo_log = fopen(nombre_archivo, "w");

    if (archivo_log == NULL)
    {
        printf("Error fatal: No se pudo generar el archivo de salida.\n");
        return 1;
    }

    printf("Iniciando la simulacion de las Torres de Hanoi para %d discos...\n", total_discos);

    ejecutarHanoi(total_discos, 'A', 'C', 'B', archivo_log);

    fclose(archivo_log);

    printf("Proceso finalizado. El registro detallado se guardo en '%s'.\n", nombre_archivo);
    return 0;
}
