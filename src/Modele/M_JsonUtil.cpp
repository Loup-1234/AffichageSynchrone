#include "../../include/Modele/M_JsonUtil.h"

#include <sstream>
#include <iomanip>
#include <stdexcept>

// ================================================================
// M_JsonUtil V1 — implementation RFC 8259 pour objets JSON plats.
// ================================================================

void M_JsonUtil::sauterEspaces(const std::string &json, size_t &pos) {
    while (pos < json.size() &&
           (json[pos] == ' ' || json[pos] == '\t' ||
            json[pos] == '\r' || json[pos] == '\n')) {
        pos++;
    }
}

std::string M_JsonUtil::echapperChaine(const std::string &s) {
    std::string out;
    out.reserve(s.size() + 4);
    for (unsigned char c: s) {
        switch (c) {
            case '"': out += "\\\"";
                break;
            case '\\': out += "\\\\";
                break;
            case '\b': out += "\\b";
                break;
            case '\f': out += "\\f";
                break;
            case '\n': out += "\\n";
                break;
            case '\r': out += "\\r";
                break;
            case '\t': out += "\\t";
                break;
            default:
                if (c < 0x20) {
                    std::ostringstream oss;
                    oss << "\\u" << std::hex << std::setw(4)
                            << std::setfill('0') << static_cast<int>(c);
                    out += oss.str();
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

std::string M_JsonUtil::lireChaine(const std::string &json, size_t &pos) {
    if (pos >= json.size() || json[pos] != '"') {
        throw std::runtime_error("M_JsonUtil::lireChaine : guillemet attendu");
    }
    pos++;

    std::string resultat;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\') {
            pos++;
            if (pos >= json.size()) {
                throw std::runtime_error("M_JsonUtil::lireChaine : sequence d'echappement tronquee");
            }
            switch (json[pos]) {
                case '"': resultat += '"';
                    break;
                case '\\': resultat += '\\';
                    break;
                case '/': resultat += '/';
                    break;
                case 'b': resultat += '\b';
                    break;
                case 'f': resultat += '\f';
                    break;
                case 'n': resultat += '\n';
                    break;
                case 'r': resultat += '\r';
                    break;
                case 't': resultat += '\t';
                    break;
                case 'u': {
                    if (pos + 4 >= json.size()) {
                        throw std::runtime_error("M_JsonUtil::lireChaine : \\uXXXX tronque");
                    }
                    std::string hex = json.substr(pos + 1, 4);
                    int cp = std::stoi(hex, nullptr, 16);
                    if (cp < 0x80) {
                        resultat += static_cast<char>(cp);
                    } else if (cp < 0x800) {
                        resultat += static_cast<char>(0xC0 | (cp >> 6));
                        resultat += static_cast<char>(0x80 | (cp & 0x3F));
                    } else {
                        resultat += static_cast<char>(0xE0 | (cp >> 12));
                        resultat += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
                        resultat += static_cast<char>(0x80 | (cp & 0x3F));
                    }
                    pos += 4;
                    break;
                }
                default: resultat += json[pos];
            }
        } else {
            resultat += json[pos];
        }
        pos++;
    }

    if (pos >= json.size()) {
        throw std::runtime_error("M_JsonUtil::lireChaine : guillemet fermant manquant");
    }
    pos++;
    return resultat;
}

std::string M_JsonUtil::lireValeurBrute(const std::string &json, size_t &pos) {
    std::string valeur;
    while (pos < json.size() &&
           json[pos] != ',' && json[pos] != '}' &&
           json[pos] != ' ' && json[pos] != '\t' &&
           json[pos] != '\r' && json[pos] != '\n') {
        valeur += json[pos++];
    }
    return valeur;
}

std::string M_JsonUtil::construire(const std::map<std::string, std::string> &champs) {
    std::string json = "{";
    bool premier = true;
    for (const auto &[cle, valeur]: champs) {
        if (!premier) json += ',';
        json += '"';
        json += echapperChaine(cle);
        json += "\":\"";
        json += echapperChaine(valeur);
        json += '"';
        premier = false;
    }
    json += '}';
    return json;
}

std::map<std::string, std::string> M_JsonUtil::parser(const std::string &json) {
    std::map<std::string, std::string> resultat;

    if (json.empty()) return resultat;

    size_t pos = 0;
    sauterEspaces(json, pos);

    if (pos >= json.size() || json[pos] != '{') return resultat;
    pos++;

    while (pos < json.size()) {
        sauterEspaces(json, pos);
        if (pos >= json.size() || json[pos] == '}') break;

        if (json[pos] != '"') break;
        std::string cle = lireChaine(json, pos);

        sauterEspaces(json, pos);
        if (pos >= json.size() || json[pos] != ':') break;
        pos++;

        sauterEspaces(json, pos);
        std::string valeur;
        if (pos < json.size() && json[pos] == '"') {
            valeur = lireChaine(json, pos);
        } else {
            valeur = lireValeurBrute(json, pos);
        }

        if (!cle.empty()) {
            resultat[cle] = valeur;
        }

        sauterEspaces(json, pos);
        if (pos < json.size() && json[pos] == ',') pos++;
    }

    return resultat;
}
