CFLAGS += -std=c99 `pkg-config --cflags gtk+-3.0`
LDFLAGS += `pkg-config --libs gtk+-3.0` -lm -lGL -lGLEW

PROG = gtk3-opengl
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
SHADERS = $(patsubst %.glsl,%.o,$(wildcard shaders/*.glsl))

.PHONY: clean

$(PROG): $(OBJS) $(SHADERS)
	$(CC) -o $@ $^ $(LDFLAGS)

shaders/%.o: shaders/%.glsl
	$(LD) --relocatable --format=binary -o $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) -c $^

clean:
	rm -f $(PROG) $(OBJS) $(SHADERS)
