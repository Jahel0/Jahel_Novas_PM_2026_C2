#include <stdio.h>

void trueque(int *x, int *y);
int suma(int x);

int main(void)
{
    int a = 5, b  = 10;
    trueque(&a, &b);
    suma(a);

    return 0;
}
void trueque(int *x, int *y)
{
    int tem;
    tem = *x;
    *x = *y;
    *y = tem;
}
int  suma(int x)
{
    return (x + x);
}
