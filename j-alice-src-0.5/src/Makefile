PYTHON_INCDIR = /usr/local/include/python2.2
PYTHON_LIBDIR = /usr/local/lib/python2.2/config
PYTHON_LIB    = python2.2

JALICE_SRC = AimlWriter.cpp Element.cpp FlashResponder.cpp HTTPProcessor.cpp \
	Handler.cpp IrcResponder.cpp Kernel.cpp Logger.cpp Main.cpp Match.cpp \
	NodeWalker.cpp Nodemaster.cpp Parser.cpp PreParser.cpp PredicateEngine.cpp \
	SaxParser.cpp ServerSocket.cpp Socket.cpp SocketHandler.cpp Stream.cpp \
	StringTokenizer.cpp Substituter.cpp Template.cpp TemplateProcessor.cpp \
	TimeKeeper.cpp TokenProxyListener.cpp Utils.cpp XmlSocketResponder.cpp
JALICE_OBJ = $(JALICE_SRC:.cpp=.o)
HOWIE_SRC = jalice-py.cpp
HOWIE_OBJ = $(HOWIE_SRC:.cpp=.o)
SWIG_INPUT = jalice.i
SWIG_SRC = jalice_wrap.cxx
SWIG_OBJ = $(SWIG_SRC:.cxx=.o)

OBJ = $(JALICE_OBJ) $(HOWIE_OBJ) $(SWIG_OBJ)

OUTPUT = _jalice.so

# Set up C++ compiler options
CC = g++
CCFLAGS = -fPIC -DJALICE_BUILDING_DLL -c
INCDIR = -I$(PYTHON_INCDIR) -I/usr/include/g++/backward/
LIBDIR = -L$(PYTHON_LIBDIR)
LIBS = -l$(PYTHON_LIB)

# Set up linker options
LD = ld
LDFLAGS = -G

# Set up SWIG options
SWIG = swig
SWIGFLAGS = -c++ -python

#########################################################
# Conversion rules
.SUFFIXES: .cpp .cxx
.cpp.o:
	$(CC) $(CCFLAGS) $(INCDIR) $<
.cxx.o:
	$(CC) $(CCFLAGS) $(INCDIR) $<

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	$(LD) $(LDFLAGS) -o $(OUTPUT) $(OBJ) $(LIBDIR) $(LIBS)

$(SWIG_OBJ): $(SWIG_SRC)

$(SWIG_SRC): $(SWIG_INPUT)
	$(SWIG) $(SWIGFLAGS) -o $(SWIG_SRC) $(SWIG_INPUT)

clean:
	rm -f *.o $(OUTPUT) $(SWIG_SRC)

install:
	cp $(OUTPUT) ../../howie/backends/
	cp jalice.py ../../howie/backends/