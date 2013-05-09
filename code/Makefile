COMPILER = g++ 
FLAGS = -lgsl -lgslcblas -lm 
EXECUTABLE = smm

OBJECTS = BPException.o BPMath.o Dataset.o Letter.o MinimizerBase.o NumVec.o SMMCrossValidate.o SMMOptimizeLambda.o SMMRepeater.o SMMSeqMatrix.o SMMSeqPair.o SMMSet.o SMMSolve.o SeqMatrix.o SeqPair.o SeqSet.o VSet.o stdafx.o tinystr.o tinyxml.o tinyxmlerror.o tinyxmlparser.o smm.o


$(EXECUTABLE) :  $(OBJECTS) 
	$(COMPILER) $(FLAGS) -o $(EXECUTABLE) $(OBJECTS) /usr/local/boost_1_52_0/stage/lib/libboost_program_options.a

%.o : %.cpp 
	$(COMPILER) -o $*.o -c $*.cpp 
 
