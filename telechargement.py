import os
import yt_dlp

# 1. Configuration du dossier de destination
DOSSIER_DESTINATION = r"C:\Users\ahumeaub\Desktop\CLionProject\AffichageSynchrone\cmake-build-debug"

# 2. Liste des liens fournie
LIENS_MUSIQUE = [
    "https://www.youtube.com/watch?v=Scop3XmycKk"
]

LIENS_VIDEOS = [
    "https://www.youtube.com/watch?v=kkzLWEVyMWs&list=RDkkzLWEVyMWs&start_radio=1",
    "https://www.youtube.com/watch?v=j8lg9wljwuw",
    "https://www.youtube.com/watch?v=eJI_OzBOaYQ",
    "https://www.youtube.com/watch?v=K9HuvYLtJIU",
    "https://www.youtube.com/watch?v=uZ0ZeQ4p4Pw&list=RDuZ0ZeQ4p4Pw&start_radio=1",
    "https://www.youtube.com/watch?v=FAZuWnr20hg",
    "https://www.youtube.com/watch?v=vd4WPW5IIh0",
    "https://www.youtube.com/watch?v=WuKINkqS4Mo_"
]

def telecharger_en_mp3(urls, chemin_sortie):
    """Télécharge les vidéos et les convertit en fichier audio MP3"""
    options = {
        'format': 'bestaudio/best',
        'outtmpl': os.path.join(chemin_sortie, '%(title)s.%(ext)s'),
        'noplaylist': True,
        'nocheckcertificate': True,  # 💡 ICI : Force l'ignorance de l'erreur SSL
        'postprocessors': [{
            'key': 'FFmpegExtractAudio',
            'preferredcodec': 'mp3',
            'preferredquality': '192',
        }],
    }

    print(f"\n--- Début du téléchargement Musique (MP3) dans : {chemin_sortie} ---")
    with yt_dlp.YoutubeDL(options) as ydl:
        ydl.download(urls)

def telecharger_en_mp4(urls, chemin_sortie):
    """Télécharge les vidéos au format MP4"""
    options = {
        'format': 'bestvideo[ext=mp4]+bestaudio[ext=m4a]/best[ext=mp4]/best',
        'outtmpl': os.path.join(chemin_sortie, '%(title)s.%(ext)s'),
        'noplaylist': True,
        'nocheckcertificate': True,  # 💡 ICI : Force l'ignorance de l'erreur SSL
    }

    print(f"\n--- Début du téléchargement Vidéos (MP4) dans : {chemin_sortie} ---")
    with yt_dlp.YoutubeDL(options) as ydl:
        ydl.download(urls)

if __name__ == "__main__":
    if not os.path.exists(DOSSIER_DESTINATION):
        os.makedirs(DOSSIER_DESTINATION)
        print(f"Dossier créé : {DOSSIER_DESTINATION}")

    if LIENS_MUSIQUE:
        telecharger_en_mp3(LIENS_MUSIQUE, DOSSIER_DESTINATION)

    if LIENS_VIDEOS:
        telecharger_en_mp4(LIENS_VIDEOS, DOSSIER_DESTINATION)

    print("\n🎉 Tous les téléchargements sont terminés avec succès !")