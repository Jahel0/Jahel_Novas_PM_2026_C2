#include <stdio.h>

void f1(void);
int K = 5;

void main(void)
{
    int I;
    for (I = 1; I <= 3; I++)
        f1();
}
void f1(void)
{

int K_local = 2;
K_local += K_local;
printf("\n\nEl valor de la variable local es: %d", K);
K = K + K_local;
printf("\nElvalor de la varible global es: %d", K);
}
