# Surgery Viewer

Visualiseur optimisé pour la visualisation de maillages provenant de dissections chirurgicales (Écrit en C++ et basé sur CGAL5 et Qt5).

# Dépendances

Sur mon système la commande suivante suffit à satisfaire les dépendances de CGAL

```bash
sudo apt install libboost-dev 'libqt5*-dev' libgmp-dev libmpfr-dev
```

# Compilation

```bash
git clone https://github.com/todorico/surgery-viewer.git
cd surgery-viewer
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=DebugFast # Compilation en mode debug accelerée
make -j
```

**Remarque** : CGAL est très long à compiler, pour accélérer la compilation, ce projet utilise une version de CMake capable de pré-compiler les headers avec la fonction `target_precompile_headers` (CMake 3.16.4+)

