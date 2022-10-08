# sepc

* TP Allocateur mémoire: malloc
  Ce squelette permet d'implanter un allocateur mémoire virtuelle. Le
  sujet de TP expliquant le travail est diponible sur
  http://ensiwiki.ensimag.fr dans [[https://ensiwiki.ensimag.fr/index.php?title=SEPC-S1][la page SEPC Semestre 1]]

** TP antécédents
    - le tp mémoire allocateur physique: first fit, buddy, chainage
    - le tp mmap
** Nouveauté par rapport aux TPs précédents
   - allocateur virtuelle en trois tailles, plus progressif.
   - une arena par thread
   - croissance de l'arena par thread en utilisant mmap et une
     structure =_Thread_local=
   - récursive doubling pour l'allocation des chunk et du pool principal
   - très gros (plus 128ko): mmap en direct
   - très petit pour alloc de moins de 64o, par chunk de 96o
   - buddy pour le pool principal
   - marquage des blocs (avant après) pour avoir sa vrai taille
     - un magic entier
       - utiliser LCG MMIX de Knuth (64 bits) pour générer la valeur
         magique à partir de l'addresse utilisée
	 https://en.wikipedia.org/wiki/Linear_congruential_generator

* Idées d'améliorations
** TODO En faire une bibliothèque dynamique
   - capable d'être utilisée à la place du malloc de la bibliothèque C
** TODO Utiliser git bundle au lieu de Cpack
   - Rendu complet, compact et avec toutes les informations de
     contributions.
   - Risque de rendu vide accru, si les étudiants n'arrivent pas à
     faire un commit
   - peut-être plus difficile pour les Phelma qui doivent apprendre
     git en plus pour pouvoir rendre quelque chose ? C'est peut-être
     le bon TP pour commencer.
** TODO discussion avec Vincent, si il est intéréssé ?





# Base for a multi-threaded video player #

## References ##

The idea came from the excellent ffmepg + SDL tutorial
http://dranger.com/ffmpeg/ 

This videoplayer does not use ffmpeg but libogg, libtheora and
libvorbis. It uses also SDL2 for the rendering and playing sound.

The ogg/vorbis ogg/theora tutorials of
https://bluishcoder.co.nz/tags/ogg/index.html
are a very good material for understanding of the libraries.

The synchronization is quite different. This code uses 1 thread per stream and
common real-time deadline to set up synchronization. The sound playing
is mostly managed by SDL2. The displayed frame is used with an
additional thread and producer/consumer algorithms to synchronize
stream reading and displaying.

## Compilation and unit testing ##

The compilation process use Cmake

To compile:
	```sh
	cd build
	cmake ..
	make
	```

## Goal ##

The goal is to complete and parallelize the provided code using the
hints of the subject.

### Actions that threads managed ###

1. reading initialization packet in ogg file; init the sdl (video and audio)
2. initializing the theora stream
3. initializing the vorbis stream
4. reading packets in ogg file (twice, each stream reading thread) 
5. decoding theora
6. decoding vorbis
7. displaying the video stream with the sdl
8. playing the sound with the sdl

### Threads ###

1. Thread 1: main thread; init the sdl
2. Thread 2: theora stream reading and decoding video, write texture
3. Thread 3: vorbis stream reading and decoding sound
4. Thread 4: displaying video frame







ensimag-shell
=============

Squelette pour le TP shell

Tous les fichiers sont sous licence GPLv3+

Introduction
----------

Ces fichiers servent de base à un TP sur le shell de commande à la Unix.

Spécificités de ce squelette:
- plusieurs variantes (libre choix par les étudiants)
- jeux de tests fournis aux étudiants
- utilisation de Gnu Readline
- Scheme (interpréteur Guile; Javascript possible)

Compilation et lancement des tests
----------

cd ensimag-shell
cd build
cmake ..
make
make test



Autres
------

Les premières versions imposaient la variante par un modulo sur le
sha512 sur de la liste triée des logins des étudiants. Cela peut être
réalisé complètement en CMake (>= 2.8).


