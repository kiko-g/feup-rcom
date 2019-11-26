#include "states.h"
#include "data_macros.h"
#include <stdio.h>

// USES A_ANS
void handleStateOfEstablishment(int *state, unsigned char byte, unsigned char C_FIELD)
{
	switch (*state)
	{
	case Start:
		if (byte == FLAG)
			*state = FLAG_RCV;
		break;
	case FLAG_RCV:
		if (byte == A_ANS)
			*state = A_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case A_RCV:
		if (byte == C_FIELD)
			*state = C_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case C_RCV:
		if (byte == (A_ANS ^ C_FIELD))
			*state = BCC_OK;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case BCC_OK:
		if (byte == FLAG)
			*state = STOP;
		else
			*state = Start;
		break;
	}
}

void handleStateOfTransmissionSender(int *state, unsigned char byte, unsigned char C_FIELD1, unsigned char C_FIELD2)
{
	switch (*state)
	{
	case Start:
		if (byte == FLAG)
			*state = FLAG_RCV;
		break;
	case FLAG_RCV:
		if (byte == A_ANS)
			*state = A_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case A_RCV:
		if (byte == C_FIELD1)
			*state = C_RR;
		else if (byte == C_FIELD2)
			*state = C_REJ;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case C_RR:
		if (byte == (A_ANS ^ C_FIELD1))
			*state = BCC_OK;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case C_REJ:
		if (byte == (A_ANS ^ C_FIELD2))
			*state = BCC_OK;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case BCC_OK:
		if (byte == FLAG)
			*state = STOP;
		else
			*state = Start;
		break;
	}
}

void handleStateOfTransmissionReceiver(int *state, unsigned char byte, unsigned char C_FIELD)
{
	switch (*state)
	{
	case Start:
		if (byte == FLAG)
			*state = FLAG_RCV;
		break;
	case FLAG_RCV:
		if (byte == A_ANS)
			*state = A_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case A_RCV:
		if (byte == C_DISC)
			*state = C_RCV;
		else if (byte == C_FIELD)
			*state = C_I_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case C_RCV:
		if (byte == (A_ANS ^ C_DISC))
			*state = BCC_OK;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case C_I_RCV:
		if (byte == (A_ANS ^ C_FIELD))
			*state = DATA;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case DATA:
		if (byte == FLAG)
			*state = STOP;
		else
			*state = DATA;
		break;
	case BCC_OK:
		if (byte == FLAG)
			*state = STOP;
		else
			*state = Start;
		break;
	}
}

// USES A_SND
void handleStateOfTermination(int *state, unsigned char byte, unsigned char C_FIELD)
{
	switch (*state)
	{
	case Start:
		if (byte == FLAG)
			*state = FLAG_RCV;
		break;
	case FLAG_RCV:
		if (byte == A_SND)
			*state = A_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case A_RCV:
		if (byte == C_FIELD)
			*state = C_RCV;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case C_RCV:
		if (byte == (A_SND ^ C_FIELD))
			*state = BCC_OK;
		else if (byte == FLAG)
			*state = FLAG_RCV;
		else
			*state = Start;
		break;
	case BCC_OK:
		if (byte == FLAG)
			*state = STOP;
		else
			*state = Start;
		break;
	}
}
