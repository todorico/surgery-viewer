# Surgery Viewer

Viewer écrit en C++, utilisant Assimp, CGAL5, Qt5, pour optimiser la visualisation et le traitement de maillages surfaciques multi-couches provenant de dissections chirurgicales.

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

La majorité des dépendances sont installés durant la configuration du projet avec CMake cependant CGAL et Qt5 doivent être installer manuellement par l'utilisateur.

## Dépandances manuelles (Linux)

```sh
# Installation des bibliothèques et programmes de build
sudo apt-get install libcgal-qt5-dev build-essential cmake python3-pip  # avec git et le compilateur de votre choix (gcc, clang, ...)
# Installation de conan un gestionnaire de paquets c++
sudo pip3 install conan
```

## Build (Linux)

```sh
# 1. clonez le repertoire surgery-viewer
git clone https://github.com/todorico/surgery-viewer.git

# 2. creez puis déplacez vous dans le repertoire build
mkdir surgery-viewer/build && cd surgery-viewer/build

# 3. compilez le projet
cmake .. -DCMAKE_BUILD_TYPE=Release && make
```

## Dépandances manuelles (Windows)

Installez les version 64bit des bibliothèques et pour Qt installer la version compatible avec votre compilateur (MinGW ici)

- Installeur CGAL-5.0.2 sur Github : [https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-5.0.2/CGAL-5.0.2-Setup.exe](https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-5.0.2/CGAL-5.0.2-Setup.exe)
- Installeur Qt5 open source offline : [http://download.qt.io/official_releases/qt/5.12/5.12.9/qt-opensource-windows-x86-5.12.9.exe](http://download.qt.io/official_releases/qt/5.12/5.12.9/qt-opensource-windows-x86-5.12.9.exe)

Pour eviter d'installer les programmes suivants sur windows je recommande l'utilisation d'un gestionnaire de package (chocolatey). Autrement les programmes devronts être installés manuellement.

1. Ouvrir PowerShell.exe en tant qu'administrateur
2. Executez la commande suivante pour installer chocolatey

```sh
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://chocolatey.org/install.ps1'))
```

3. Fermez puis ré-ouvrez le terminal en tant qu'administrateur

```sh
choco install -y git
choco install -y cmake --installargs 'ADD_CMAKE_TO_PATH=System'
choco install -y mingw
choco install -y make
choco install -y conan
```

Tout les programmes necessaires sont désormais installés.

## Build (Windows)

Ouvrez PowerShell (en tant qu'utilisateur) et déplacez vous dans le dossier ou vous voulez installer surgery-viewer

```sh
# 1. clonez le repertoire surgery-viewer
git clone https://github.com/todorico/surgery-viewer.git

# 2. creez puis déplacez vous dans le repertoire build
mkdir surgery-viewer/build
cd surgery-viewer/build

# ATTENTION! : Sur windows cmake à du mal à reperer ou les bibliothèques (CGAL et Qt5) se trouvent.
#              Ont peux donc spécifier des chemin manuellement comme avec la commande ci-dessous pour résoudre ce problème.
#              Pour Qt5 il faut faire attention a utiliser la bibliothèque adapté au compilateur utilisé (MinGW dans notre cas).

# 3. compilez le projet
cmake .. -G"MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCGAL_DIR="C:/dev/CGAL-5.0.2" -DQt5_DIR="C:/dev/Qt/5.15.0/mingw81_64/lib/cmake/Qt5"
make
```
