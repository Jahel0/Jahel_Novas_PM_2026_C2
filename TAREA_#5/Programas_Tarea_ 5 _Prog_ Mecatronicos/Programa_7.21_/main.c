#include <stdio.h>
#include <string.h>

void intercambia(char FRA[][30], int);

int main(void)
{
    int i, n;
    char FRA[20][30];
    printf("\nIngrese el numero de filas del arreglo: ");
    scanf("%d", &n);
    for (i = 0; i < n; i++)
    {
        printf("Ingrese la linea de texto numero %d: ", i + 1);
        fflush(stdin);
        gets(FRA[i]);
    }
    printf("\n\n");
    intercambia(FRA, n);
    for (i = 0; i < n; i++)
    {
        printf("Impresion de la linea de texto %d: ", i + 1);
        puts(FRA[i]);
    }

    return 0;
}

void intercambia(char FRA[][30], int n)
{
    int i, j;
    char cad[30];
    j = n - 1;
    for (i = 0; i < (n / 2); i++)
    {
        strcpy(cad, FRA[i]);
        strcpy(FRA[i], FRA[j]);
        strcpy(FRA[j], cad);
        j--;
    }
}
