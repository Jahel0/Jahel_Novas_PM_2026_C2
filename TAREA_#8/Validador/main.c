#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>

void obtener_extension(const char *nombre, char *ext_salida)
{
    int len = (int)strlen(nombre);
    int punto_idx = len - 1;

    while (punto_idx > 0 && nombre[punto_idx] != '.')
    {
        punto_idx--;
    }

    if (punto_idx == 0)
    {
        strcpy(ext_salida, "ninguna");
    }
    else
    {
        int j = 0;
        for (int i = punto_idx; i < len; i++)
        {
            char c = nombre[i];
            if (c >= 'A' && c <= 'Z') c += 32;
            ext_salida[j++] = c;
        }
        ext_salida[j] = '\0';
    }
}

void verificar_contenido_archivo(const char *nombre_archivo)
{
    FILE *archivo = fopen(nombre_archivo, "rb");
    if (!archivo)
    {
        printf("Error: No se pudo abrir el archivo para lectura.\n\n");
        return;
    }

    char extension[20];
    obtener_extension(nombre_archivo, extension);
    printf("-> Extension detectada por nombre: '%s'\n", extension);

    unsigned char bytes_magicos[4] = {0};
    fread(bytes_magicos, 1, 4, archivo);
    fclose(archivo);

    printf("-> Primeros bytes del contenido (Hex): %02X %02X %02X %02X\n", bytes_magicos[0], bytes_magicos[1], bytes_magicos[2], bytes_magicos[3]);

    if (strcmp(extension, ".png") == 0)
    {
        if (bytes_magicos[0] == 0x89 && bytes_magicos[1] == 0x50 && bytes_magicos[2] == 0x4E && bytes_magicos[3] == 0x47)
        {
            printf("[VALIDO]: El archivo es un PNG autentico. Su contenido es valido.\n\n");
        }
        else
        {
            printf("[ALERTA - FRAUDE]: La extension dice .png, pero el contenido interno NO es un PNG real.\n\n");
        }
    }
    else if (strcmp(extension, ".jpg") == 0 || strcmp(extension, ".jpeg") == 0)
    {
        if (bytes_magicos[0] == 0xFF && bytes_magicos[1] == 0xD8)
        {
            printf("[VALIDO]: El archivo es un JPG autentico. Su contenido es valido.\n\n");
        }
        else
        {
            printf("[ALERTA - FRAUDE]: La extension dice .jpg, pero el contenido interno NO es un JPG real.\n\n");
        }
    }
    else if (strcmp(extension, ".bmp") == 0)
    {
        if (bytes_magicos[0] == 0x42 && bytes_magicos[1] == 0x4D)
        {
            printf("[VALIDO]: El archivo es un BMP autentico. Su contenido es valido.\n\n");
        }
        else
        {
            printf("[ALERTA - FRAUDE]: La extension dice .bmp, pero el contenido interno NO es un BMP real.\n\n");
        }
    }
    else
    {
        printf("[INFO]: Archivo con extension no soportada para validacion de imagen (.png/.jpg/.bmp).\n\n");
    }
}

int main()
{
    char lista_archivos[100][260];

    while (1)
    {
        int total_archivos = 0;
        DIR *dir = opendir(".");

        if (!dir)
        {
            printf("Error: No se pudo abrir la carpeta.\n");
            return 1;
        }

        printf("==================================================\n");
        printf("     SISTEMA DE IDENTIFICACION Y VALIDACION       \n");
        printf("==================================================\n");

        struct dirent *entrada_dir;

        while ((entrada_dir = readdir(dir)) != NULL)
        {
            if (strcmp(entrada_dir->d_name, ".") != 0 && strcmp(entrada_dir->d_name, "..") != 0)
            {
                strcpy(lista_archivos[total_archivos], entrada_dir->d_name);
                total_archivos++;
                if (total_archivos >= 100)
                    break;
            }
        }
        closedir(dir);

        if (total_archivos == 0)
        {
            printf("La carpeta esta vacia.\n\n");
            break;
        }

        for (int i = 0; i < total_archivos; i++)
        {
            printf("%d. %s\n", i + 1, lista_archivos[i]);
        }
        printf("0. Salir\n");

        int opcion;
        printf("\nSeleccione el numero del archivo a verificar: ");
        if (scanf("%d", &opcion) != 1)
        {
            printf("Entrada no valida.\n\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            continue;
        }

        if (opcion == 0)
        {
            printf("Saliendo del validador...\n");
            break;
        }
        else if (opcion > 0 && opcion <= total_archivos)
        {
            printf("\nAnalizando estructura de: '%s'\n", lista_archivos[opcion - 1]);
            verificar_contenido_archivo(lista_archivos[opcion - 1]);
        }
        else
        {
            printf("Opcion fuera de rango.\n\n");
        }
    }

    return 0;
}
