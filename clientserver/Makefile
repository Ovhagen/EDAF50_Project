# Define the compiler and the linker. The linker must be defined since
# the implicit rule for linking uses CC as the linker. g++ can be
# changed to clang++.
CXX = g++
CC  = $(CXX)

# Generate dependencies in *.d files
DEPFLAGS = -MT $@ -MMD -MP -MF $*.d

# Define preprocessor, compiler, and linker flags. Uncomment the # lines
# if you use clang++ and wish to use libc++ instead of GNU's libstdc++.
# -g is for debugging.
CPPFLAGS =  -std=c++11 -I.
CXXFLAGS =  -O2 -Wall -Wextra -pedantic-errors -Wold-style-cast
CXXFLAGS += -std=c++11
CXXFLAGS += -g
CXXFLAGS += $(DEPFLAGS)
LDFLAGS =   -g
#CPPFLAGS += -stdlib=libc++
#CXXFLAGS += -stdlib=libc++
#LDFLAGS +=  -stdlib=libc++

# Targets

all: libclientserver.a news_server news_client

# Create the library; ranlib is for Darwin (OS X) and maybe other systems.
# Doesn't seem to do any damage on other systems.

libclientserver.a: connection.o server.o dbondisk.o dbinmemory.o newsgroup.o messagehandler.o news_server.o
	ar rv libclientserver.a  connection.o server.o
	ranlib libclientserver.a
#test_in: dbinmemory.o newsgroup.o
news_server: news_server.o dbinmemory.o dbondisk.o newsgroup.o messagehandler.o server.o connection.o

news_client: news_client.o newsgroup.o messagehandler.o connection.o commandhandler.o

# Phony targets
.PHONY: all clean distclean

# Standard clean
clean:
	rm -f *.o

distclean: clean
	-rm *.d libclientserver.a


# Include the *.d files
SRC = $(wildcard *.cc)
-include $(SRC:.cc=.d)
