//Interface Protocolo ligacao de dados-Aplicacao
//Funcoes implementadas 

#pragma once

#include "data_Macros.h"

int llopen(int porta, enum openAS modoAbertura);
// argumentos
// – porta: COM1, COM2, ...
// – flag: TRANSMITTER / RECEIVER
// retorno
// – identificador da ligação de dados
// – valor negativo em caso de erro

int llwrite(int fd, char * buffer, int length);
// argumentos
// – fd: identificador da ligação de dados
// – buffer: array de caracteres a transmitir
// – length: comprimento do array de caracteres
// retorno
// – número de caracteres escritos
// – valor negativo em caso de erro

int llread(int fd, char * buffer);
// argumentos
// – fd: identificador da ligação de dados
// – buffer: array de caracteres recebidos
// retorno
// – comprimento do array
// (número de caracteres lidos)
// – valor negativo em caso de erro

int llclose(int fd);
// argumentos
// – fd: identificador da ligação de dados
// retorno
// – valor positivo em caso de sucesso
// – valor negativo em caso de erro