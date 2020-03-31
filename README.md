# Surgery Viewer

Visualiseur optimisé pour la visualisation de maillages provenant de dissections chirurgicales (Écrit en C++ et basé sur CGAL5 et Qt5).

# Dépendances

Sur mon système la commande suivante suffit à satisfaire les dépendances de CGAL

```
sudo apt install libboost-dev 'libqt5*-dev' libgmp-dev libmpfr-dev
```

# Compilation

```bash
git clone https://github.com/todorico/surgery-viewer.git
cd surgery-viewer
mkdir build
cd build
cmake ..
```

**Remarque** : CGAL est très long à compiler, si vous voulez accélérer la compilation, utilisez une version de CMake capable de pré-compiler les headers (CMake 3.16.4+)

