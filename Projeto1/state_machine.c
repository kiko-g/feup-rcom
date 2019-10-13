#include "states.h"
#include "data_macros.h"

// void handleState(enum state_machines m, unsigned int *currentState, unsigned char byte)
// {
//     switch (m)
//     {
//         case ESTABLISH:
//                 handleEstablish(currentState, byte);
//             break;
        
//         case DATA_TRANSFER:
            
//             break;

//         case TERMINATION:
            
//             break;
        
//         default:
//             break;
//     }
// }

void handleStateOf_S_and_U_Trama(unsigned char byte, unsigned char C_FIELD)
{
	switch (state)
	{
	case Start:
		if (byte == FLAG)
			state = FLAG_RCV;
		break;
	case FLAG_RCV:
		if (byte == A_ANS)
			state = A_RCV;
		else if (byte == FLAG)
			state = FLAG_RCV;
		else
			state = Start;
		break;
	case A_RCV:
		if (byte == C_FIELD)
			state = C_RCV;
		else if (byte == FLAG)
			state = FLAG_RCV;
		else
			state = Start;
		break;
	case C_RCV:
		if (byte == (A_ANS ^ C_FIELD))
			state = BCC_OK;
		else if (byte == FLAG)
			state = FLAG_RCV;
		else
			state = Start;
		break;
	case BCC_OK:
		if (byte == FLAG)
			state = STOP;
		else
			state = Start;
		break;
	}
}

void handleStateOf_I_Trama(unsigned char byte, unsigned char C_FIELD)
{
	switch (state)
	{
	case Start:
		if (byte == FLAG)
			state = FLAG_RCV;
		break;
	case FLAG_RCV:
		if (byte == A_ANS)
			state = A_RCV;
		else if (byte == FLAG)
			state = FLAG_RCV;
		else
			state = Start;
		break;
	case A_RCV:
		if (byte == C_FIELD)
			state = C_RCV;
		else if (byte == FLAG)
			state = FLAG_RCV;
		else
			state = Start;
		break;
	case C_RCV:
		if (byte == (A_ANS ^ C_FIELD))
			state = BCC_OK;
		else if (byte == FLAG)
			state = FLAG_RCV;
		else
			state = Start;
		break;
	case BCC_OK:
		if (byte == FLAG)
			state = STOP;
		else
			state = BBC_OK;
		break;
	}
}

// void handleEstablish(unsigned int *currentState, unsigned char byte)
// {
//     switch (*currentState)
//     {
//       case Start:
//         if (byte == FLAG)
//           *currentState = FLAG_RCV;
//         break;

//       case FLAG_RCV:
//         if (byte == A_ANS)
//           *currentState = A_RCV;
//         else if (byte == FLAG)
//           *currentState = FLAG_RCV;
//         else
//           *currentState = Start;
//         break;
    
//       case A_RCV:
//         if (byte == C_UA)
//           *currentState = C_RCV;
//         else if (byte == FLAG)
//           *currentState = FLAG_RCV;
//         else
//           *currentState = Start;
//         break;

//       case C_RCV:
//         if (byte == BCC)
//           *currentState = BCC_OK;
//         else if (byte == FLAG)
//           *currentState = FLAG_RCV;
//         else
//           *currentState = Start;
//         break;

//       case BCC_OK:
//         if (byte == FLAG)
//           *currentState = STOP;
//         else
//           *currentState = Start;
//         break;		
//     }
// }