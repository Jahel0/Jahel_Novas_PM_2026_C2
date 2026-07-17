#include <stdio.h>

void Mayor(float, float, float);

int main(void)
{
    int I;
    float GOL, PAC, CAR, AGOL = 0, APAC = 0, ACAR = 0;

    for (I = 1; I <= 12; I++)
    {
        printf("\n--- Mes %d ---", I);

        printf("\nIngrese lluvias Region Golfo: ");

        scanf("%f", &GOL);

        AGOL += GOL;

        printf("Ingrese lluvias Region Pacifico: ");
        scanf("%f", &PAC);
        APAC += PAC;

        printf("Ingrese lluvias Region Caribe: ");
        scanf("%f", &CAR);
        ACAR += CAR;
    }

    printf("\n\n=== REPORTE ANUAL ===");

    printf("\nPromedio de lluvias Region Golfo: %6.2f", (AGOL / 12));

    printf("\nPromedio de lluvias Region Pacifico: %6.2f", (APAC / 12));

    printf("\nPromedio de lluvias Region Caribe: %6.2f \n", (ACAR / 12));

    Mayor(AGOL, APAC, ACAR);

    return 0;
}

void Mayor(float R1, float R2, float R3)
{
    if (R1 > R2)
    {
        if (R1 > R3)
            printf("\nRegion con mayor promedio: Region Golfo. Promedio: %6.2f\n", R1 / 12);
        else
            printf("\nRegion con mayor promedio: Region Caribe. Promedio: %6.2f\n", R3 / 12);
    }
    else
    {
        if (R2 > R3)
            printf("\nRegion con mayor promedio: Region Pacifico. Promedio: %6.2f\n", R2 / 12);
        else
            printf("\nRegion con mayor promedio: Region Caribe. Promedio: %6.2f\n", R3 / 12);
    }
}

