//Todas as macros para data.h

#pragma once

enum openAS {TRANSMITTER, RECEIVER};

// Tramas
#define FLAG 0x7e
#define ESCAPE 0x7d

#define A_ANS 0x03
#define A_SND 0x01

// I tramas
#define C_Ns0 0x00
#define C_Ns1 0x40

// S and U tramas
#define C_SET 0x03
#define C_DISC 0x0b
#define C_UA 0x07
#define C_RR_Nr0 0x03
#define C_RR_Nr1 0x83
#define C_REJ_Nr0 0x01
#define C_REJ_Nr1 0x81

//Byte stuffing
#define subFLAG 0x7d5e
#define subESCAPE 0x7d5d

//Byte deStuffing
#define xorFLAG 0x5e
#define xorESCAPE 0x5d

//Max length of a string
#define MAX_LEN 256