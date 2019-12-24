xpto: download

download:
	gcc -Wall -o download ftp.c main.c -Wall -Wextra

clean:
	-rm download index.html
	-rm -f *.o


# Try the following
# --------------------------------------------------------------
# make clean && make
# ./download ftp://anyuser:anypass@ftp.up.pt/pub/CPAN/index.html
# --------------------------------------------------------------
# or
# --------------------------------------------------------------
# make clean && make
# ./download ftp://ftp.up.pt/pub/CPAN/index.html
# --------------------------------------------------------------