CC = gcc
CFLAGS = -Wall -Werror -O0 -g
LDLIBS = -lcurses

OBJS = exec.o cpu_estado.o es.o mem.o rel.o term.o instr.o err.o mmu.o \
			 tela.o contr.o so.o teste.o processos.o
OBJS_MONT = instr.o err.o montador.o
MAQS = ex1.maq ex2.maq ex3.maq ex4.maq ex5.maq init.maq p1.maq p2.maq initso.maq
TARGETS = teste montador

all: ${TARGETS}
# para gerar o montador, precisa de todos os .o do montador
montador: ${OBJS_MONT}

# para gerar o programa de teste, precisa de todos os .o)
teste: ${OBJS}

# para gerar so.o, precisa, além do so.c, dos arquivos .maq
so.o: so.c ${MAQS}

# para transformar um .asm em .maq, precisamos do montador
%.maq: %.asm montador
	./montador $*.asm > $*.maq

clean:
	rm ${OBJS} ${OBJS_MONT} ${TARGETS} ${MAQS}
