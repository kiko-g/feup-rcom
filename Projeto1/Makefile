xpto: sender receiver

sender:
	gcc -Wall -o sender sender.c data.c state_machine.c

receiver:
	gcc -Wall -o receiver receiver.c data.c state_machine.c

clean:
	-rm -f *.o
	-rm -f user
	-rm -f server