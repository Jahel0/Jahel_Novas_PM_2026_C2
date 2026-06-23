#include <stdio.h>
#include <string.h>

int cuentap(char *);

int main(void)
{
    int i;
    char fra[50];
    printf("\nIngrese la linea de texto: ");
    gets(fra);
    strcat(fra, " ");
    i = cuentap(fra);
    printf("\nLa linea de texto tiene %d palabras", i);
    return 0;
}

int cuentap(char *cad)
{
    char *cad0;
    int i = 0;
    cad0 = strstr(cad, " ");
    while (cad0 != NULL && strcmp(cad0, " ") != 0)
    {
        i++;
        cad0 = strstr(cad0 + 1, " ");
    }

    return i;
}
