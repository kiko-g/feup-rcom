#pragma once

enum state_machines {ESTABLISH, DATA_TRANSFER, TERMINATION};

//Establish States
#define Start 0
#define FLAG_RCV 1
#define A_RCV 2
#define C_RCV 3
#define BCC_OK 4
#define STOP 5