all:	serve client

client:
	gcc -w -o DUMBclient DUMBclient.c

serve:
	gcc -w -o DUMBserve DUMBserver.c -lpthread

clean:
	rm -f DUMBclient DUMBserve
