#include <stdio.h>
#include <string.h>

int longitud(char *);

int main(void)
{
    int i, n, l = -1, p = 0, t;
    char FRA[20][50];
    printf("\nIngrese el numero de filas del arreglo: ");
    scanf("%d", &n);
    for (i = 0; i < n; i++)
    {
        printf("Ingrese la linea %d de texto: ", i + 1);
        fflush(stdin);
        gets(FRA[i]);
    }

    for (i = 0; i < n; i++)
    {
        t = longitud(FRA[i]);
        if (t > l)
        {
            l = t;
            p = i;
        }
    }

    printf("\nLa cadena con mayor longitud es: ");
    puts(FRA[p]);
    printf("\nLongitud: %d\n", l);
    return 0;
}

int longitud(char *cadena)
{
    int cue = 0;
    while (cadena[cue] != '\0')
    {
        cue++;
    }
    return (cue);
}
