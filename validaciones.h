#ifndef VALIDACIONES_H_INCLUDED
#define VALIDACIONES_H_INCLUDED

/* Reglas de negocio puras: no dependen del indice ni de los archivos */

/* ---- CUIL (Anexo A.1) ---- */
#define TAM_CUIL 14      /* "XY-NNNNNNNN-Z" + '\0' */
#define PREFIJO_HOMBRE 20
#define PREFIJO_MUJER  27
#define PREFIJO_RESTO1 23

void calcular_cuil(long dni, char sexo, char *dest);

/* ---- Email (Anexo A.3) ---- */
#define TAM_EMAIL     31     /* 30 caracteres + '\0' */
#define EMAIL_MAX_LEN 30

int email_valido(const char *);

/*============================================================================*/
/*            FUNCIONES A IMPLEMENTAR POR EL ALUMNO (version _ALU)             */
/*============================================================================*/

#include "fecha.h"
#include "indice.h"

#define TO_UPPER(c) ((c >= 'a' && c <= 'z') ? c - 32 : c)
#define TO_LOWER(c) ((c >= 'A' && c <= 'Z') ? c + 32 : c)
#define ES_LETRA(letra) (((letra >= 'a' && letra <= 'z') || (letra >= 'A' && letra <= 'Z')) ? 1 : 0)
#define CARACTER_VALIDO(c) (((c >= '0' && c <= '9') || (c == '.')) ? 1 : 0)

#define ANTERIOR -1
#define IGUAL 0
#define POSTERIOR 1

void calcular_cuil_ALU(long dni, char sexo, char *dest);
int email_valido_ALU(const char *email);

int validar_dni(const long dni);
int validar_sexo(char *sexo);
int calcular_resto(const char* cuil);
int validar_nya(char* nombre);
int cantidad_palabras(const char *cadena);
int normalizar_socio(char* nombre, const int cant_palabras);
void capitalizar(char* palabra);
int validar_fnac(const t_fecha *f_nac, const t_fecha *f_proc, int *edad);
int calcular_edad(const t_fecha *nacimiento, const t_fecha *proceso);
int comparar_fechas(const t_fecha *fecha1, const t_fecha *fecha2);
int validar_fafi(const t_fecha *f_afi, const t_fecha *f_nac, const t_fecha *f_proc);
int validar_categoria(char *categoria, const int *edad);
int validar_fucp(const t_fecha *f_ucp, const t_fecha *f_afi, const t_fecha *f_proc);
int validar_plan(char *plan);
int validar_email(char *email, const int *edad);
int email_unico(const char *email, const t_indice *indice, FILE *pf);

int validar_id(const int id);
int normalizar_titulo(char *titulo);
int validar_genero(const char *genero);
int validar_stock(const int stock);

#endif /* VALIDACIONES_H_INCLUDED */
