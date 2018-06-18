CC = gcc
CFLAGS = -Wall
LDLIBS = -lm
TARGET = client_recv client_send_recv client_recv_udp client_send_recv_udp serv_send serv_send_loop serv_send2

all: $(TARGET)

client_recv: client_recv.o
	$(CC) -o $@ $^
client_send_recv: client_send_recv.o
	$(CC) -o $@ $^
client_recv_udp: client_recv_udp.o
	$(CC) -o $@ $^
client_send_recv_udp: client_send_recv_udp.o
	$(CC) -o $@ $^
serv_send: serv_send.o
	$(CC) -o $@ $^
serv_send_loop: serv_send_loop.o
	$(CC) -o $@ $^
serv_send2: serv_send2.o
	$(CC) -o $@ $^

.PHONY: clean tmpclean
tmpclean:
	rm -f *~
clean: tmpclean
	rm $(TARGET) *.o
