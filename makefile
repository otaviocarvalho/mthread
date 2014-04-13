SRC_MTHREAD = src/mthread.c
OBJ_MTHREAD = bin/mthread.o
OUT_LIB = lib/libmthread.a
FLAGS = -Wall -lrt
INCLUDES = -I./include/

FLAGS_TESTES = -L./lib -lmthread -Wall -lrt
TESTE = testes/teste
TESTE1 = testes/teste1

teste1_run: teste1_compile
		./$(TESTE1)
teste1_compile: teste_run
		gcc $(TESTE1).c -o $(TESTE1) $(FLAGS_TESTES)
teste_run: teste_compile
		./$(TESTE)
teste_compile: $(OUT_LIB)
		gcc $(TESTE).c -o $(TESTE) $(FLAGS_TESTES)

$(OUT_LIB): $(OBJ_MTHREAD)
		ar crs $(OUT_LIB) $(OBJ_MTHREAD)

$(OBJ_MTHREAD): $(SRC_MTHREAD)
		gcc -c $(SRC_MTHREAD) -o $(OBJ_MTHREAD) $(FLAGS)

clean:
		rm -rf $(OBJ_MTHREAD) $(OUT_LIB)
		rm $(TESTE) $(TESTE1)
