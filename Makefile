PREFIX ?= /usr/local

EXEC=banner_launcher
FAKE=debug_config

.PHONY: install uninstall clean fake

all: $(EXEC)

fake: $(FAKE)

libs := gtk+-3.0 libcurl
CFLAGS += `pkg-config --cflags $(libs)`
LDFLAGS += `pkg-config --libs $(libs)`

$(EXEC): $(wildcard src/*.c)
	gcc -g $(CFLAGS) $^ $(LDFLAGS) -o $@ 

install: $(EXEC)
	install -D $(EXEC) $(DESTDIR)$(PREFIX)/bin 

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC)

clean:
	rm -f $(EXEC)
	rm -fr debug_config

$(FAKE): $(EXEC)
	bash generate_fake_entries.sh
