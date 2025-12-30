all: filemanager

filemanager: main.c syscall.c string.c #dir.c screen.c keybind.c
	# gcc -o filemanager main.c syscall.c string.c dir.c screen.c keybind.c -nostdlib -static -fno-stack-protector -no-pie
	gcc -o filemanager main.c syscall.c string.c ansi.c screen.c -nostdlib -static -fno-stack-protector -no-pie

clean:
	rm -f filemanager
