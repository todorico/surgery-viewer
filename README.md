# Surgery Viewer

Viewer écrit en C++, utilisant CGAL5, Qt5 et LibQGLViewer pour optimiser la visualisation de maillages surfaciques multi-couches provenant de dissections chirurgicales.

## Dépendances

Les templates C++ sont fortement utilisés par la bibliothèque CGAL. Cela lui permet d'être flexible et générique cependant la durée de compilations s'en retrouve fortement affectée.

Pour accélérer la durée de compilation et donc le développement ce projet applique des optimisations de compilations utilisable seulement avec une version de CMake capable de pré-compiler les fichiers d'en-têtes C++.

- CMake 3.16+

La compilation de la version de CGAL5 incluse dans ce projet nécessite les bibliothèques suivantes 

- Boost
- GMP
- MPFR
- Eigen

Le viewer utilise quant à lui Qt5 pour l'affichage fênétré et Assimp pour l'importation de maillages

- Qt5
- Assimp

Sur les systèmes du type Debian / Ubuntu les dépendances peuvent être installer avec la commande ci-dessous.

```sh
sudo apt-get install libboost-dev libgmp-dev libmpfr-dev libeigen3-dev 'libqt5*-dev' libassimp-dev
```

## Compilation

```sh
# 1. clone surgery-viewer repository

git clone https://github.com/todorico/surgery-viewer.git

# 2. build included CGAL library

cd surgery-viewer/CGAL-5.0.1
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGAL_HEADER_ONLY=OFF -DWITH_Eigen3=ON ..
make

# 3. build surgery-viewer programs

cd ../../ # root of surgery-viewer
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=DebugFast ..
make
```

## Programmes

```
Usage: view (<input-file> | --from=<format>) : Utilitaire permettant de visualiser des objets géométriques.

Usage: prop (<input-file> | --from=<format>) : Utilitaire permettant d'afficher et modifier les propriétés d'un objet géométrique.

Usage: match <threshold> <input-file>... : Utilitaire permettant de produire un super-maillage contenant seulement les parties suffisament différentes provenants de plusieurs maillage d'entrés.

Usage: project <input-file1> <input-file2> : Utilitaire permettant de produire un maillage étant la projection du 1er maillage sur le 2eme.

Usage: rotate (<input-file> | --from=<input-format>) (<output-file> | --to=<output-format>) [options] : Utilitaire permettant d'orienté un maillage

Usage: translate (<input-file> | --from=<input-format>) (<output-file> | --to=<output-format>) [options] : Utilitaire permettant de translater un maillage

Usage: manip (<input-file> | --from=<input-format>) (<output-file> | --to=<output-format>) [options] : Utilitaire permettant de manipuler les attributs d'un maillage (couleur, normals, etc...)
```