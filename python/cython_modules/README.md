# Cython Project

L'idée est d'optmiser la vitesse des parties limitantes du programme python pour gagner en profondeur dans le minmax.

## Principes

Un fihcier source python est un fichier .pyx. Tout fichier python est un fichier pyx valide. Mais dans un fichier pyx vous pouvez également ajouter des indications sur le type des objets cela permet d'accélerer l'éxécution.
Ensuite il faut demander à cython de "compiler" le pyx (pas sûr que cela soit une compilation). Il génère alors un module qui s'importe dans un script python classique de manière tranparente. 

## Installation

via pip : ```pip install cython```
plus d'info ici : http://docs.cython.org/src/quickstart/install.html

## Utilisation

pour compiler les changements il faut entrer ``python setup.py build_ext --inplace`` setup.py est un fichier contenant les infos sur les fichier .pyx à conpiler

