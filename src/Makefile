CXX := c++
CFLAGS := -I../thirdparty/librtneat/include -Wall -Wfatal-errors -g -O3
OBJS := organism.o population.o level.o debugdraw.o main.o

all: ../rtneatbox

../rtneatbox: ${OBJS}
	$(CXX) -o $@ $^ -L../thirdparty/librtneat -lrtneat -lbox2d -lglut

.cpp.o:
	$(CXX) ${CFLAGS} -c $<

${OBJS}: *.h

clean:
	rm -f ${OBJS} ../rtneatbox
