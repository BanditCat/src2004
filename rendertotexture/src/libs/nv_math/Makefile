MAIN=../../../lib/libnv_math.so

CC=g++

CFLAGS= -DUNIX -Wall -g 

INCLUDES= -I../../../include/glh -I../../../include/shared -I../../../include/nv_math -I ../../../include/nv_util -I../../../include/nv_memory

LFLAGS= 

SRCS = nv_algebra.cpp

OBJS = $(SRCS:.cpp=.o)

$(MAIN):  $(OBJS)
	$(CC) -shared -Wl,-soname,libnv_math.so $(CFLAGS) $(OBJS) -o $(MAIN) $(LFLAGS)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<

clean:
	rm -f *.o *~ $(MAIN)

install:
	make
	install -c -m644 $(MAIN) $(ROOT)/usr/local/lib
