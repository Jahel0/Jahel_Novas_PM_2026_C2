#include <stdio.h>
#include <math.h>

/* Par, Impar o nulo.
NUM: variable de este tipo entero. */

void main(void)
{
int NUM;
printf("Ingrese el Numero: ");
scanf("d%", &NUM);
if (NUM == 0)
        printf("\nNulo");
  else
        if  (pow (-1, NUM)> 0)
             printf("\nPar");
        else
             printf("\nImpar");
}
