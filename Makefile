CFLAGS  += -std=c99
LDFLAGS += -lm -lGL -lGLEW

CFLAGS_GTK  := $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS_GTK := $(shell pkg-config --libs   gtk+-3.0)

PROG = gtk3-opengl
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
SHADERS = $(patsubst %.glsl,%.o,$(wildcard shaders/*.glsl))

.PHONY: clean

$(PROG): $(OBJS) $(SHADERS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDFLAGS_GTK)

shaders/%.o: shaders/%.glsl
	$(LD) --relocatable --format=binary -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) $(CFLAGS_GTK) -c $^

clean:
	rm -f $(PROG) $(OBJS) $(SHADERS)
