SRC_MTHREAD = src/mthread.c
OBJ_MTHREAD = bin/mthread.o
OUT_LIB = lib/libmthread.a
FLAGS = -Wall -lrt
INCLUDES = -I./include/

FLAGS_TESTES = -L./lib -lmthread -Wall -lrt
TESTE = testes/teste
TESTE1 = testes/teste1
TESTE2 = testes/teste2
TESTE_MLOCK = testes/teste_mlock
TESTE_MJOIN = testes/teste_mjoin
TESTE_LISTA= testes/teste_lista

teste_lista_run: teste_lista_compile
		./$(TESTE_LISTA)
teste_lista_compile: teste_mjoin_run
		gcc $(TESTE_LISTA).c -o $(TESTE_LISTA) $(FLAGS_TESTES)

teste_mjoin_run: teste_mjoin_compile
		./$(TESTE_MJOIN)
teste_mjoin_compile: teste_mlock_run
		gcc $(TESTE_MJOIN).c -o $(TESTE_MJOIN) $(FLAGS_TESTES)

teste_mlock_run: teste_mlock_compile
		./$(TESTE_MLOCK)
teste_mlock_compile: teste2_run
		gcc $(TESTE_MLOCK).c -o $(TESTE_MLOCK) $(FLAGS_TESTES)

teste2_run: teste2_compile
		./$(TESTE2)
teste2_compile: teste1_run
		gcc $(TESTE2).c -o $(TESTE2) $(FLAGS_TESTES)

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
		rm $(TESTE) $(TESTE1) $(TESTE2) $(TESTE_MLOCK) $(TESTE_MYIELD)
