#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <conio.h>
#include "fecha.h"
#include "util.h"
#include "indice.h"      /* OK / ERROR */

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    solicita la fecha de proceso por teclado hasta obtener una
                valida. Si el usuario solo presiona Enter toma la fecha actual
                del sistema.
Parametros:     fecha_proceso: fecha destino.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void pedir_fecha_proceso_ALU(t_fecha *fecha_proceso)
{
    char tecla;
    puts("Desea ingresar fecha de proceso?\nPresione 'Y' si desea ingresar fecha. Caso contrario presione 'N'");
    tecla = getch();

    while((tecla != 'N' && tecla != 'n') && (tecla != 'Y' && tecla != 'y'))
        tecla = getch();

    procesar_fecha_proceso(fecha_proceso, tecla);
}

void procesar_fecha_proceso(t_fecha* fecha, const char tecla)
{
    if(tecla == 'Y' || tecla == 'y')
    {
        cargar_fecha(fecha);

        while(validar_fecha(fecha) == ERROR_FECHA)
        {
            puts("\nLa fecha ingresada es invalida. Volver a ingresar.\n");
            cargar_fecha(fecha);
        }
    }
    else
    {
        time_t tiempo = time(NULL);
        struct tm *tmPtr = localtime(&tiempo);

        fecha->dia = tmPtr->tm_mday;
        fecha->mes = tmPtr->tm_mon + 1;
        fecha->anio = tmPtr->tm_year + 1900;
    }
}

void cargar_fecha(t_fecha* fecha)
{
    char cadena[50];
    char extra;     //"Extra" lo utilizamos para comprobar si se ingresa un caracter que no es un numero
    char *salto;    //Puntero utilizado para convertir el '\n' del fgets en '\0'

    printf("Ingrese la fecha (dd/mm/aaaa): ");
    fgets(cadena, sizeof(cadena), stdin);
    salto = strchr(cadena, '\n');

    if(salto)
        *salto = '\0';

    //Si se ingresan 3 numeros el sscanf deberia ser igual a 3.
    while(sscanf(cadena, "%d/%d/%d%c", &fecha->dia, &fecha->mes, &fecha->anio, &extra) != 3)
    {
        puts("\nLa fecha ingresada contiene caracteres invalidos.");
        printf("Ingrese la fecha (dd/mm/aaaa): ");
        fgets(cadena, sizeof(cadena), stdin);

        salto = strchr(cadena, '\n');

        if(salto)
            *salto = '\0';
    }
}

int validar_fecha(const t_fecha* fecha)
{

    int vecDias[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if(fecha->anio < 1582 || fecha->anio > 2099)
        return ERROR_FECHA;

    if(fecha->mes < 1 || fecha->mes > 12)
        return ERROR_FECHA;

    if(fecha->mes == 2)
    {
        if(fecha->dia < 1 || fecha->dia > BISIESTO(fecha->anio))
            return ERROR_FECHA;
    }
    else
    {
        if(fecha->dia < 1 || fecha->dia > vecDias[fecha->mes - 1])
            return ERROR_FECHA;
    }

    return FECHA_OK;
}
