# Surgery Viewer

Viewer écrit en C++, utilisant CGAL5, Qt5 et LibQGLViewer pour optimiser la visualisation de maillages surfaciques multi-couches provenant de dissections chirurgicales.

## Dépendances

Les templates C++ sont fortement utilisés par la bibliothèque CGAL. Cela lui permet d'être flexible et générique cependant la durée de compilations s'en retrouve fortement affectée.

Pour accélérer la durée de compilation et donc le développement ce projet applique des optimisations de compilations utilisable seulement avec une version de CMake capable de pré-compiler les fichiers d'en-têtes C++.

- CMake 3.16+

Les traitements géométriques necessites les bibliothèques suivantes

- CGAL
- Boost
- GMP
- MPFR
- Eigen

Le viewer utilise quant à lui Qt5 pour l'affichage fênétré et Assimp pour l'importation de maillages

- Qt5
- Assimp

Sur les systèmes du type Debian / Ubuntu les dépendances peuvent être installer avec la commande ci-dessous.

```sh
# Si tout vas bien
sudo apt-get install libcgal-qt5-dev libassimp-dev
```

```sh
# Si tout vas mal
sudo apt-get install libcgal-qt5-dev 'libqt5*-dev' libboost-dev libgmp-dev libmpfr-dev libeigen3-dev libassimp-dev
```

## Compilation

```sh
# 1. clone surgery-viewer repository

git clone https://github.com/todorico/surgery-viewer.git

# 2. create and move to build directory

mkdir surgery-viewer/build 
cd surgery-viewer/build

# 3. build project

cmake ..
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