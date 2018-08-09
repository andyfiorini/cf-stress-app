all: build push

build:
	gcc -g stress-app.c -o stress-app

push:
	cf push stress-app -c './stress-app' -b binary_buildpack

debug:
	gdb ./stress-app 

cpu:
	(./putparam.sh cpu | telnet 192.168.11.11 80;echo )
cpu+mem:
	(./putparam.sh mem | telnet 192.168.11.11 80;echo )
disk:
	(./putparam.sh disk | telnet 192.168.11.11 80;echo )

test:
	curl -v -k crashy.local.pcfdev.io
