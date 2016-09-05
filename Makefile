# GTK+-3 introduced the native GtkGLArea in 3.16.
# Check that we have at least that version:
ifneq ($(shell pkg-config --atleast-version=3.16 gtk+-3.0 && echo 1 || echo 0),1)
  $(error $(shell  pkg-config --print-errors --atleast-version=3.16 gtk+-3.0))
endif

PROG	 = gtk3-opengl

CFLAGS	+= $(shell pkg-config --cflags gtk+-3.0)
CFLAGS	+= -std=c99

LIBS	+= $(shell pkg-config --libs gtk+-3.0)
LIBS	+= -lGLEW -lGL -lm

OBJS	 = $(patsubst %.c,%.o,$(wildcard *.c))
OBJS	+= $(patsubst %.glsl,%.o,$(wildcard shaders/*/*.glsl))
OBJS	+= $(patsubst %.svg,%.o,$(wildcard textures/*.svg))

.PHONY: clean

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LIBS)

textures/%.png: textures/%.svg
	rsvg-convert --format png --output $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $^

%.o: %.glsl
	$(LD) --relocatable --format=binary -o $@ $^

%.o: %.png
	$(LD) --relocatable --format=binary -o $@ $^

clean:
	rm -f $(PROG) $(OBJS)
