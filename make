CC = g++

FLAGS = -std=c++11
FLAGS += -Wall
FLAGS += -g

LINK = $(CC) $(FLAGS) -o

COMPILE = $(CC) $(FLAGS) -c

pipe : pipe.o
  $(LINK) $@ $^
  
socket : socket.o
  $(LINK) $@ $^
  
custom_pipe : custom_pipe.o
  $(LINK) $@ $^
  
custom_socket : custom_socket.o
  $(LINK) $@ $^
  
pipe.o : pipe.cpp pipe.h
  $(COMPILE) $<
  
socket.o : socket.cpp socket.h
  $(COMPILE) $< 
  
custom_pipe.o : custom_pipe.cpp custom_pipe.h
  $(COMPILE) $<
  
socket.o : custom_socket.cpp custom_socket.h
  $(COMPILE) $< 

clean:
  @rm *.o
