CC = gcc
CFLAGS = -Wall
LDLIBS = -lm
TARGET = client_recv client_send_recv client_recv_udp client_send_recv_udp serv_send serv_send2 file_loop

all: $(TARGET)

client_recv: client_recv.o net.o
	$(CC) -o $@ $^
client_send_recv: client_send_recv.o net.o
	$(CC) -o $@ $^
client_recv_udp: client_recv_udp.o net.o
	$(CC) -o $@ $^
client_send_recv_udp: client_send_recv_udp.o net.o
	$(CC) -o $@ $^
serv_send: serv_send.o net.o
	$(CC) -o $@ $^
serv_send2: serv_send2.o net.o
	$(CC) -o $@ $^
file_loop: file_loop.o
	$(CC) -o $@ $^

.PHONY: clean tmpclean
tmpclean:
	rm -f *~
clean: tmpclean
	rm $(TARGET) *.o
