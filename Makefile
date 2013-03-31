# Makefile for client/server

SERVER          = server
CLIENT          = client
TOOL		 = getpwd
SERVER_SOURCES  = server.c
CLIENT_SOURCES  = client.c
TOOL_SOURCES 	 = getpwd.c

DEFINES         = 

CFLAGS         = -std=c99 -g -pedantic -Wall

LIBRARIES       = #-llibrary_name

CC              = gcc
SERVER_OBJECTS  = $(SERVER_SOURCES:.c=.o)
CLIENT_OBJECTS  = $(CLIENT_SOURCES:.c=.o)
TOOL_OBJECTS 	= $(TOOL_SOURCES:.c=.o)
INCLUDES        = #-I.
LIBDIRS         = 
LDFLAGS         = $(LIBDIRS) $(LIBRARIES)

###########################################

.SUFFIXES: .c .o

.c.o:
		$(CC) $(CFLAGS) -c $<

###########################################

all:		$(SERVER) $(CLIENT) $(TOOL)

rebuild:	clean all

$(SERVER):	$(SERVER_OBJECTS)
		$(CC) $(SERVER_OBJECTS) $(LDFLAGS) -o $@

$(CLIENT):	$(CLIENT_OBJECTS)
		$(CC) $(CLIENT_OBJECTS) $(LDFLAGS) -o $@
		
$(TOOL):	$(TOOL_OBJECTS)
		$(CC) $(TOOL_OBJECTS) $(LDFLAGS) -o $@		

###########################################

clean:
	rm -fr core* *~ $(SERVER_OBJECTS) $(CLIENT_OBJECTS) $(TOOL_OBJECTS) $(SERVER) $(CLIENT) $(TOOL) .make.state .sb
