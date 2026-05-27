#ifdef _WIN32
#undef NOUSER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#pragma comment(lib, "advapi32.lib")
#else
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif

#include "Modele/M_LecteurPhysique.h"
#include "../libs/JSON/json.hpp"

#include <filesystem>
#include <thread>
#include <chrono>
#include <cstring>
#include <fstream>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

M_LecteurPhysique::M_LecteurPhysique()
{
    // Arguments pour configurer LibVLC en mode "extraction mémoire" (vmem) sans conflits
    const char* const vlc_args[] = {
        "--avcodec-hw=none",       // 1. Désactive l'accélération matérielle (incompatible avec les callbacks RAM)
        "--no-avcodec-dr",         // 2. Désactive le Direct Rendering FFmpeg pour éviter les échecs de buffer
        "--no-video-title-show",   // 3. Optionnel : Évite l'affichage du titre du fichier à l'écran
        "--quiet"                  // 4. Optionnel : Demande à VLC d'être plus discret sur les logs mineurs
    };
    int argc = sizeof(vlc_args) / sizeof(vlc_args[0]);

    // Initialisation du coeur natif libVLC avec nos arguments de contournement
    instanceVLC = libvlc_new(argc, vlc_args);
    lecteurVLC = libvlc_media_player_new(instanceVLC);
}
M_LecteurPhysique::~M_LecteurPhysique() {
    if (lecteurVLC) { libvlc_media_player_release(lecteurVLC); }
    if (instanceVLC) { libvlc_release(instanceVLC); }
}

float M_LecteurPhysique::getProgressionActuelle() const {
    libvlc_time_t t = libvlc_media_player_get_time(lecteurVLC);
    return (t != -1) ? static_cast<float>(t) / 1000.0f : 0.0f;
}

void M_LecteurPhysique::lireVideo(const string &cheminVideo) {
    if (!filesystem::exists(cheminVideo)) return;

    libvlc_media_t *media = libvlc_media_new_path(instanceVLC, cheminVideo.c_str());
    if (!media) return;

    libvlc_media_player_set_media(lecteurVLC, media);
    libvlc_media_release(media);

    // Configuration des callbacks memoire pour l extraction des images
    libvlc_video_set_callbacks(lecteurVLC, cb_verrouiller, cb_deverrouiller, nullptr, this);
    libvlc_video_set_format_callbacks(lecteurVLC, libvlc_video_format_cb(cb_configurerVideo), nullptr);

    demarrer();
    this_thread::sleep_for(chrono::milliseconds(200));
    pause();

    const libvlc_time_t len = libvlc_media_player_get_length(lecteurVLC);
    dureeTotale = (len != -1) ? static_cast<float>(len) / 1000.0f : 0.0f;
}

bool M_LecteurPhysique::recupererFrameVideo(void *&outPixels, unsigned int &outLargeur, unsigned int &outHauteur,
                                            bool &outRedimensionnement) {
    // Synchronisation par verrou pour empecher l écriture simultanée par LibVLC
    lock_guard lock(mutexImage);
    if (textureDoitEtreRedimensionnee || framePrete) {
        outPixels = pixelsVideo.data();
        outLargeur = largeurVideo;
        outHauteur = hauteurVideo;
        outRedimensionnement = textureDoitEtreRedimensionnee;
        textureDoitEtreRedimensionnee = false;
        framePrete = false;
        return true;
    }
    return false;
}

void *M_LecteurPhysique::cb_verrouiller(void *opaque, void **plans) {
    auto *mod = static_cast<M_LecteurPhysique *>(opaque);
    mod->mutexImage.lock();
    
    if (mod->pixelsVideo.empty()) {
        mod->pixelsVideo.resize(64 * 64 * 4, 0); // Allocation minimale temporaire (ex: 64x64 en RGBA)
    }

    *plans = mod->pixelsVideo.data();
    return nullptr;
}

void M_LecteurPhysique::cb_deverrouiller(void *opaque, void *, void *const*) {
    auto *mod = static_cast<M_LecteurPhysique *>(opaque);
    mod->framePrete = true;
    mod->mutexImage.unlock();
}

unsigned M_LecteurPhysique::cb_configurerVideo(void **opaque, char *chrominance, const unsigned *largeur,
                                               const unsigned *hauteur, unsigned *pas, unsigned *lignes) {
    auto *mod = static_cast<M_LecteurPhysique *>(*opaque);
    lock_guard lock(mod->mutexImage);
    mod->largeurVideo = *largeur;
    mod->hauteurVideo = *hauteur;
    mod->pixelsVideo.resize(*largeur * *hauteur * 4);
    memcpy(chrominance, "RGBA", 4);
    *pas = *largeur * 4;
    *lignes = *hauteur;
    mod->textureDoitEtreRedimensionnee = true;
    return 1;
}

void M_LecteurPhysique::collecterInfosLocales() {
    collecterIp();
    collecterMac();
    collecterOs();
    collecterTailleEcran();
}

void M_LecteurPhysique::collecterIp() {
#ifdef _WIN32
    ULONG bufferSize = 0;
    if (GetAdaptersInfo(nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        vector<char> buffer(bufferSize);
        PIP_ADAPTER_INFO adapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.data());

        if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
            while (adapterInfo) {
                if (adapterInfo->Type != MIB_IF_TYPE_LOOPBACK && string(adapterInfo->IpAddressList.IpAddress.String) != "0.0.0.0") {
                    m_ip = adapterInfo->IpAddressList.IpAddress.String;
                    return;
                }
                adapterInfo = adapterInfo->Next;
            }
        }
    }
    if (m_ip.empty()) m_ip = "127.0.0.1";
#else
    ifaddrs *interfaces = nullptr;
    if (getifaddrs(&interfaces) != 0) {
        m_ip = "0.0.0.0";
        return;
    }
    for (ifaddrs *it = interfaces; it != nullptr; it = it->ifa_next) {
        if (it->ifa_addr == nullptr) continue;
        string nomIface(it->ifa_name);
        if (nomIface == "lo") continue;
        if (it->ifa_addr->sa_family == AF_INET) {
            char ipStr[INET_ADDRSTRLEN];
            auto *addr = reinterpret_cast<sockaddr_in *>(it->ifa_addr);
            inet_ntop(AF_INET, &addr->sin_addr, ipStr, sizeof(ipStr));
            m_ip = string(ipStr);
            break;
        }
    }
    freeifaddrs(interfaces);
    if (m_ip.empty()) m_ip = "0.0.0.0";
#endif
}

void M_LecteurPhysique::collecterMac() {
#ifdef _WIN32
    ULONG bufferSize = 0;
    if (GetAdaptersInfo(nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        vector<char> buffer(bufferSize);
        PIP_ADAPTER_INFO adapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.data());

        if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
            while (adapterInfo) {
                if (adapterInfo->Type != MIB_IF_TYPE_LOOPBACK && string(adapterInfo->IpAddressList.IpAddress.String) != "0.0.0.0") {
                    char macStr[18];
                    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                             adapterInfo->Address[0], adapterInfo->Address[1], adapterInfo->Address[2],
                             adapterInfo->Address[3], adapterInfo->Address[4], adapterInfo->Address[5]);
                    m_mac = macStr;
                    return;
                }
                adapterInfo = adapterInfo->Next;
            }
        }
    }
    if (m_mac.empty()) m_mac = "00:00:00:00:00:00";
#else
    auto lireMac = [](const string &iface) -> string {
        ifstream f("/sys/class/net/" + iface + "/address");
        string mac;
        if (f.is_open()) getline(f, mac);
        return mac;
    };

    m_mac = lireMac("eth0");
    if (m_mac.empty()) m_mac = lireMac("wlan0");
    if (m_mac.empty()) m_mac = lireMac("enp3s0");
    if (m_mac.empty()) m_mac = "00:00:00:00:00:00";
#endif
}

void M_LecteurPhysique::collecterOs() {
#ifdef _WIN32
    HKEY hKey;
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char osName[256] = {0};
        DWORD dataSize = sizeof(osName);
        if (RegQueryValueExA(hKey, "ProductName", nullptr, nullptr, reinterpret_cast<LPBYTE>(osName), &dataSize) == ERROR_SUCCESS) {
            m_os = string(osName);
        }
        RegCloseKey(hKey);
    }
    if (m_os.empty()) m_os = "Windows";
#else
    ifstream f("/etc/os-release");
    if (!f.is_open()) {
        m_os = "Linux";
        return;
    }
    string ligne;
    while (getline(f, ligne)) {
        if (ligne.rfind("PRETTY_NAME=", 0) == 0) {
            m_os = ligne.substr(12);
            m_os.erase(remove(m_os.begin(), m_os.end(), '"'), m_os.end());
            return;
        }
    }
    m_os = "Linux";
#endif
}

void M_LecteurPhysique::collecterTailleEcran() {
#ifdef _WIN32
    m_largeurEcran = GetSystemMetrics(SM_CXSCREEN);
    m_hauteurEcran = GetSystemMetrics(SM_CYSCREEN);
#else
    m_largeurEcran = 0;
    m_hauteurEcran = 0;
#endif
}

void M_LecteurPhysique::versJson(const string& cheminFichier) const {
    cout << "[DEBUG] [Lecteur Physique] Exportation des donnees en JSON..." << endl;

    // 1. Initialisation de l'objet JSON fourni par la bibliothèque
    nlohmann::json objetJson;

    // 2. Remplissage manuel des champs
    objetJson["ip"] = m_ip;
    objetJson["mac"] = m_mac;
    objetJson["os"] = m_os;
    objetJson["largeurEcran"] = m_largeurEcran;
    objetJson["hauteurEcran"] = m_hauteurEcran;

    // 3. Écriture sécurisée dans le fichier
    ofstream fichier(cheminFichier);
    if (fichier) {
        fichier << objetJson.dump(4);
        fichier.close();
        cout << "[DEBUG] [Lecteur Physique] Fichier JSON cree avec succes : " << cheminFichier << endl;
    } else {
        cerr << "[DEBUG] [Lecteur Physique] [ERROR] Impossible d'ouvrir le fichier pour ecriture : " << cheminFichier << endl;
    }
}