EXEC=banner_launcher
FAKE=debug_config

.PHONY: install uninstall clean fake

all: $(EXEC)

fake: $(FAKE)

$(EXEC): launcher.c Entry.c steam.c util.c main_window.c
	gcc -g `pkg-config --cflags gtk+-3.0` -o $@ $^ -lcurl `pkg-config --libs gtk+-3.0`

install: $(EXEC)
	cp $(EXEC) /usr/local/bin
	chmod +x /usr/local/bin/$(EXEC)

uninstall:
	rm -f /usr/local/bin/$(EXEC)

clean:
	rm -f $(EXEC)
	rm -fr debug_config

$(FAKE): $(EXEC)
	bash generate_fake_entries.sh
