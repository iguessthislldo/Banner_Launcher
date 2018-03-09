EXEC=banner_launcher

all: $(EXEC)

$(EXEC): launcher.c Entry.c steam.c
	gcc -g `pkg-config --cflags gtk+-3.0` -o $@ $^ `pkg-config --libs gtk+-3.0`

clean:
	rm -f $(EXEC)
