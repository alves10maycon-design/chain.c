ALLEGRO_VERSION=5.0.10
MINGW_VERSION=4.7.0
FOLDER=C:

FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
INCLUDE_ALLEGRO=\include


all: chain


chain: chain.o

	gcc -o chain chain.o -lallegro -lallegro_main -lallegro_primitives -lallegro_font -lallegro_ttf -lallegro_audio -lallegro_image -lallegro_acodec -lm

chain.o: chain.c		

	gcc -c chain.c
	
	
clean:
	rm chain.o 
	rm chain 




#ALLEGRO_VERSION=5.0.10
#MINGW_VERSION=4.7.0
#FOLDER=C:

#FOLDER_NAME=\allegro-$(ALLEGRO_VERSION)-mingw-$(MINGW_VERSION)
#PATH_ALLEGRO=$(FOLDER)$(FOLDER_NAME)
#LIB_ALLEGRO=\lib\liballegro-$(ALLEGRO_VERSION)-monolith-mt.a
#iNCLUDE_ALLEGRO=\include

#all: chain.exe
#all: chain 

#chain.exe: chain.o
#chain: chain.o
#       gcc -o chain.exe chain.o $(PATH_ALLEGRO)$(LIB_ALLEGRO)
#chain.o: chain.c
#       gcc -I $(PATH_ALLEGRO)$(INCLUDE_ALLEGRO) -c chain.c     
#clean:
#       del chain.o 
#      del chain.exe
