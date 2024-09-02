LINUX_CC = gcc
LINUX_CXX = g++

# Opzioni di compilazione
LINUX_CFLAGS = -Wall -O3
LINUX_CXXFLAGS = -Wall -O3 -std=c++17

# Directory di inclusione
LINUX_INCLUDE_DIRS = -Iinclude

# Flag di collegamento
LDFLAGS =

# Nome dell'eseguibile
LINUX_TARGET = $(LINUX_BINS)

# File sorgente
LINUX_SRCS = $(LINUX_OBJS:.o=.c)

all: clean_linux build_linux run

# build
.PHONY: build_linux
build_linux: $(LINUX_TARGET)

# Regola per compilare l'eseguibile
$(LINUX_TARGET): $(LINUX_OBJS)
	$(LINUX_CC) $(LINUX_OBJS) -o $(LINUX_TARGET) $(LDFLAGS)

# Regola per compilare i file oggetto C
%.o: %.c $(LINUX_HEADERS)
	$(LINUX_CC) $(LINUX_CFLAGS) $(LINUX_INCLUDE_DIRS) -c $< -o $@

# Regola per compilare i file oggetto C++
%.o: %.cpp $(LINUX_HEADERS)
	$(LINUX_CXX) $(LINUX_CXXFLAGS) $(LINUX_INCLUDE_DIRS) -c $< -o $@

# Regola per pulire i file generati
.PHONY: clean_linux
clean_linux:
	rm -f $(LINUX_OBJS) $(LINUX_BINS) *~ *.o ./data.txt

# Regola per eseguire il programma
.PHONY: run
run: $(LINUX_TARGET) 
	./$(LINUX_TARGET) $(ARGS)
