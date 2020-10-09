
cc = gcc
C_FLAGC = -Wall
.PHONY = all clean
all:fi

fi: fi.o
	        $(cc) $(C_FLAGS) -pthread fi.c  -o fi
clean:
	        rm -f fi.o fi


