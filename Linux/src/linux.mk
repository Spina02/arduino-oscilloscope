# Opzioni di compilazione
CFLAGS = -Wall -O3
CXXFLAGS = -Wall -O3 -std=c++17

# Directory di inclusione
INCLUDE_DIRS = -Iinclude

# Flag di collegamento
LDFLAGS =

# Nome dell'eseguibile
TARGET = main

# File sorgente
SRCS = src/serial.c src/plot.c src/utils.c main.c  # Aggiungi main.c qui

# File oggetto
OBJS = $(SRCS:.c=.o)

# Regola per compilare l'eseguibile
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Regola per compilare i file oggetto C
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Regola per compilare i file oggetto C++
%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_DIRS) -c $< -o $@

# Regola per pulire i file generati
.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)

# Regola per eseguire il programma
.PHONY: run
run: $(TARGET)
	./$(TARGET)