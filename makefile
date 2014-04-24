SRC_MTHREAD = src/mthread.c
OBJ_MTHREAD = bin/mthread.o
OUT_LIB = lib/libmthread.a
FLAGS = -Wall -lrt
INCLUDES = -I./include/

FLAGS_TESTES = -L./lib -lmthread -Wall -lrt
TESTE = testes/teste
TESTE2 = testes/teste2
TESTE_MLOCK = testes/teste_mlock
TESTE_MJOIN = testes/teste_mjoin
TESTE_MCREATE = testes/teste_mcreate
TESTE_MYIELD = testes/teste_myield
TESTE_MJOIN_MCREATE = testes/teste_mjoin_mcreate
TESTE_SCHEDULER_SPN = testes/teste_scheduler_spn

teste_scheduler_spn_run: teste_scheduler_spn_compile
		./$(TESTE_SCHEDULER_SPN)
teste_scheduler_spn_compile: teste_mjoin_mcreate_run
		gcc $(TESTE_SCHEDULER_SPN).c -o $(TESTE_SCHEDULER_SPN) $(FLAGS_TESTES)

teste_mjoin_mcreate_run: teste_mjoin_mcreate_compile
		./$(TESTE_MJOIN_MCREATE)
teste_mjoin_mcreate_compile: teste_myield_run
		gcc $(TESTE_MJOIN_MCREATE).c -o $(TESTE_MJOIN_MCREATE) $(FLAGS_TESTES)

teste_myield_run: teste_myield_compile
		./$(TESTE_MYIELD)
teste_myield_compile: teste_mcreate_run
		gcc $(TESTE_MYIELD).c -o $(TESTE_MYIELD) $(FLAGS_TESTES)

teste_mcreate_run: teste_mcreate_compile
		./$(TESTE_MCREATE)
teste_mcreate_compile: teste_mjoin_run
		gcc $(TESTE_MCREATE).c -o $(TESTE_MCREATE) $(FLAGS_TESTES)

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
teste2_compile: teste_run
		gcc $(TESTE2).c -o $(TESTE2) $(FLAGS_TESTES)

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
		rm $(TESTE) $(TESTE2) $(TESTE_MLOCK) $(TESTE_MYIELD) $(TESTE_MJOIN) $(TESTE_MCREATE) $(TESTE_MJOIN_MCREATE) $(TESTE_SCHEDULER_SPN)
