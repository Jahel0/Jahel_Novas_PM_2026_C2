#include <stdio.h>

const int F = 8, C = 2, P = 5;

void Lectura(int [][C][P], int, int, int);
void Funcion1(int [][C][P], int, int, int);
void Funcion2(int [][C][P], int, int, int);
void Funcion3(int [][C][P], int, int, int);

int main(void)
{
    int UNI[F][C][P];
    Lectura(UNI, F, C, P);
    Funcion1(UNI, F, C, P);
    Funcion2(UNI, F, C, P);
    Funcion3(UNI, 6, C, P);
    return 0;
}

void Lectura(int A[][C][P], int FI, int CO, int PR)
{
    int K, I, J;
    for (K = 0; K < PR; K++)
    {
        for (I = 0; I < FI; I++)
        {
            for (J = 0; J < CO; J++)
            {
                printf("Ano: %d\tCarrera: %d\tSemestre: %d: ", K + 1, I + 1, J + 1);
                scanf("%d", &A[I][J][K]);
            }
        }
    }
}

void Funcion1(int A[][C][P], int FI, int CO, int PR)
{
    int I, J, K, SUM;
    for (I = 0; I < FI; I++)
    {
        SUM = 0;
        for (K = 0; K < PR; K++)
        {
            for (J = 0; J < CO; J++)
            {
                SUM += A[I][J][K];
            }
        }
        printf("\nTotal de alumnos en la carrera %d: %d", I + 1, SUM);
    }
    printf("\n");
}

void Funcion2(int A[][C][P], int FI, int CO, int PR)
{
    int I, K, MAY = -1, CAR = -1, SUM;
    for (I = 0; I < FI; I++)
    {
        SUM = 0;
        for (K = 0; K < PR; K++)
        {
            SUM += A[I][0][K];
        }
        if (SUM > MAY)
        {
            MAY = SUM;
            CAR = I;
        }
    }
    printf("\nCarrera con mayor numero de alumnos en el primer semestre: %d (Total: %d)\n", CAR + 1, MAY);
}

void Funcion3(int A[][C][P], int FI, int CO, int PR)
{
    int K, J, MAY = -1, AO = -1, SUM;
    for (K = 0; K < PR; K++)
    {
        SUM = 0;
        for (J = 0; J < CO; J++)
        {
            SUM += A[FI][J][K];
        }
        if (SUM > MAY)
        {
            MAY = SUM;
            AO = K;
        }
    }
    printf("\nAno en que la carrera %d tuvo mayor ingreso: %d (Total: %d)\n", FI + 1, AO + 1, MAY);
}
