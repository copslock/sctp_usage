all: enb mme

enb:
	g++ -o enb enb.c -lsctp -lpthread

mme:
	g++ -o mme mme.c mme_function.c -lsctp -lcassandra

clean:
	rm -f mme enb