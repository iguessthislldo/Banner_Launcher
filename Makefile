EXEC=banner_launcher

all: $(EXEC)

$(EXEC): main.c Entry.c
	gcc `pkg-config --cflags gtk+-3.0` -o $@ $^ `pkg-config --libs gtk+-3.0`

clean:
	rm -f $(EXEC)
