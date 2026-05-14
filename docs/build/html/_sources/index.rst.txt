========================================
Documentation de AffichageSynchrone
========================================

.. image:: https://img.shields.io/badge/C%2B%2B-20-blue.svg
   :target: https://isocpp.org/

Bienvenue sur la documentation technique du projet **AffichageSynchrone**.

Ce logiciel permet de piloter un mur d'écrans de manière synchronisée via un protocole UDP, 
incluant la génération de vidéos complexes et le déploiement par TFTP.

Description du Projet
=====================

Le système repose sur une architecture **MVC** (Modèle-Vue-Contrôleur) :

* **Vue** : Interface graphique développée avec Raylib.
* **Contrôleur** : Gestionnaire de l'état de lecture et des threads de fond.
* **Modèle** : Moteurs de lecture (VLC), de réseau (UDP/TFTP) et de traitement (FFmpeg/FFTW).

Guide de Navigation
===================

.. toctree::
   :maxdepth: 2
   :caption: Table des matières:

   api
   installation