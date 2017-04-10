OBJS:=$(wildcard *.c)
server:$(OBJS)
	gcc $^ -o ../bin/$@ -I ../include -lcrypt -lmysqlclient
clean:
	rm ../bin/server 

