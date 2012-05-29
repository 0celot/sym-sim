COMPILER = gcc
CCFLAGS = -c -g -o
psim: list.o report.o uav.o base.o psimmain.o sim.o genetic.o particle.o physics.o simrandom.o
	${COMPILER} -o psim list.o report.o uav.o base.o psimmain.o sim.o genetic.o particle.o physics.o simrandom.o -lm -lmpi

list.o: list.c
	${COMPILER} ${CCFLAGS} list.o list.c

report.o: report.c 
	${COMPILER} ${CCFLAGS} report.o report.c

uav.o: uav.c
	${COMPILER} ${CCFLAGS} uav.o uav.c

base.o: base.c
	${COMPILER} ${CCFLAGS} base.o base.c 

psimmain.o: psimmain.c
	${COMPILER} ${CCFLAGS} psimmain.o psimmain.c 

sim.o: sim.c
	${COMPILER} ${CCFLAGS} sim.o sim.c 

genetic.o: genetic.c
	${COMPILER} ${CCFLAGS} genetic.o genetic.c

particle.o: particle.c
	${COMPILER} ${CCFLAGS} particle.o particle.c 

physics.o: physics.c
	${COMPILER} ${CCFLAGS} physics.o physics.c

simrandom.o: simrandom.c
	${COMPILER} ${CCFLAGS} simrandom.o simrandom.c

clean: 
	rm -rf *.o psim 
 

