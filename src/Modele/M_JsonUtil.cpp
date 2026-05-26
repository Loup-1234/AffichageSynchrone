#include "../../include/Modele/M_JsonUtil.h"

#include <sstream>
#include <iomanip>
#include <stdexcept>

void M_JsonUtil::sauterEspaces(const string &json, size_t &pos) {
    // Avancement de l index tant qu un caractere de controle ou espace est detecte
    while (pos < json.size() &&
           (json[pos] == ' ' || json[pos] == '\t' ||
            json[pos] == '\r' || json[pos] == '\n')) {
        pos++;
    }
}

string M_JsonUtil::echapperChaine(const string &s) {
    string out;
    out.reserve(s.size() + 4);
    for (unsigned char c: s) {
        switch (c) {
            case '"': out += "\\\""; break;
            case '\\': out += "\\\\"; break;
            case '\b': out += "\\b"; break;
            case '\f': out += "\\f"; break;
            case '\n': out += "\\n"; break;
            case '\r': out += "\\r"; break;
            case '\t': out += "\\t"; break;
            default:
                if (c < 0x20) {
                    ostringstream oss;
                    oss << "\\u" << hex << setw(4)
                            << setfill('0') << static_cast<int>(c);
                    out += oss.str();
                } else {
                    out += static_cast<char>(c);
                }
        }
    }
    return out;
}

string M_JsonUtil::lireChaine(const string &json, size_t &pos) {
    if (pos >= json.size() || json[pos] != '"') {
        throw runtime_error("M_JsonUtil::lireChaine : guillemet attendu");
    }
    pos++;

    string resultat;
    while (pos < json.size() && json[pos] != '"') {
        if (json[pos] == '\\') {
            pos++;
            if (pos >= json.size()) {
                throw runtime_error("M_JsonUtil::lireChaine : sequence d'echappement tronquee");
            }
            switch (json[pos]) {
                case '"': resultat += '"'; break;
                case '\\': resultat += '\\'; break;
                case '/': resultat += '/'; break;
                case 'b': resultat += '\b'; break;
                case 'f': resultat += '\f'; break;
                case 'n': resultat += '\n'; break;
                case 'r': resultat += '\r'; break;
                case 't': resultat += '\t'; break;
                case 'u': {
                    if (pos + 4 >= json.size()) {
                        throw runtime_error("M_JsonUtil::lireChaine : \\uXXXX tronque");
                    }
                    string hex = json.substr(pos + 1, 4);
                    int cp = stoi(hex, nullptr, 16);
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
        throw runtime_error("M_JsonUtil::lireChaine : guillemet fermant manquant");
    }
    pos++;
    return resultat;
}

string M_JsonUtil::lireValeurBrute(const string &json, size_t &pos) {
    string valeur;
    // Accumulation des caracteres de la valeur jusqu au prochain separateur structurel
    while (pos < json.size() &&
           json[pos] != ',' && json[pos] != '}' &&
           json[pos] != ' ' && json[pos] != '\t' &&
           json[pos] != '\r' && json[pos] != '\n') {
        valeur += json[pos++];
    }
    return valeur;
}

string M_JsonUtil::construire(const map<string, string> &champs) {
    string json = "{";
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

map<string, string> M_JsonUtil::parser(const string &json) {
    map<string, string> resultat;

    if (json.empty()) return resultat;

    size_t pos = 0;
    sauterEspaces(json, pos);

    if (pos >= json.size() || json[pos] != '{') return resultat;
    pos++;

    // Analyse syntaxique de la chaine JSON par itérations successives
    while (pos < json.size()) {
        sauterEspaces(json, pos);
        if (pos >= json.size() || json[pos] == '}') break;

        if (json[pos] != '"') break;
        string cle = lireChaine(json, pos);

        sauterEspaces(json, pos);
        if (pos >= json.size() || json[pos] != ':') break;
        pos++;

        sauterEspaces(json, pos);
        string valeur;
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