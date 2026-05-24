# AffichageSynchrone (Master)

## 📋 Présentation du Projet

Ce dépôt contient le code source de l'application **Master** développée dans le cadre du projet de BTS CIEL IR pour l'association *La Hora del Tango*. 

Le système complet permet de synchroniser l'affichage de vidéos de danse de tango filmées sous différents angles sur plusieurs écrans distants.

## 🏗️ Architecture du projet

### Diagramme de cas d'utilisation

```mermaid
flowchart LR
%% Acteur
    Operateur[<< actor >><br>Opérateur]

%% Système Master
    subgraph Master [MASTER Lecteur Physique IP : 127.0.0.1]
        direction TB
        UC5([UC5 : Enregistrer configuration réseau])
        UC3(["<b>UC3 : Visualiser lecteurs physiques<br/><font color='#888' size='-1'>points d'extension</font><br/><font size='-1'>Enregistrer configuration reseau<br/>Rechercher lecteurs physiques</font>"])
        UC4([UC4 : Rechercher lecteurs physiques])
        UC6([UC6 : Générer videos complexes])
        UC2([UC2 : Preparer session lecture videos complexes])
        UCT2([UCT2 : Uploader videos complexes])
        UC1([UC1 : Gérer Lecture vidéos complexes])
    end

%% Bases de données (Composants)
    BddConfig[<size='-1'><< component >></font><br/>BDD config reseau]
    BddMusiques[<size='-1'><< component >></font><br/>Bdd musiques]
    BddVideos[<size='-1'><< component >></font><br/>Bdd videos]

%% Système Lecteur Physique distant
    subgraph Lecteur [Lecteur physique IP : X.X.X.X]
        direction TB
        UTC1([UTC1 : S'identifié sur le réseau])
        UCT3([UCT3 : Recevoir vidéo complexe])
        UC7([UC7 : Lire video complexe])
    end

%% Relations Acteur -> Use Cases
    Operateur --- UC1
    Operateur --- UC2

%% Relations Include / Extend dans le Master
    UC2 -. "<< include >>" .-> UC3
    UC2 -. "<< include >>" .-> UC6
    UC2 -. "<< include >>" .-> UCT2
    UC1 -. "<< include >>" .-> UC2
    UC5 -. "<< extend >>" .-> UC3
    UC4 -. "<< extend >>" .-> UC3

%% Relations Master <-> Bases de données
    UC3 --- BddConfig
    UC6 --- BddMusiques
    UC6 --- BddVideos

%% Relations Master <-> Lecteur Physique
    UC4 --- UTC1
    UCT2 --- UCT3
    UC1 --- UC7
```

### Diagramme de classes

```mermaid
classDiagram
    direction LR

%% --- MODÈLES ---
    class M_ProtocoleReseau {
        +enum Expediteur
        +enum TypeCommande
        +enum Action
        +struct PaquetControle
        +struct LecteurConfig
    }

    class M_BDD {
        -pBaseDonne
        +enregistrerDonnees()
        +actualiserDonnees()
        +recupererDonnees()
    }

    class M_VideoComplexe {
        +genererVideoComplexe()
        -calculerDecalages()
        -construireCommandeFFmpeg()
    }

    class M_SessionLecture {
        -M_VideoComplexe
        -idLecteurs
        -ipLecteurs
        +preparerSessionLecture()
        +genererVideoComplexe()
        +uploaderVideoComplexe()
        +rechercherLecteurs()
    }

    class M_DecouverteReseau {
        -m_expediteur
        -m_receveur
        +lancerDecouverte()
        +getReponsesBrutes()
        +afficherResultats()
    }

    class M_LecteurPhysique {
        -instanceVLC
        -m_ip
        -m_os
        -m_memoireMo
        +lireVideo()
        +recupererFrameVideo()
        +collecterInfosLocales()
        +versJson()
        +play()
        +pause()
        +stop()
    }

    class M_JsonUtil {
        +parser()
        +construire()
    }

    class M_TFTP_W {
        -TFTP_PORT
        -BLOCK_SIZE
        -MAX_RETRIES
        +envoyer()
        +recevoirFichierPousse()
    }

    class M_ExpediteurUDP {
        -descripteurSocket
        -groupeMulticast
        +envoyer()
        +transmettreCommande()
    }

    class M_ReceveurUDP {
        -descripteurSocket
        -groupeMulticast
        +recevoir()
        +recevoirAvecTimeout()
        +envoyerReponse()
    }

    class M_ClientTFTP {
        +recevoirFichier()
    }

%% --- CONTRÔLEURS ---
    class C_LecteurPhysiqueLocal {
        -M_LecteurPhysique
        -M_SessionLecture
        -M_ExpediteurUDP
        -M_ReceveurUDP
        +initialiserSession()
        +basculerPlayPause()
        +lancerRechercheLecteurs()
        +demarrerEcouteMulticast()
        +mettreAJour()
    }

    class C_LecteurPhysiqueDistant {
        -M_LecteurPhysique
        -M_ClientTFTP
        -M_ReceveurUDP
        +recevoirVideosComplexes()
        +recevoirCommande()
    }

%% --- VUES ---
    class V_Master {
        -C_LecteurPhysiqueLocal
        +executer()
        -chargerListeVideos()
        -chargerListeLecteurs()
        -dessinerZoneVideo()
        -dessinerOverlayChargement()
    }

    class V_LecteurPhysiqueDistant {
        -C_LecteurPhysiqueDistant
        +afficherVideo()
    }

%% --- RELATIONS ET LIENS ---

%% Dépendances Protocoles (Nouvelles)
    M_ExpediteurUDP ..> M_ProtocoleReseau
    M_ReceveurUDP ..> M_ProtocoleReseau

%% Master (MVC)
    C_LecteurPhysiqueLocal <--* V_Master
    C_LecteurPhysiqueLocal *--> M_SessionLecture
    C_LecteurPhysiqueLocal *--> M_LecteurPhysique
    M_ExpediteurUDP <--* C_LecteurPhysiqueLocal
    M_ReceveurUDP <--* C_LecteurPhysiqueLocal

%% Session et Découverte
    M_SessionLecture *--> M_VideoComplexe
    M_SessionLecture ..> M_DecouverteReseau
    M_SessionLecture ..> M_TFTP_W
    M_DecouverteReseau *--> M_ExpediteurUDP
    M_DecouverteReseau *--> M_ReceveurUDP

%% Utilitaires et BDD
    M_SessionLecture ..> M_JsonUtil
    M_LecteurPhysique ..> M_JsonUtil
    M_SessionLecture ..> M_BDD
    M_DecouverteReseau ..> M_BDD

%% Client Distant (MVC)
    V_LecteurPhysiqueDistant *--> C_LecteurPhysiqueDistant
    M_ClientTFTP <--* C_LecteurPhysiqueDistant
    C_LecteurPhysiqueDistant *--> M_ReceveurUDP
    C_LecteurPhysiqueDistant *--> M_LecteurPhysique
```

Le projet respecte l'architecture **MVC (Modèle-Vue-Contrôleur)**.

## 🛠️ Technologies et dépendances

Développé en **C++23**, le programme utilise des threads et des variables atomiques pour gérer les calculs lourds sans jamais ralentir ou bloquer l'affichage de l'interface graphique.

* **Interface graphique (IHM) :** [Raylib](https://www.raylib.com/) & [Raygui](https://github.com/raysan5/raygui)
* **Moteur multimédia :** [LibVLC](https://www.videolan.org/vlc/libvlc.html)
* **Traitement du signal :** [FFTW3](https://www.fftw.org/)
* **Traitement vidéo et audio :** [FFmpeg](https://ffmpeg.org/)

## 📂 Structure du projet

```text
├── include/
│   ├── Controleur/
│   │   └── C_LecteurPhysiqueLocal.h
│   ├── Modele/
│   │   ├── M_DecouverteReseau.h
│   │   ├── M_ExpediteurUDP.h
│   │   ├── M_JsonUtil.h
│   │   ├── M_LecteurPhysique.h
│   │   ├── M_ProtocoleReseau.h
│   │   ├── M_ReceveurUDP.h
│   │   ├── M_SessionLecture.h
│   │   ├── M_TFTP_W.h
│   │   └── M_VideoComplexe.h
│   └── Vue/
│       └── V_Master.h
├── libs/
│   ├── FFTW
│   ├── JSON
│   ├── LibVLC
│   └── raygui
├── src/
│   ├── Controleur/
│   │   └── C_LecteurPhysiqueLocal.cpp
│   ├── Modele/
│   │   ├── M_DecouverteReseau.cpp
│   │   ├── M_ExpediteurUDP.cpp
│   │   ├── M_JsonUtil.cpp
│   │   ├── M_LecteurPhysique.cpp
│   │   ├── M_ReceveurUDP.cpp
│   │   ├── M_SessionLecture.cpp
│   │   ├── M_TFTP_W.cpp
│   │   └── M_VideoComplexe.cpp
│   └── Vue/
│       └── V_Master.cpp
├── main.cpp
└── CMakeLists.txt
```

## 🚀 Compilation & Installation

### Prérequis (Exemple sous Windows avec MinGW-w64)

Assurez-vous que les bibliothèques d'en-tête et les fichiers binaires (`.a` / `.lib` / `.dll`) de **Raylib**, **raygui**, **JSON**, **LibVLC** et **FFTW3** sont correctement installés sur votre système et référencés dans vos variables d'environnement ou le fichier `CMakeLists.txt`.

### Étapes de compilation

1. Créez un dossier de génération :

```bash
mkdir build && cd build
```

2. Générez les fichiers de configuration de build avec CMake :

```bash
cmake ..
```

3. Compilez le projet :

```bash
cmake --build . --config Release
```

> ⚠️ **Note importante :** Veillez à copier les DLL requises (`libvlc.dll`, `libvlccore.dll`, `libfftw3-3.dll`, etc.) ainsi que les exécutables FFmpeg (`ffmpeg.exe`) dans le répertoire de sortie de votre exécutable final pour assurer le bon fonctionnement du programme.

## 📊 Configuration de test

```cpp

const string IP_MULTICAST = "224.0.0.1";
constexpr int PORT_COMMANDES = 54321;
constexpr int PORT_DECOUVERTE = 5000;
constexpr int PORT_REPONSE = 5001;

// Liste statique préconfigurée pour le recettage initial
const vector<LecteurConfig> configurationReseau = {
    {0, "127.0.0.1", 2},       // Master local (ID 0)
    {1, "127.0.0.1", 2},       // Instance de test locale Slave (ID 1)
    {2, "172.31.71.104", 2}    // Lecteur physique distant cible (ID 2)
};

const string DOSSIER_VIDEOS = "videos";
const string CHEMIN_VIDEO_MASTER = "videosComplexes/VideoComplexe_0.mp4";

V_Master master(IP_MULTICAST, PORT_COMMANDES, PORT_DECOUVERTE, PORT_REPONSE, configurationReseau, DOSSIER_VIDEOS, CHEMIN_VIDEO_MASTER);

master.executer();

```

## 👥 Développeurs

* **Étudiant 1 :** Responsable de l'application **Master** (Interface graphique raygui, gestion des sessions, algorithme de corrélation croisée FFTW3/FFmpeg et intégration globale).

* **Étudiant 2 :** Responsable du protocole réseau, de la couche de communication UDP multicast et des transferts TFTP binaires.

* **Étudiant 3 :** Responsable du développement de l'application **Slave**.
