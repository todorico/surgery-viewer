# Surgery Viewer

Viewer écrit en C++, utilisant Assimp, CGAL5, Qt5, pour optimiser la visualisation et le traitement de maillages surfaciques multi-couches provenant de dissections chirurgicales. Le code de ce répertoire génère plusieurs binaires : match, prop et view (le programme de test est utilisé seulement pour le développement des applications).

## Table des matières

<!-- MarkdownTOC -->

- [Programmes](#programmes)
  - [Match](#match)
    - [Exécution](#ex%C3%A9cution)
    - [Résultat](#r%C3%A9sultat)
  - [Prop](#prop)
    - [Exemples](#exemples)
    - [Exécution](#ex%C3%A9cution-1)
  - [View](#view)
    - [Exécution](#ex%C3%A9cution-2)
    - [Fonctionnalités](#fonctionnalit%C3%A9s)
- [Développement](#d%C3%A9veloppement)
    - [Structure du répertoire](#structure-du-r%C3%A9pertoire)
  - [Dépendances](#d%C3%A9pendances)
  - [Linux \(debian\)](#linux-debian)
    - [Installation des dépendances](#installation-des-d%C3%A9pendances)
    - [Build](#build)
  - [Windows](#windows)
    - [Installation des dépendances](#installation-des-d%C3%A9pendances-1)
    - [Build \(Windows\)](#build-windows)

<!-- /MarkdownTOC -->

## Programmes

### Match

Ce programme crée un découpage de maillages en détectant les parties similaire entre plusieurs maillages. Le détection s'effectue en fonction d'un seuil sur la distance entre deux maillages.

```sh
Create a new mesh by matching parts of multiple similars meshes.

    Usage: match [options] <threshold> <input-files>...

    Options:
      -h, --help                          Show this screen
      --version                           Show version
```

#### Exécution

```sh
# En supposant que l'utilisateur se trouve dans surgery-viewer/build
./bin/match 0.5 maillage1.obj maillage2.obj
```
#### Résultat

Le résultat de l’exécution du programme est l'ensemble de maillages suivant :

- 

### Prop

Ce programme affiche les propriétés d'une maillage (nombre de sommets, arêtes, faces, coordonnées de textures, etc...).

```sh
List geometrical properties from mesh.

    Usage: prop <input-file>

    Options:
      -h, --help       Show this screen.
      --version        Show version.
```

Les propriétés sont notés sous la forme element:property_name

#### Exemples

- v:point → information de position sur les sommets
- v:texcoord → information de coordonnée de texture sur les sommets
- f:connectivity → information de connectivité sur les faces
- e:removed → information d'arêtes enlevés
- h:connectivity → information de connectivité sur les demi-arêtes

#### Exécution

```sh
# En supposant que l'utilisateur se trouve dans surgery-viewer/build
./bin/prop mon_maillage.ply
```
### View

Ce programme ouvre un fenêtre de visualisation de maillages.

```sh
3D viewer for geometrical file formats (OFF, PLY, OBJ, ...).

    Usage: view [options] <input-files>...

    Options:
      -c, --colorize  Colorize geometrical objects by files.
      -h, --help      Show this screen.
      --version       Show version.
```

#### Exécution

```sh
# En supposant que l'utilisateur se trouve dans surgery-viewer/build
./bin/view maillage1.obj maillage2.ply
# L'option -c applique un code couleur sur les maillages (M1:rouge, M2:vert, M3:bleu, ..., Mn:random)
./bin/view -c maillage1.obj maillage2.ply
```

#### Fonctionnalités

- L'utilisateur peut cacher/afficher
  - L'aide du programme avec la touche H
  - Les maillages d'entrés
    - de 0 à 5 en utilisant les touches respectives : W, X, C, V, B, N.
    - de 6 à 11 en utilisant les touches respectives : Q, S, D, F, G, H.
  - Les axes 3D avec la touche A
  - Les arêtes des maillages avec la touche E
  - Les points des maillages avec la touche P
- L'utilisateur peut tourner autour du maillage avec un clic gauche et un déplacement de la souris
- L'utilisateur peut déplacer la caméra avec un clic droit et un déplacement de la souris

## Développement

#### Structure du répertoire

Le répertoire est composé des éléments suivants : 

- **cmake** : contient les modules CMake utilisés pour la construction des binaires
- **data** : contient les maillages qui servent de données d'entrée à nos programmes
- **doc** : contient la documentation rédigée durant le stage
- **src** : contient les sources des programmes
  - **instance** : contient les déclaration des types utilisés dans nos programmes (permet de simplifier le code en éviter de ré-instancier les templates dans chacun de nos programmes)
  - **mesh** : contient les fonctionnalités développer pour les maillages
  - **pch** : contient les headers à pré-compiler avec cmake (cela permet d’éviter de recompiler les headers et fait gagner un temps non négligeable sur la compilation durant le développement des programmes)
  - **shader** : contient les shaders du viewer (mesh/viewer.cpp). Le viewer utilise un fragment shader différent selon le mode d'affichage.
  - **match.cpp, prop.cpp, test.cpp, view.cpp, \*.cpp** : ce sont les sources contenant les fonctions main qui généreront nos programmes (le cmake considèrent que tout les fichiers \*.cpp qui sont directement dans src/ sont des programmes à générer).

### Dépendances

Les templates C++ sont fortement utilisés par la bibliothèque CGAL. Cela lui permet d'être flexible et générique cependant la durée de compilations s'en retrouve fortement affectée.

Pour accélérer la durée de compilation et donc le développement ce projet applique des optimisations de compilations utilisable seulement avec une version de CMake capable de pré-compiler les fichiers d'en-têtes C++.

- CMake 3.16+

Les traitements géométriques nécessites les bibliothèques suivantes

- CGAL
- Boost
- GMP
- MPFR
- Eigen

Le viewer utilise quant à lui Qt5 pour l'affichage fenêtré et Assimp pour l'importation de maillages

- Qt5
- Assimp

La majorité des dépendances sont installés durant la configuration du projet avec CMake cependant CGAL et Qt5 doivent être installer manuellement par l'utilisateur.

### Linux (debian)

#### Installation des dépendances

```sh
# Installation des bibliothèques et programmes de build
sudo apt-get install libcgal-qt5-dev build-essential cmake python3-pip  # avec git et le compilateur de votre choix (gcc, clang, ...)
# Installation de conan un gestionnaire de paquets c++
sudo pip3 install conan
```

#### Build

```sh
# 1. clonez le repertoire surgery-viewer
git clone https://github.com/todorico/surgery-viewer.git

# 2. creez puis déplacez vous dans le repertoire build
mkdir surgery-viewer/build && cd surgery-viewer/build

# 3. compilez le projet
cmake .. -DCMAKE_BUILD_TYPE=Release && make
```

### Windows

#### Installation des dépendances

Installez les version 64 bit des bibliothèques et pour Qt installer la version compatible avec votre compilateur (MinGW ici)

- Installeur CGAL-5.0.2 sur Github : [https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-5.0.2/CGAL-5.0.2-Setup.exe](https://github.com/CGAL/cgal/releases/download/releases%2FCGAL-5.0.2/CGAL-5.0.2-Setup.exe)
- Installeur Qt5-5.15 sur qt.io : [https://www.qt.io/download-open-source](https://www.qt.io/download-open-source)

Pour éviter d'installer les programmes suivants sur Windows je recommande l'utilisation d'un gestionnaire de package (Chocolatey). Autrement les programmes devrons être installés manuellement.

1. Ouvrir PowerShell.exe en tant qu'administrateur
2. Exécutez la commande suivante pour installer Chocolatey

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

Tous les programmes nécessaires sont désormais installés.

#### Build (Windows)

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

Une fois les binaires générés les dll associés à Qt doivent être déployés dans le dossier des binaires. Pour faire cela de manière automatique utilisez le programme windeployqt.exe fournis avec la version de Qt5 utilisée.
En reprenant notre version utilisée le programme devrait avoir le chemin suivant "C:/dev/Qt/5.15.0/mingw81_64/bin/windeployqt.exe".

```sh
# windeployqt.exe analyse les binaires et copie dans le même dossier les Dlls nécessaires. le programme prend un binaire en paramètre comme ceci :

windeployqt.exe bin/match.exe 
```
