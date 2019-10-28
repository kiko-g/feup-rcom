#pragma once

enum state_machines {ESTABLISH, DATA_TRANSFER, TERMINATION};

//Establish States
#define Start 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define STOP 5
#define C_I_RCV 6
#define DATA 7
#define C_RR 8
#define C_REJ 9


void handleStateOfEstablishment(int *state, unsigned char byte, unsigned char C_FIELD);

void handleStateOfTransmissionSender(int *state, unsigned char byte, unsigned char C_FIELD1, unsigned char C_FIELD2);

void handleStateOfTransmissionReceiver(int *state, unsigned char byte, unsigned char C_FIELD);

void handleStateOfTermination(int *state, unsigned char byte, unsigned char C_FIELD);