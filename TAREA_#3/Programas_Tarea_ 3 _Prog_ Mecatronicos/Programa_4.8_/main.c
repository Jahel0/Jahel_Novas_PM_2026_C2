#include <stdio.h>

int a, b, c, d;

void function1(int *, int *);
int function2 (int, int *);

void main(void)
{
    int a;
    a = 1;
    b = 2;
    c = 3;
    d = 4;
    printf("\n%d %d %d %d", a, b, c, d);
    function1(&b, &c);
    printf("\n%d %d %d %d", a, b, c, d);
    a = function2(c, &d);
    printf("\n%d %d %d %d", a, b, c, d);
}
void function1(int *b, int *c)
{
    int d;
    a = 5;
    d = 3;
    (*b)++;
    (*c) += 2;
    printf("\n%d %d %d %d", a, *b, *c, d);
}
int function2(int c, int *d)
{
    int b;
    a++;
    b = 7;
    c += 3;
    (*d) += 2;
    printf("\n%d %d %d %d", a, b, c, *d);
    return (c);
}
