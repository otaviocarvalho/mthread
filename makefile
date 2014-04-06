SRC_MTHREAD = src/mthread.c
OBJ_MTHREAD = bin/mthread.o
OUT_LIB = lib/libmthread.a
FLAGS = -Wall -lrt
INCLUDES = -I./include/

test_run: testes/teste
		./testes/teste

test_compile: $(OUT_LIB)
		gcc testes/teste.c -o testes/teste -L./lib -lmthread

$(OUT_LIB): $(OBJ_MTHREAD)
		ar crs $(OUT_LIB) $(OBJ_MTHREAD)

$(OBJ_MTHREAD): $(SRC_MTHREAD)
		gcc -c $(SRC_MTHREAD) -o $(OBJ_MTHREAD) $(FLAGS)

clean:
		rm -rf $(OBJ_MTHREAD) $(OUT_LIB)
