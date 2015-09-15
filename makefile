TARGET = scp

CC      = g++
CFLAGS  = -O2 -Iinclude -ansi -Wall -pedantic -DIL_STD
LDFLAGS = -O2 -lconcert -lilocplex -lcplex -lm -lpthread -lz

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
DEP = $(SRC:.cpp=.d)

.PHONY: all clean distclean dist

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

src/%.o: src/%.cpp
	$(CC) -o $@ -c $< $(CFLAGS) $(DFLAGS)

src/%.d: src/%.cpp
	@ $(CC) -o $@ -MM -MT '$(<:.cpp=.o)' $< $(CFLAGS) $(DFLAGS)

clean:
	@ rm -vf $(TARGET) $(OBJ) $(DEP)

distclean: clean
	@ find . -name "*~" -exec rm -f {} + -printf "removed '%P'\n"

dist: distclean
	@ tar -vczf "$(TARGET)_$(shell date --rfc-3339='date').tar.gz" \
		--ignore-failed-read \
		--exclude '$(TARGET)_*.tar.gz' \
		$(shell find . -type f -printf "'%P' ")

-include $(DEP)
