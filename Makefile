# GTK+-3 introduced the native GtkGLArea in 3.16.
# Check that we have at least that version:
ifneq ($(shell pkg-config --atleast-version=3.16 gtk+-3.0 && echo 1 || echo 0),1)
  $(error $(shell  pkg-config --print-errors --atleast-version=3.16 gtk+-3.0))
endif

CFLAGS  += -std=c99
LDFLAGS += -lm -lGL -lGLEW

CFLAGS_GTK  := $(shell pkg-config --cflags gtk+-3.0)
LDFLAGS_GTK := $(shell pkg-config --libs   gtk+-3.0)

PROG = gtk3-opengl
OBJS = $(patsubst %.c,%.o,$(wildcard *.c))
SHADERS = $(patsubst %.glsl,%.o,$(wildcard shaders/*/*.glsl))
TEXTURES = $(patsubst %.svg,%.o,$(wildcard textures/*.svg))

.PHONY: clean

$(PROG): $(OBJS) $(SHADERS) $(TEXTURES)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDFLAGS_GTK)

textures/%.png: textures/%.svg
	rsvg-convert --format png --output $@ $^

%.o: %.c
	$(CC) -o $@ $(CFLAGS) $(CFLAGS_GTK) -c $^

%.o: %.glsl
	$(LD) --relocatable --format=binary -o $@ $^

%.o: %.png
	$(LD) --relocatable --format=binary -o $@ $^

clean:
	rm -f $(PROG) $(OBJS) $(SHADERS) $(TEXTURES)
