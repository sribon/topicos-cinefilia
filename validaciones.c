#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "validaciones.h"
#include "fecha.h"
#include "indice.h"
#include "socio.h"
#include "titulo.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    calcula el CUIL con el algoritmo modulo 11 del Anexo A.1:
                prefijo 20 (hombre) o 27 (mujer), DNI a 8 digitos y digito
                verificador. Si el resto es 1 el prefijo pasa a 23 y Z es 9
                (hombre) o 4 (mujer).
Parametros:     dni: numero de documento.
                sexo: 'H' o 'M'.
                dest: buffer de al menos TAM_CUIL bytes donde se deja "XY-
                NNNNNNNN-Z".
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void calcular_cuil_ALU(long dni, char sexo, char *dest)
{
    char dni_aux[9];
    int resto;

    if(sexo == 'H')
        strcat(dest, "20-");
    else
        strcat(dest, "27-");

    sprintf(dni_aux, "%08ld", dni);
    strcat(dest, dni_aux);
    strcat(dest, "-");

    resto = calcular_resto(dest);

    switch(resto)
    {
    case 0:
        strcat(dest, "0");
        break;
    case 1:
        if(sexo == 'H')
            strcat(dest, "9");
        else
            strcat(dest, "4");
        *dest = '2';
        *(dest + 1) = '3';
        break;
    default:
        *(dest + strlen(dest)) = ('0' + 11 - resto);
        *(dest + strlen(dest) + 1) = '\0';
        break;
    }
}

/******************************************************************************
Descripcion:    valida un email segun el Anexo A.3: hasta 30 caracteres sin
                espacios, una unica '@', parte local de letras/digitos/puntos
                que no empieza ni termina con punto, y dominio de
                letras/digitos/guiones/puntos con al menos un punto, que no
                empieza ni termina con punto o guion y cuyo ultimo segmento
                tiene al menos 2 letras.
Parametros:     email: cadena a validar.
Retorno:        1 si es valido, 0 en caso contrario.
Observaciones:  No verifica unicidad; eso lo hace socio_email_existe.
******************************************************************************/
int email_valido_ALU(const char *email)
{
    //SI HAY ESPACIOS DEVUELVO ERROR
    if(strchr(email, ' ') != NULL)
        return ERROR;

    //BUSCO EL ARROBA - SI NO LO ENCUENTRO DEVUELVO ERROR
    char *arroba = strchr(email, '@');

    if(arroba == NULL)
        return ERROR;

    /// ANALISIS PARTE LOCAL
    //CALCULO CARACTERES ENTRE EL INICIO DEL DOMINIO Y EL ARROBA
    int cant = arroba - email;

    //SI NO HAY AL MENOS 1 CARACTER DEVUELVO ERROR
    if(cant < 1)
        return ERROR;

    //SI EL EMAIL COMIENZA CON PUNTO O ANTES DEL ARROBA HAY UN PUNTO DEVUELVO ERROR
    if(*email == '.' || *(arroba - 1) == '.')
        return ERROR;

    //RECORRO LA PARTE LOCAL - SI HAY ALGUN CARACTER QUE NO SEA LETRA O LOS CARACTERES PERMITIDOS DEVUELVO ERROR
    for(const char *p = email; p < arroba; p++)
    {
        if(!ES_LETRA(*p) && !CARACTER_VALIDO(*p))
            return ERROR;
    }

    /// ANALISIS PARTE DOMINIO
    //AVANZO EL PUNTERO ARROBA AL PRIMER CARACTER DEL DOMINIO
    arroba++;

    //SI NO HAY AL MENOS UN PUNTO DEVUELVO ERROR
    if(strchr(arroba, '.') == NULL)
        return ERROR;

    //POSICIONO UN PUNTERO AUXILIAR AL FINAL DEL EMAIL
    char *fin = strchr(email, '\0');

    //SI EL PRIMER O ULTIMO CARACTER DEL DOMINIO ES UN PUNTO DEVUELVO ERROR
    if(*arroba == '.' || *(fin - 1) == '.')
        return ERROR;

    //SI EL PRIMER O ULTIMO CARACTER DEL DOMINIO ES UN GUION DEVUELVO ERROR
    if(*arroba == '-' || *(fin - 1) == '-')
        return ERROR;

    //GENERO UN PUNTERO QUE POSICIONE EN EL ULTIMO PUNTO DE LA CADENA
    char *punto = strrchr(arroba, '.');

    //CALCULO CUANTOS CARACTERES HAY ENTRE EL ULTIMO PUNTO Y EL FINAL DE LA CADENA
    cant = fin - punto;

    //SI NO HAY 3 CARACTERES(INCLUYENDO EL PUNTO) DEVUELVO ERROR
    if(cant < 3)
        return ERROR;

    //RECORRO LA PARTE DE DOMINIO - SI HAY ALGUN CARACTER QUE NO SEA LETRA, CARACTER PERMITIDO O GUION DEVUELVO ERROR
    for(; arroba < punto; arroba++)
    {
        if(!ES_LETRA(*arroba) && !CARACTER_VALIDO(*arroba) && *arroba != '-')
            return ERROR;
    }

    //RECORRO EL SEGMENTO FINAL - SI NO SON LETRAS DEVUELVO ERROR
    punto++;
    for(; punto < fin; punto++)
    {
        if(!ES_LETRA(*punto))
            return ERROR;
    }

    //SI LLEGO HASTA ACA DEVUELVO QUE EL EMAIL ES CORRECTO.
    return OK;
}

int validar_dni(const long dni)
{
    return (dni > 1000000 && dni < 100000000);
}

int validar_sexo(char *sexo)
{
    *sexo = TO_UPPER(*sexo);
    return (*sexo == 'H' || *sexo == 'M');
}

int calcular_resto(const char* cuil)
{
    int resultado = 0;
    int secuencia[10] = {5, 4, 3, 2, 7, 6, 5, 4, 3, 2};
    int *p_sec = secuencia;

    for(int i = 0; i < 11; i++, cuil++)
    {
        if(*cuil != '-')
        {
            resultado += (*p_sec * (*cuil - '0'));
            p_sec++;
        }
    }

    return (resultado % 11);
}

int validar_nya(char* nombre)
{
    int cant = cantidad_palabras(nombre);
    //Si apellido y nombre no contiene mas de dos palabras, significa que falta nombre o apellido y devuelvo error.
    if(cant < 2)
        return ERROR;
    return normalizar_socio(nombre, cant);
}

int cantidad_palabras(const char *cadena)
{
    int cant = 0;
    bool palabra = false;

    while(*cadena != '\0')
    {
        if(ES_LETRA(*cadena))
        {
            if(!palabra)
            {
                palabra = true;
                cant++;
            }
        }
        else
        {
            palabra = false;
        }

        cadena++;
    }

    return cant;
}

int normalizar_socio(char* nombre, const int cant_palabras)
{
    char cad[TAM_APYN] = {0};
    char palabra[15] = {0};
    char *p_nombre = nombre;
    char *p_palabra;

    char* p_coma = strchr(nombre, ',');

    if(p_coma == NULL)
    {
        bool coma = false;
        while(*p_nombre != '\0' && !ES_LETRA(*p_nombre))
            p_nombre++;

        while(*p_nombre != '\0')
        {
            p_palabra = palabra;

            while(ES_LETRA(*p_nombre))
            {
                *p_palabra = *p_nombre;
                p_palabra++;
                p_nombre++;
            }

            *p_palabra = '\0';
            capitalizar(palabra);
            strcat(cad, palabra);

            while(*p_nombre != '\0' && !ES_LETRA(*p_nombre))
                p_nombre++;

            if(*p_nombre != '\0')
            {
                if(coma == false)
                {
                    coma = true;
                    strcat(cad, ", ");
                }
                else
                {
                    strcat(cad, " ");
                }
            }
        }
    }
    //Si todas las palabras estan despues de la coma devuelvo error.
    else if(cant_palabras == cantidad_palabras(p_coma + 1))
    {
        return ERROR;
    }
    //Si todas las palabras estan antes de la coma devuelvo error.
    else if(0 == cantidad_palabras(p_coma + 1))
    {
        return ERROR;
    }
    else
    {
        while(*p_nombre != '\0' && !ES_LETRA(*p_nombre))
            p_nombre++;

        while(*p_nombre != '\0')
        {
            p_palabra = palabra;

            while(ES_LETRA(*p_nombre))
            {
                *p_palabra = *p_nombre;
                p_palabra++;
                p_nombre++;
            }

            *p_palabra = '\0';
            capitalizar(palabra);
            strcat(cad, palabra);

            while(*p_nombre != '\0' && !ES_LETRA(*p_nombre))
            {
                if(p_nombre == p_coma)
                    strcat(cad, ",");
                p_nombre++;
            }

            if(*p_nombre != '\0')
                strcat(cad, " ");
        }
    }

    strcpy(nombre, cad);
    return OK;
}

void capitalizar(char* palabra)
{
    //Si la primer letra es minuscula la convierto
    if(*palabra >= 'a' && *palabra <= 'z')
        *palabra -= 32;
    palabra++;

    //Las letras que sean mayusculas hasta el final del archivo las convierto a minusculas
    while(*palabra != '\0')
    {
        if(*palabra >= 'A' && *palabra <= 'Z')
            *palabra += 32;
        palabra++;
    }
}

int validar_fnac(const t_fecha *f_nac, const t_fecha *f_proc, int *edad)
{
    if(validar_fecha(f_nac) != OK)
        return ERROR;

    *edad = calcular_edad(f_nac, f_proc);

    if(*edad < 10)
        return ERROR;

    return OK;
}

int calcular_edad(const t_fecha *nacimiento, const t_fecha *proceso)
{
    int edad = proceso->anio - nacimiento->anio;

    if(proceso->mes < nacimiento->mes)
        edad--;
    else if((proceso->mes == nacimiento->mes) && (proceso->dia < nacimiento->dia))
        edad--;

    return edad;
}

int validar_fafi(const t_fecha *f_afi, const t_fecha *f_nac, const t_fecha *f_proc)
{
    if(validar_fecha(f_afi) != OK)
        return ERROR;

    if(comparar_fechas(f_nac, f_afi) != ANTERIOR)
        return ERROR;
    else if(comparar_fechas(f_afi, f_proc) == POSTERIOR)
        return ERROR;

    return OK;
}

int comparar_fechas(const t_fecha *fecha1, const t_fecha *fecha2)
{
    if(fecha1->anio < fecha2->anio)
        return ANTERIOR;

    if(fecha1->anio > fecha2->anio)
        return POSTERIOR;

    if(fecha1->mes < fecha2->mes)
        return ANTERIOR;

    if(fecha1->mes > fecha2->mes)
        return POSTERIOR;

    if(fecha1->dia < fecha2->dia)
        return ANTERIOR;

    if(fecha1->dia > fecha2->dia)
        return POSTERIOR;

    return IGUAL;
}

int validar_categoria(char *categoria, const int *edad)
{
    char *p = categoria;
    for(; *p != '\0'; p++)
        *p = TO_UPPER(*p);

    if((strcmp(categoria, "MENOR") != 0) && (strcmp(categoria, "ADULTO") != 0))
        return ERROR;

    if((strcmp(categoria, "MENOR") == 0) && (*edad >= 18))
        return ERROR;
    else if((strcmp(categoria, "ADULTO") == 0) && (*edad < 18))
        return ERROR;

    return OK;
}

int validar_fucp(const t_fecha *f_ucp, const t_fecha *f_afi, const t_fecha *f_proc)
{
    if(validar_fecha(f_ucp) != OK)
        return ERROR;

    if(comparar_fechas(f_ucp, f_afi) == ANTERIOR)
        return ERROR;
    else if(comparar_fechas(f_ucp, f_proc) == POSTERIOR)
        return ERROR;

    return OK;
}

int validar_plan(char *plan)
{
    char *p = plan;
    for(; *p != '\0'; p++)
        *p = TO_UPPER(*p);

    char planes[4][TAM_PLAN] = {"BASIC", "PREMIUM", "VIP", "FAMILY"};

    for(int i=0; i < 4; i++)
    {
        if(strcmp(plan, planes[i]) == 0)
            return OK;
    }

    return ERROR;
}

int validar_email(char *email, const int *edad)
{
    if((*edad < 18) && (strcmp(email, "\0") == 0))
        return ERROR;
    else if((*edad >= 18) && (strlen(email) != 0))
        return ERROR;
    else if((*edad < 18) && email_valido_ALU(email) != OK)
        return ERROR;

    char *p = email;
    for(;*p != '\0'; p++)
        *p = TO_LOWER(*p);

    return OK;
}

int email_unico(const char *email, const t_indice *indice, FILE *pf)
{
    t_reg_indice *reg_ini = indice->vindice;
    t_reg_indice *reg_fin = indice->vindice + (indice->cantidad_elementos_actual * sizeof(t_reg_indice));
    t_socio socio;

    for(;reg_ini < reg_fin; reg_ini++)
    {
        fseek(pf, sizeof(t_socio) * reg_ini->nro_reg, SEEK_SET);
        fread(&socio, sizeof(t_socio), 1, pf);

        if(strlen(socio.email_tutor) > 0 && strcmp(socio.email_tutor, email) == 0)
            return ERROR;
    }

    return OK;
}

int validar_id(const int id)
{
    return id > 0;
}

int normalizar_titulo(char *titulo)
{
    char cad[TAM_TITULO] = {0};
    char palabra[15] = {0};
    char *p_titulo = titulo;
    char *p_palabra;

    if(strlen(titulo) == 0)
        return ERROR;

    while(*p_titulo != '\0' && !ES_LETRA(*p_titulo))
        p_titulo++;

    while(*p_titulo != '\0')
    {
        p_palabra = palabra;

        while(ES_LETRA(*p_titulo))
        {
            *p_palabra = *p_titulo;
            p_palabra++;
            p_titulo++;
        }

        *p_palabra = '\0';
        capitalizar(palabra);
        strcat(cad, palabra);

        while(*p_titulo != '\0' && !ES_LETRA(*p_titulo))
            p_titulo++;

        if(*p_titulo != '\0')
            strcat(cad, " ");

    }

    strcpy(titulo, cad);
    return OK;
}

int validar_genero(const char *genero)
{
    char v_generos[5][TAM_GENERO] = {"Accion", "Drama", "Comedia", "Terror", "Ciencia Ficcion"};

    for(int i=0; i < 5; i++)
    {
        if(strcmp(genero, v_generos[i]) == 0)
            return OK;
    }

    return ERROR;
}

int validar_stock(const int stock)
{
    return stock >= 0;
}
