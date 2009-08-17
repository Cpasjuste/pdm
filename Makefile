NAME	  = pdm

ifndef PANDORA
CC        = gcc
STRIP     = strip
else
CC        = arm-none-linux-gnueabi-gcc
STRIP     = arm-none-linux-gnueabi-strip
endif

LDFLAGS   = -Llib 

ifndef PANDORA
CFLAGS    = -Werror `sdl-config --cflags` -Iinclude
else
CFLAGS    = -Werror -I$(PNDSDK)/usr/include -Iinclude -DPANDORA
endif

ifndef PANDORA
LIBS	  = -lconfig -lSDL_image -lSDL_mixer -lSDL_ttf -lfreetype `sdl-config --libs`
else
LIBS	  = -L$(PNDSDK)/usr/lib -lconfig_pnd -lIMGegl -lsrv_um -lXau -lXdmcp -lXi -lXext \
		-lX11 -lGLES_CM -lGLES2D -lSDL13 -lpng -ltiff -ljpeg -lfreetype -lstdc++ -lz
endif

OBJECTS   = main.o config.o utils.o

all: $(NAME)

$(NAME): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -o $@
	$(STRIP) $(NAME)

.cc.o:
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f $(OBJECTS) $(NAME)
