


d_fat: $(NAME).d_fat

.SUFFIXES: .d_fat

OUTPUT=d_tabreadmix3~.d_fat
OBJS=d_tabreadmix3~.o

PD_SRC_PATH=/Applications/Art/Pd-0.41-4.app/Contents/Resources/src

CC=gcc

.c.d_fat:
	$(CC) -arch i386 -I$(PD_SRC_PATH) -o $*.o -c $*.c -c -DPD -Werror \
		-Wno-unused -Wstrict-prototypes -Wno-parentheses -Wno-switch -O3 \
		-funroll-loops -fomit-frame-pointer
	$(CC) -arch i386 -bundle -undefined suppress -flat_namespace \
		-o $*.d_fat $*.o
	rm -f $*.o

