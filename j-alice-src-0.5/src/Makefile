PYTHON_INCDIR = $(PYTHON_INCLUDE)

JALICE_SRC = AimlWriter.cpp Element.cpp FlashResponder.cpp HTTPProcessor.cpp \
	Handler.cpp IrcResponder.cpp Kernel.cpp Logger.cpp Main.cpp Match.cpp \
	NodeWalker.cpp Nodemaster.cpp Parser.cpp PreParser.cpp PredicateEngine.cpp \
	SaxParser.cpp ServerSocket.cpp Socket.cpp SocketHandler.cpp Stream.cpp \
	StringTokenizer.cpp Substituter.cpp Template.cpp TemplateProcessor.cpp \
	TimeKeeper.cpp TokenProxyListener.cpp Utils.cpp XmlSocketResponder.cpp
JALICE_HDR = AimlProcessor.h AimlWriter.h BotProcessor.h BrProcessor.h Compat.h \
	ConditionProcessor.h Config.h Element.h FlashResponder.h FormalProcessor.h \
	GetProcessor.h HTTPProcessor.h Handler.h IdProcessor.h ImpExpProcessor.h \
	InputProcessor.h IrcProcessor.h JavaScriptProcessor.h Kernel.h \
	LearnProcessor.h Logger.h LowercaseProcessor.h Main.h Match.h \
	NameProcessor.h NodeWalker.h Nodemapper.h Nodemaster.h NormalizeProcessor.h \
	Parser.h PersonProcessor.h Pointer.h PreParser.h PredicateEngine.h \
	RandomProcessor.h Responder.h SaxParser.h SecureProcessor.h \
	SentenceProcessor.h ServerSocket.h SetProcessor.h Socket.h SocketHandler.h \
	SrProcessor.h SraiProcessor.h StarProcessor.h Stream.h StringTokenizer.h \
	SubstituteProcessor.h Substituter.h SystemProcessor.h Template.h \
	TemplateProcessor.h ThatProcessor.h ThatstarProcessor.h ThinkProcessor.h \
	TimeKeeper.h TokenProxyListener.h TopicstarProcessor.h UppercaseProcessor.h \
	Utils.h VersionProcessor.h XmlSocketResponder.h wxwindows.h
JALICE_OBJ = $(JALICE_SRC:.cpp=.o)

HOWIE_SRC = jalice-py.cpp
HOWIE_OBJ = $(HOWIE_SRC:.cpp=.o)

SWIG_INPUT = jalice.i
SWIG_SRC = jalice_wrap.cxx
SWIG_OBJ = $(SWIG_SRC:.cxx=.o)
SWIG_PY  = $(SWIG_INPUT:.i=.py)

OBJ = $(JALICE_OBJ) $(HOWIE_OBJ) $(SWIG_OBJ)

OUTPUT = _jalice.so

# Set up C++ compiler options
CC = g++
CCFLAGS = -O2 -s -fpic -DJALICE_BUILDING_DLL
INCDIR = -I$(PYTHON_INCDIR)
LIBDIR = 
LIBS =

# Set up linker options
LD = ld
LDFLAGS = -G

# Set up makedepend options
DEPENDOPT = $(CCFLAGS) $(INCDIR) -o.o -DIGNORE_DEPEND

# Set up SWIG options
SWIG = swig
SWIGFLAGS = -c++ -python

#########################################################
# Conversion rules
.SUFFIXES: .cpp .cxx
.cpp.o:
	$(CC) $(CCFLAGS) -c $(INCDIR) $<
.cxx.o:
	$(CC) $(CCFLAGS) -c $(INCDIR) $<

all: $(OUTPUT)

$(OUTPUT): $(OBJ)
	g++ -shared -o $(OUTPUT) $(OBJ)

$(SWIG_OBJ): $(SWIG_SRC)

$(SWIG_SRC): $(SWIG_INPUT)
	$(SWIG) $(SWIGFLAGS) -o $(SWIG_SRC) $(SWIG_INPUT)

clean:
	rm -f *.o $(OUTPUT) $(SWIG_SRC) $(SWIG_PY)

install: $(OUTPUT)
	strip $(OUTPUT)
	cp $(OUTPUT) ../../howie/backends/
	cp $(SWIG_PY) ../../howie/backends/

depend: $(JALICE_SRC) $(JALICE_HDR)
	touch -f Makefile.depend
	makedepend -fMakefile.depend $(JALICE_SRC)
	rm -f Makfile.depend.bak

include Makefile.depend
