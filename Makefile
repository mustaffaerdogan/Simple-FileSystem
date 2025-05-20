all:
	gcc main.c fs.c -o simplefs

clean:
	rm -f simplefs
