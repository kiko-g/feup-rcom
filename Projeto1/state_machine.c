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