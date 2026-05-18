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
#include <unistd.h>
#endif

#include "Modele/M_LecteurPhysique.h"
#include "Modele/M_JsonUtil.h"

#include <filesystem>
#include <thread>
#include <chrono>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>

using namespace std;

M_LecteurPhysique::M_LecteurPhysique()
    : m_tailleEcranPouces(0), m_memoireMo(0), m_nbMaxVideos(1) {
    m_layoutAffichage = "plein_ecran";
    m_lecteurVideo = "VLC";
    m_resolution = "1920x1080";

    instanceVLC = libvlc_new(0, nullptr);
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

// --- PARTIE VLC ---
void M_LecteurPhysique::lireVideo(const string &cheminVideo) {
    if (!filesystem::exists(cheminVideo)) return;

    libvlc_media_t *media = libvlc_media_new_path(instanceVLC, cheminVideo.c_str());
    if (!media) return;

    libvlc_media_player_set_media(lecteurVLC, media);
    libvlc_media_release(media);

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

// --- PARTIE MATÉRIELLE COMPATIBLE LINUX/WINDOWS ---
void M_LecteurPhysique::collecterInfosLocales() {
    collecterNom();
    collecterIpEtMac();
    collecterOs();
    collecterCpu();
    collecterMemoire();
#ifdef _WIN32
    m_typeMachine = "PC-Windows";
#else
    m_typeMachine = "PC-Linux";
#endif
}

void M_LecteurPhysique::collecterNom() {
    char hostname[256];
#ifdef _WIN32
    DWORD size = sizeof(hostname);
    if (GetComputerNameA(hostname, &size)) {
        m_nom = string(hostname);
    } else {
        m_nom = "LecteurWin";
    }
#else
    if (gethostname(hostname, sizeof(hostname)) == 0) m_nom = string(hostname);
    else m_nom = "LecteurLinux";
#endif
}

void M_LecteurPhysique::collecterIpEtMac() {
#ifdef _WIN32
    ULONG bufferSize = 0;
    // Premier appel pour connaître la taille de la mémoire à allouer
    if (GetAdaptersInfo(nullptr, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        vector<char> buffer(bufferSize);
        PIP_ADAPTER_INFO adapterInfo = reinterpret_cast<PIP_ADAPTER_INFO>(buffer.data());

        // Second appel pour récupérer les données
        if (GetAdaptersInfo(adapterInfo, &bufferSize) == NO_ERROR) {
            while (adapterInfo) {
                // On ignore les interfaces virtuelles de boucle locale (Loopback)
                if (adapterInfo->Type != MIB_IF_TYPE_LOOPBACK && string(adapterInfo->IpAddressList.IpAddress.String) != "0.0.0.0") {
                    m_ip = adapterInfo->IpAddressList.IpAddress.String;

                    // Formatage de l'adresse MAC en XX:XX:XX:XX:XX:XX
                    char macStr[18];
                    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
                             adapterInfo->Address[0], adapterInfo->Address[1], adapterInfo->Address[2],
                             adapterInfo->Address[3], adapterInfo->Address[4], adapterInfo->Address[5]);
                    m_mac = macStr;
                    break; // On s'arrête à la première vraie carte réseau trouvée
                }
                adapterInfo = adapterInfo->Next;
            }
        }
    }
    // Valeurs de secours au cas où aucune carte n'est connectée
    if (m_ip.empty()) m_ip = "127.0.0.1";
    if (m_mac.empty()) m_mac = "00:00:00:00:00:00";
#else
    ifaddrs *interfaces = nullptr;
    if (getifaddrs(&interfaces) != 0) {
        m_ip = "0.0.0.0";
        m_mac = "00:00:00:00:00:00";
        return;
    }
    string ifaceRetenue;
    for (ifaddrs *it = interfaces; it != nullptr; it = it->ifa_next) {
        if (it->ifa_addr == nullptr) continue;
        string nomIface(it->ifa_name);
        if (nomIface == "lo") continue;
        if (it->ifa_addr->sa_family == AF_INET && m_ip.empty()) {
            char ipStr[INET_ADDRSTRLEN];
            auto *addr = reinterpret_cast<sockaddr_in *>(it->ifa_addr);
            inet_ntop(AF_INET, &addr->sin_addr, ipStr, sizeof(ipStr));
            m_ip = string(ipStr);
            ifaceRetenue = nomIface;
        }
    }
    freeifaddrs(interfaces);

    auto lireMac = [](const string &iface) -> string {
        ifstream f("/sys/class/net/" + iface + "/address");
        string mac;
        if (f.is_open()) getline(f, mac);
        return mac;
    };
    if (!ifaceRetenue.empty()) m_mac = lireMac(ifaceRetenue);
    if (m_mac.empty()) m_mac = lireMac("eth0");
    if (m_mac.empty()) m_mac = lireMac("wlan0");
    if (m_mac.empty()) m_mac = "00:00:00:00:00:00";
    if (m_ip.empty()) m_ip = "0.0.0.0";
#endif
}

void M_LecteurPhysique::collecterOs() {
#ifdef _WIN32
    HKEY hKey;
    // Ouverture du registre pour lire la version de Windows
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

void M_LecteurPhysique::collecterCpu() {
#ifdef _WIN32
    HKEY hKey;
    // Ouverture du registre pour lire le nom du processeur physique
    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
        char cpuName[256] = {0};
        DWORD dataSize = sizeof(cpuName);
        if (RegQueryValueExA(hKey, "ProcessorNameString", nullptr, nullptr, reinterpret_cast<LPBYTE>(cpuName), &dataSize) == ERROR_SUCCESS) {
            m_processeur = string(cpuName);
        }
        RegCloseKey(hKey);
    }
    if (m_processeur.empty()) m_processeur = "Inconnu";
#else
    ifstream f("/proc/cpuinfo");
    if (!f.is_open()) {
        m_processeur = "Inconnu";
        return;
    }
    string ligne;
    while (getline(f, ligne)) {
        if (ligne.rfind("model name", 0) == 0) {
            size_t pos = ligne.find(':');
            if (pos != string::npos) {
                m_processeur = ligne.substr(pos + 2);
                return;
            }
        }
        if (ligne.rfind("Hardware", 0) == 0 && m_processeur.empty()) {
            size_t pos = ligne.find(':');
            if (pos != string::npos) m_processeur = ligne.substr(pos + 2);
        }
    }
    if (m_processeur.empty()) m_processeur = "Inconnu";
#endif
}

void M_LecteurPhysique::collecterMemoire() {
#ifdef _WIN32
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    if (GlobalMemoryStatusEx(&statex)) {
        m_memoireMo = static_cast<int>(statex.ullTotalPhys / (1024 * 1024));
    } else {
        m_memoireMo = 0;
    }
#else
    ifstream f("/proc/meminfo");
    if (!f.is_open()) {
        m_memoireMo = 0;
        return;
    }
    string ligne;
    while (getline(f, ligne)) {
        if (ligne.rfind("MemTotal:", 0) == 0) {
            istringstream iss(ligne);
            string label, unite;
            long kB = 0;
            iss >> label >> kB >> unite;
            m_memoireMo = static_cast<int>(kB / 1024);
            return;
        }
    }
    m_memoireMo = 0;
#endif
}

string M_LecteurPhysique::versJson() const {
    map<string, string> champs = {
        {"nom", m_nom}, {"ip", m_ip}, {"mac", m_mac}, {"os", m_os},
        {"typeMachine", m_typeMachine}, {"resolution", m_resolution},
        {"tailleEcran", to_string(m_tailleEcranPouces)}, {"processeur", m_processeur},
        {"memoire", to_string(m_memoireMo)}, {"lecteurVideo", m_lecteurVideo},
        {"layoutAffichage", m_layoutAffichage}, {"nbMaxVideos", to_string(m_nbMaxVideos)}
    };
    return M_JsonUtil::construire(champs);
}