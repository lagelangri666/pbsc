PBS_SRC_DIR = /home/zwhuang/opt/torque/torque-6.1.1.1/src/

CC = g++
MYCFLAGS= $(CXXFLAGS) -std=c++0x -DHAVE_CONFIG_H -MD -MP -MF
#-W -Wall -Wextra -Wno-unused-parameter -Wno-long-long -Werror -Wno-sign-compare -fPIC -MT qstat-qstat.o -MD -MP -MF
MYINCLUDES = -I. -I$(PBS_SRC_DIR)/cmds -I$(PBS_SRC_DIR)/include `xml2-config --cflags`
LIB		= /opt/torque/lib/libtorque.a $(PBS_SRC_DIR)/lib/Libutils/libutils.a -L/usr/lib64 -ltcl8.5 -ldl -lieee -lm -lz -lxml2 -lz -lm   -lpthread -lrt

EXE		= ./nodetest
OBJ_DIR	=./obj
OBJ		=  $(OBJ_DIR)/libqstat.o $(OBJ_DIR)/qstat-MXML.o
OBJ2 		=  $(OBJ_DIR)/treeend.o
#$(OBJ_DIR)/treeend.o

STATICLIB =  ./libqstat.a

ALL:$(OBJ) $(STATICLIB) $(EXE)
		
$(OBJ_DIR)/%.o : %.cpp
	$(CC) -c $(MYCFLAGS)   $(MYINCLUDES)  -o $@ $<

$(STATICLIB) : $(OBJ)
	ar rcs libqstat.a $(OBJ)
#	ar rcs libqstat_all.a $(OBJ_DIR)/libqstat.o 

$(EXE)	:$(OBJ) $(OBJ2)
	 $(CC) -o $@ $(OBJ) $(OBJ2)  $(LIB) 

#install::
#	install -m 755 	$(SRCDIR)/CANNY	 $(MODULEINSTALLROOT)/$(MODULEINISTALLSUBDIR) 

clean	:
	rm -f $(EXE) $(OBJ) $(STATICLIB)

