all:
	gcc -g -Wall -o smdr-syslog smdr-syslog.c
test:
	gcc -g -Wall -o test test.c
clean:
	-rm smdr-syslog test a.out core 2>/dev/null
