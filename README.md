# Surgery Viewercmake -DCMAKE_BUILD_TYPE=DebugFast ..


Visualisateur optimisé pour la visualisation de maillages provenant de dissections chirurgicales (Ecrit en C++ et basé sur CGAL5 et Qt5).

# Compilation

```sh
# 1. clone surgery-viewer repository

git clone https://github.com/todorico/surgery-viewer.git

# 2. build included CGAL library

cd surgery-viewer/CGAL-5.0.1
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo -DCGAL_HEADER_ONLY=OFF -DWITH_Eigen3=ON ..
make

# 3. build surgery-viewer programs

cd ../../ # Root of surgery-viewer
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=DebugFast ..
make
```

# Programmes

```
Usage: view (<input-file> | --from=<format>) : Utilitaire permettant de visualiser des objets géométriques.

Usage: prop (<input-file> | --from=<format>) : Utilitaire permettant d'afficher et modifier les propriétés d'un objet géométrique.

Usage: match <threshold> <input-file>... : Utilitaire permettant de produire un super-maillage contenant seulement les parties suffisament différentes provenants de plusieurs maillage d'entrés.

Usage: project <input-file1> <input-file2> : Utilitaire permettant de produire un maillage étant la projection du 1er maillage sur le 2eme.

Usage: rotate (<input-file> | --from=<input-format>) (<output-file> | --to=<output-format>) [options] : Utilitaire permettant d'orienté un maillage

Usage: translate (<input-file> | --from=<input-format>) (<output-file> | --to=<output-format>) [options] : Utilitaire permettant de translater un maillage

Usage: manip (<input-file> | --from=<input-format>) (<output-file> | --to=<output-format>) [options] : Utilitaire permettant de manipuler les attributs d'un maillage (couleur, normals, etc...)
```