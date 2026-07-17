#include <stdio.h>

int main()
{
    int opcion = 0;
    int cantidadJugadas = 0;
    int numeroGanador = 1;
    int huboGanador = 0;

    int numerosJugados[100];
    float dineroApostado[100];


    printf("=== BIENVENIDO AL CASINO LOTEKA TETOKA - DE: JNC ===\n");

    while(opcion != 3)
    {
        numeroGanador = numeroGanador + 7;
    if (numeroGanador > 40)
    {
        numeroGanador = 1;
    }

        printf("\n--- MENU PRINCIPAL ---\n");

        printf("\n--- Una Vez Que Entras Se Te Hara Dificil Salir ---\n");

        printf("1. Elegir la Cantidad de Jugadas que Vas a Realizar (Actual: %d)\n", cantidadJugadas);
        printf("2. Jugar\n");
        printf("3. Salir\n");
        printf("Seleciona una opcion: ");
        scanf("%d", &opcion);

        switch(opcion)
        {
        case 1:
            printf("\n¿Cuantas jugadas  deseas  registrar? ");
            scanf("%d", &cantidadJugadas);

            if(cantidadJugadas <= 0 || cantidadJugadas > 100)
            {
                printf("Cantidad invalida. Intente de nuevo.\n");
                cantidadJugadas = 0;
            }
            else
        {

        printf("Has configurado  %d jugadas correctaente.\n", cantidadJugadas);
    }
    break;

        case  2:
            if(cantidadJugadas == 0)
            {
                printf("\n[ERROR] Primero debe elegir la cantidad de jugadas en la opcion 1.\n");
                break ;
            }
            printf("\n--- REGISTRO DE JUGADAS ---\n");
            for(int i = 0; i < cantidadJugadas; i++)
            {
                printf("\n->  JUGADA  #%d:\n", i + 1);

                int numValido = 0;
                while(numValido  ==  0)
                {
                    printf("  1. Ingrese el numero elegido  (Entre el 1 y 40): ");
                    scanf("%d", &numerosJugados[i]);

                    if(numerosJugados[i] >=  1 &&  numerosJugados[i] <= 40)
                    {
                        numValido  = 1;
                    }
                    else
                    {
                        printf("   [ERROR] El  numero debe estar entre 1 y 40.\n");
                    }
                }
                printf("  2. Ingrese  la cantidad de Dinero a apostar: $");
                scanf("%f", &dineroApostado[i]);
            }
            printf("\n========================================");
            printf("\n La loteria se a ha cerrado.");
            printf("\n El numero que estaba oculto era: [%d] ", numeroGanador);
            printf("\n========================================\n");

            huboGanador = 0;

            for(int i = 0; i < cantidadJugadas;  i++)
            {
                if(numerosJugados[i] ==  numeroGanador)
                {
                    float premio = dineroApostado[i] * 1000;
                    printf("\nFELICIDADES TE  SACASTE LA LOTO LA JUGADA #%d fue la ganadora.\n", i + 1);
                    printf("Te has ganado: $%.2f  (Tus $%0.2f apostados x 1000\n)", premio, dineroApostado[i]);
                    huboGanador =  1;
                }
            }
            if (huboGanador == 0)
            {
                printf("\nTe Guallaste Ninguna de tus jugadas pego con la ganadora.\n");
                printf("Suerte para la proxima.\n");
            }

          cantidadJugadas = 0;
          break ;

          case 3:
          printf("\nGracias por gastar tu Dinero en LOTEKA TETOKA. Vuelve Pronto bajo tu propio riesgo\n");
          break;

          default:
            printf("Opcion invalida. Intente de nuevo.\n");
            break;
}
}
return  0;
}
