all:	client serve

client: 
	gcc -w -o DUMBclient DUMBclient.c

serve:
	gcc -w -o DUMBserver DUMBserver.c

clean:
	rm -f DUMBclient DUMBserver
