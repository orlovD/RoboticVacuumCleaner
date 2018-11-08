OBJS = Point.o House.o Configuration.o RunResult.o ReadAllFiles.o Montage.o Encoder.o Sensor.o Video.o AlgorithmRegistration.o AlgorithmRegistrar.o Simulation.o Simulator.o 
CC = g++
DEBUG = -g
CFLAGS = -Wall -c -O2 -pedantic -pthread -std=c++1y $(DEBUG)
LFLAGS = -Wall $(DEBUG)

simulator : $(OBJS)
	$(CC) $(LFLAGS) $(OBJS) -o simulator -ldl -rdynamic -pthread

Point.o : Point.h Point.cpp
	$(CC) $(CFLAGS) Point.cpp

Montage.o : Montage.h Montage.cpp Utils.h
	$(CC) $(CFLAGS) Montage.cpp

Encoder.o : Encoder.h Encoder.cpp Utils.h
	$(CC) $(CFLAGS) Encoder.cpp

Video.o : Video.h Video.cpp House.h Point.h ReadAllFiles.h Montage.h Encoder.h Utils.h
	$(CC) $(CFLAGS) Video.cpp

House.o : House.h House.cpp Utils.h Point.h ReadAllFiles.h Montage.h
	$(CC) $(CFLAGS) House.cpp

Configuration.o : Configuration.h Configuration.cpp Utils.h
	$(CC) $(CFLAGS) Configuration.cpp

RunResult.o : RunResult.h RunResult.cpp
	$(CC) $(CFLAGS) RunResult.cpp

ReadAllFiles.o : ReadAllFiles.h ReadAllFiles.cpp Configuration.h
	$(CC) $(CFLAGS) ReadAllFiles.cpp

Sensor.o : Sensor.h Sensor.cpp Direction.h AbstractAlgorithm.h AbstractSensor.h Point.h House.h
	$(CC) $(CFLAGS) Sensor.cpp

AlgorithmRegistration.o : AlgorithmRegistration.h AlgorithmRegistration.cpp Direction.h AbstractAlgorithm.h AlgorithmRegistrar.h Utils.h
	$(CC) $(CFLAGS) AlgorithmRegistration.cpp

AlgorithmRegistrar.o : AlgorithmRegistrar.h AlgorithmRegistrar.cpp Direction.h AbstractAlgorithm.h ReadAllFiles.h AlgorithmRegistration.h Utils.h
	$(CC) $(CFLAGS) AlgorithmRegistrar.cpp

Simulation.o : Simulation.h Simulation.cpp Direction.h AbstractAlgorithm.h AbstractSensor.h Configuration.h Sensor.h Point.h House.h RunResult.h Video.h Utils.h
	$(CC) $(CFLAGS) Simulation.cpp

Simulator.o : Simulator.h Simulator.cpp Direction.h AbstractAlgorithm.h AbstractSensor.h Configuration.h Simulation.h ReadAllFiles.h Utils.h
	$(CC) Simulator.cpp $(CFLAGS) 

SmartAlgorithm.o : SmartAlgorithm.h SmartAlgorithm.cpp Direction.h AbstractAlgorithm.h AbstractSensor.h Utils.h
	$(CC) $(CFLAGS) SmartAlgorithm.cpp -mcmodel=large

_321892366_A.o : _321892366_A.h _321892366_A.cpp Direction.h SmartAlgorithm.h AbstractSensor.h Utils.h AlgorithmRegistration.h
	$(CC) $(CFLAGS) _321892366_A.cpp -mcmodel=large

_321892366_B.o : _321892366_B.h _321892366_B.cpp Direction.h SmartAlgorithm.h AbstractSensor.h Utils.h AlgorithmRegistration.h
	$(CC) $(CFLAGS) _321892366_B.cpp -mcmodel=large

_321892366_C.o : _321892366_C.h _321892366_C.cpp Direction.h SmartAlgorithm.h AbstractSensor.h Utils.h AlgorithmRegistration.h
	$(CC) $(CFLAGS) _321892366_C.cpp -mcmodel=large

321892366_A_.so : _321892366_A.o
	$(CC) _321892366_A.o SmartAlgorithm.o -shared -fPIC -ldl -o 321892366_A_.so

321892366_B_.so : _321892366_B.o
	$(CC) _321892366_B.o SmartAlgorithm.o -shared -fPIC -ldl -o 321892366_B_.so

321892366_C_.so : _321892366_C.o
	$(CC) _321892366_C.o SmartAlgorithm.o -shared -fPIC -ldl -o 321892366_C_.so

ScoreFormula.o : ScoreFormula.cpp Utils.h
	$(CC) $(CFLAGS) -fPIC ScoreFormula.cpp

score_formula.so : ScoreFormula.o
	$(CC) ScoreFormula.o -shared -fPIC -ldl -o score_formula.so

all : simulator Point.o House.o Configuration.o RunResult.o ReadAllFiles.o Montage.o Encoder.o Video.o Sensor.o AlgorithmRegistration.o AlgorithmRegistrar.o Simulation.o Simulator.o SmartAlgorithm.o 321892366_A_.so 321892366_B_.so 321892366_C_.so score_formula.so

clean:
	\rm -f *.so *.o simulator

