all: filemanager

filemanager: main.c string.c ansi.c screen.c dir.c
	gcc -o filemanager main.c string.c ansi.c screen.c dir.c -static -fno-stack-protector -no-pie

clean:
	rm -f filemanager
