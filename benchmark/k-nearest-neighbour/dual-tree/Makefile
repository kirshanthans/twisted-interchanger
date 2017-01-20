CC = clang++

CLANGINC = /usr/local/lib/clang/3.8.0/include/

CCFLAGS =-w -std=c++0x -c -O3 -I./$(SOURCEDIR) -I$(CLANGINC)
LDFLAGS =-O3

SOURCEDIR    = src
BUILDDIR     = build

SOURCE    = main tree traversal util
HEAD      = main tree traversal util defines common

HEADER = $(join $(addsuffix $(SOURCEDIR)/, $(dir $(HEAD))), $(notdir $(HEAD:=.h)))
OBJECT = $(join $(addsuffix $(BUILDDIR)/, $(dir $(SOURCE))), $(notdir $(SOURCE:=.o)))

EXECUTABLE = run

all: exec

perf: CCFLAGS += -DPERF
perf: LDFLAGS += -lpapi
perf: exec

debug: CCFLAGS +=-g -DDEBUG
debug: LDFLAGS +=-g 
debug: exec

prof: CCFLAGS +=-pg
prof: LDFLAGS +=-pg
prof: exec

exec: directory $(OBJECT) 
	$(CC) $(OBJECT) $(LDFLAGS) -o $(EXECUTABLE) 

directory:
	mkdir -p $(BUILDDIR) 

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp $(HEADER)
	$(CC) $(CCFLAGS) $< -o $@

clean:
	rm -rf $(EXECUTABLE) $(BUILDDIR) *.dot *.png *.out *.txt *.log
