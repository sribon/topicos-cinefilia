#include <stdlib.h>
#include <string.h>
#include "socio.h"
#include "cadena.h"
#include "util.h"
#include "auditoria.h"
#include "config.h"
#include "menu.h"

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

/******************************************************************************
Descripcion:    convierte una linea CSV de 10 campos en t_socio. Los campos
                con formato invalido quedan con valores que haran fallar la
                validacion correspondiente (dni = 0, fecha 0/0/0, sexo '?',
                etc.). El estado se fija en 'A'.
Parametros:     linea: linea CSV.
                reg: puntero a t_socio destino.
Retorno:        OK si la cantidad de campos es correcta, ERROR en caso
                contrario.
Observaciones:  Se pasa por puntero como parsear a leer_archivo_texto.
******************************************************************************/
int texto_a_socio_ALU(const char *linea, void *reg)
{
    char cadena[TAM_LINEA];
    strcpy(cadena, linea);
    char extra; //Variable utilizada para verificar que no haya caracter indeseado en el ingreso.
    char *ps = cadena;
    t_socio *socio = reg;

    ///DNI
    char *aux = strchr(cadena, ';');
    *aux = '\0';
    aux++;
    //Si el dni es alfanumerico devuelvo error.
    if(sscanf(ps, "%ld %c", &socio->dni, &extra) != 1)
    {
        socio->dni = 0;
        return ERROR;
    }
    ps = aux;

    ///CUIL
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si el cuil es '\0' o si tiene una longitud mayor a 14 caracteres devuelvo error.
    if((sscanf(ps, "%s", socio->cuil) != 1) || (strlen(ps) >= TAM_CUIL))
    {
        socio->sexo = '?';
        strcpy(socio->cuil, "00-00000000-0");
        return ERROR;
    }
    ps = aux;

    ///APELLIDOS Y NOMBRES
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si el nombre contiene mas de 61 caracteres o es vacio devuelvo error.
    if(strlen(ps) >= TAM_APYN || strlen(ps) == 0)
    {
        strcpy(socio->apellidos_nombres, "\0");
        return ERROR;
    }
    strcpy(socio->apellidos_nombres, ps);
    ps = aux;

    ///FECHA NACIMIENTO
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si la fecha contiene un caracter que no es un numero devuelvo error.
    if(sscanf(ps, "%d/%d/%d%c", &socio->fecha_nacimiento.dia, &socio->fecha_nacimiento.mes, &socio->fecha_nacimiento.anio, &extra) != 3)
    {
        socio->fecha_nacimiento.dia = 0;
        socio->fecha_nacimiento.mes = 0;
        socio->fecha_nacimiento.anio = 0;
        return ERROR;
    }
    ps = aux;

    ///SEXO
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si el sexo contiene mas de un caracter devuelvo error.
    if(sscanf(ps, "%c %c", &socio->sexo, &extra) != 1)
    {
        socio->sexo = '?';
        return ERROR;
    }
    ps = aux;

    ///FECHA AFILIACION
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si la fecha contiene un caracter que no es un numero devuelvo error.
    if(sscanf(ps, "%d/%d/%d%c", &socio->fecha_afiliacion.dia, &socio->fecha_afiliacion.mes, &socio->fecha_afiliacion.anio, &extra) != 3)
    {
        socio->fecha_afiliacion.dia = 0;
        socio->fecha_afiliacion.mes = 0;
        socio->fecha_afiliacion.anio = 0;
        return ERROR;
    }
    ps = aux;

    ///CATEGORIA
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si la categoria contiene mas caracteres de lo correspondiente o es vacia devuelvo error.
    if(strlen(ps) >= TAM_CATEGORIA || strlen(ps) == 0)
    {
        strcpy(socio->categoria, "\0");
        return ERROR;
    }

    sscanf(ps, "%s", socio->categoria);
    ps = aux;

    ///FECHA ULT CUOTA
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si la fecha contiene un caracter que no es un numero devuelvo error.
    if(sscanf(ps, "%d/%d/%d%c", &socio->fecha_ultima_cuota.dia, &socio->fecha_ultima_cuota.mes, &socio->fecha_ultima_cuota.anio, &extra) != 3)
    {
        socio->fecha_ultima_cuota.dia = 0;
        socio->fecha_ultima_cuota.mes = 0;
        socio->fecha_ultima_cuota.anio = 0;
        return ERROR;
    }
    ps = aux;

    ///PLAN
    aux = strchr(aux, ';');
    *aux = '\0';
    aux++;
    //Si el plan contiene mas caracteres de lo correspondiente o es vacia devuelvo error.
    if(strlen(ps) >= TAM_PLAN || strlen(ps) == 0)
    {
        strcpy(socio->plan, "\0");
        return ERROR;
    }
    sscanf(ps, "%s", socio->plan);
    ps = aux;

    ///EMAIL TUTOR
    aux = strchr(aux, '\n');
    *aux = '\0';
    //Si el email contiene mas caracteres de lo correspondiente devuelvo error.
    if(strlen(ps) >= TAM_EMAIL)
    {
        strcpy(socio->email_tutor, "emailinvalido");
        return ERROR;
    }
    strcpy(socio->email_tutor, ps);

    socio->estado = 'A';

    return OK;
}

/******************************************************************************
Descripcion:    aplica las validaciones del punto 6.1 en orden (DNI, Sexo,
                CUIL, Apellidos y Nombres, Fecha de nacimiento, Fecha de
                afiliacion, Categoria, Fecha de ultima cuota, Plan, Email
                tutor). Normaliza apellidos y nombres. Se detiene en el primer
                error.
Parametros:     reg: puntero a t_socio a validar (se modifica al normalizar).
                ctx: contexto con indice, archivo binario, fecha de proceso e
                id_modificando.
                tipo_error: salida; nombre de la validacion que fallo.
Retorno:        OK si el socio es valido, ERROR en caso contrario.
Observaciones:  Si ctx->id_modificando == dni no se considera duplicado y se
                excluye ese DNI al verificar unicidad del email. Cumple con el
                puntero validar de leer_archivo_texto.
******************************************************************************/
int valida_socio_ALU(void *reg, const t_contexto *ctx, char *tipo_error)
{
    t_socio *socio = reg;
    t_reg_indice reg_temporal;

    //DNI
    if(!validar_dni(socio->dni))
    {
        strcpy(tipo_error, "DNI");
        return ERROR;
    }
    reg_temporal.id = socio->dni;
    if(indice_buscar_ALU(ctx->indice, &reg_temporal, sizeof(t_reg_indice), compara_reg_indice_ALU) != NO_EXISTE)
    {
        strcpy(tipo_error, "DNI");
        return ERROR;
    }

    //SEXO
    if(!validar_sexo(&socio->sexo))
    {
        strcpy(tipo_error, "SEXO");
        return ERROR;
    }

    //CUIL
    char cuil_aux[TAM_CUIL] = {0};
    calcular_cuil_ALU(socio->dni, socio->sexo, cuil_aux);
    if(strcmp(socio->cuil, cuil_aux) != 0)
    {
        strcpy(tipo_error, "CUIL");
        return ERROR;
    }

    //APELLIDO Y NOMBRE
    if(validar_nya(socio->apellidos_nombres) != OK)
    {
        strcpy(tipo_error, "APELLIDOS_NOMBRES");
        return ERROR;
    }

    //FECHA NACIMIENTO
    int edad;
    if(validar_fnac(&socio->fecha_nacimiento, ctx->fecha_proceso, &edad) != OK)
    {
        strcpy(tipo_error, "FECHA_NACIMIENTO");
        return ERROR;
    }

    //FECHA AFILIACION
    if(validar_fafi(&socio->fecha_afiliacion, &socio->fecha_nacimiento, ctx->fecha_proceso) != OK)
    {
        strcpy(tipo_error, "FECHA_AFILIACION");
        return ERROR;
    }

    //CATEGORIA
    if(validar_categoria(socio->categoria, &edad)!= OK)
    {
        strcpy(tipo_error, "CATEGORIA");
        return ERROR;
    }

    //FECHA ULTIMA CUOTA PAGA
    if(validar_fucp(&socio->fecha_ultima_cuota, &socio->fecha_afiliacion, ctx->fecha_proceso) != OK)
    {
        strcpy(tipo_error, "FECHA_ULTIMA_CUOTA");
        return ERROR;
    }

    //PLAN
    if(!validar_plan(socio->plan))
    {
        strcpy(tipo_error, "PLAN");
        return ERROR;
    }

    //EMAIL TUTOR
    if(validar_email(socio->email_tutor, &edad) != OK)
    {
        strcpy(tipo_error, "EMAIL_TUTOR");
        return ERROR;
    }
    if(email_unico(socio->email_tutor, ctx->indice, ctx->arch_bin) != OK)
    {
        strcpy(tipo_error, "EMAIL_TUTOR");
        return ERROR;
    }

    return OK;
}

/******************************************************************************
Descripcion:    lee el registro nro_reg del archivo binario de socios.
Parametros:     arch: archivo abierto en modo binario.
                nro_reg: numero de registro.
                s: socio destino.
Retorno:        OK si se pudo leer, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_leer_ALU(FILE *arch, unsigned nro_reg, t_socio *s)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socio_leer(arch, nro_reg, s);
}

/******************************************************************************
Descripcion:    escribe el socio en la posicion nro_reg del archivo binario y
                hace flush.
Parametros:     arch: archivo abierto en modo de escritura binaria.
                nro_reg: numero de registro.
                s: socio a grabar.
Retorno:        OK si se pudo grabar, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_escribir_ALU(FILE *arch, unsigned nro_reg, const t_socio *s)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socio_escribir(arch, nro_reg, s);
}

/******************************************************************************
Descripcion:    aplica la regla del Anexo A.4: el socio esta al dia si la
                ultima cuota es del mes de la fecha de proceso, o del mes
                anterior cuando el dia de la fecha de proceso es <= 10.
Parametros:     ultima_cuota: fecha de ultima cuota paga.
                fecha_proceso: fecha de proceso.
Retorno:        1 si esta al dia, 0 en caso contrario.
Observaciones:  -
******************************************************************************/
int cuota_al_dia_ALU(const t_fecha *ultima_cuota, const t_fecha *fecha_proceso)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return cuota_al_dia(ultima_cuota, fecha_proceso);
}

/******************************************************************************
Descripcion:    opcion Alta: carga un socio por teclado, lo valida con
                valida_socio, lo agrega al final de socios.dat con estado 'A'
                y lo inserta en el indice. Si es rechazado lo audita.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
                fecha_proceso: fecha de proceso.
Retorno:        OK si el socio fue agregado, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_alta_manual_ALU(t_indice *indice, const char *path, const t_fecha *fecha_proceso)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socio_alta_manual(indice, path, fecha_proceso);
}

/******************************************************************************
Descripcion:    opcion Modificacion: pide el DNI, lo busca en el indice (un
                socio dado de baja no se puede modificar), muestra los datos
                actuales, pide los nuevos (Enter conserva), valida y
                sobrescribe el registro. El DNI y el estado no se modifican.
                Si es rechazado lo audita.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
                fecha_proceso: fecha de proceso.
Retorno:        OK si se modifico, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socio_modificacion_manual_ALU(t_indice *indice, const char *path, const t_fecha *fecha_proceso)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socio_modificacion_manual(indice, path, fecha_proceso);
}

/******************************************************************************
Descripcion:    baja logica de socio: busca el DNI en el indice, pone estado
                'B' en el registro del archivo y lo elimina del indice.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
Retorno:        OK si se dio de baja, ERROR si no existe o ya estaba dado de
                baja.
Observaciones:  -
******************************************************************************/
int socio_baja_ALU(t_indice *indice, const char *path)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socio_baja(indice, path);
}

/******************************************************************************
Descripcion:    recorre el indice (ordenado por DNI y solo con activos) y
                lista los socios leyendo cada registro del archivo.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
Retorno:        OK si se pudo abrir el archivo, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socios_listar_ALU(const t_indice *indice, const char *path)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socios_listar(indice, path);
}

/******************************************************************************
Descripcion:    recorre el indice y exporta a un archivo binario los socios
                con plan PREMIUM o VIP y cuota al dia (el estado 'A' lo
                garantiza el indice). Sobrescribe el archivo de exportacion
                anterior.
Parametros:     indice: indice de socios.
                path: ruta de socios.dat.
                path_export: ruta de beta_testers.dat.
                fecha_proceso: fecha de proceso.
Retorno:        OK si se pudo exportar, ERROR en caso contrario.
Observaciones:  -
******************************************************************************/
int socios_exportar_ALU(const t_indice *indice, const char *path, const char *path_export,
                    const t_fecha *fecha_proceso)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    return socios_exportar(indice, path, path_export, fecha_proceso);
}

/******************************************************************************
Descripcion:    opcion 2 del menu: submenu para elegir entre alta y
                modificacion manual de socio.
Parametros:     ind_socios: indice de socios.
                fecha_proceso: fecha de proceso.
Retorno:        No posee.
Observaciones:  -
******************************************************************************/
void abm_socio_ALU(t_indice *indice, const t_fecha *fecha_proceso)
{
    /* TODO: reemplazar esta llamada a la funcion de la catedra por su propia
       implementacion. En la entrega no puede quedar ninguna llamada a una
       funcion de la biblioteca que tenga version _ALU. */
    abm_socio(indice, fecha_proceso);
}

int es_activo_socio(const void *registro)
{
    const t_socio *socio = registro;

    return socio->estado == 'A';
}

long obtener_clave_socio(const void *registro)
{
    const t_socio *socio = registro;

    return socio->dni;
}

void rechazar_socio(const void *reg, const char *linea, const char *tipo_error, const t_fecha *fecha_proc)
{
    FILE *pf = fopen(ARCH_ERRORES, "a+");
    (void)reg;
    char dni[TAM_LINEA];
    strcpy(dni, linea);
    char *aux;
    aux = strchr(dni, ';');
    *aux = '\0';

    fprintf(pf, "%s;%s;%d/%d/%d\n", dni, tipo_error, fecha_proc->dia, fecha_proc->mes, fecha_proc->anio);

    fclose(pf);
}
