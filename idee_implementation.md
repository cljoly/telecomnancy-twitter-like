# Serveur

- stocker la liste des utilisateurs & mdp (hash?) dans un fichier en msgpack
- pas de bd, messages de la session en mémoire (liste chainée d’objets)
- limite du nb de client -> fork à chaque requête client (à tester) ? Un processus par client ?
- faire expirer le cookie à la fermeture 
- utiliser sqilte pour créer les comptes utilisateurs avec trucs suivis

# Client

- Plusieurs arguments en ligne de commandes
  - watch : recevoir les messages
  - autres commandes : publication
- expression régulière pour extraire les thématiques des messages
- affichage message :
  - éventuel relayeur (précédé de la mention « relayé par »)
  - nom auteur
  - corps du message
- var globale pour tenir compte de l’identifiant
- le format nom d’utilisateur / mdp doît être vérifié côté client
- on retourne toujours au menu de base après une action utilisateur
  - menu connecté ≠ non conncté

# Commun

- Bibliothèque JSON intéressante : https://github.com/netmail-open/wjelement

# Spec

- carac mdp
- confirmer la connexion au client
- le serveur fera raisonnablement confiance au client
  - comportement simple -> jeter le paquet

  
# Fonctionnalités en plus

- Se désabonner
- Enlever retweet de la spécification ?

- Changement de mdp, de nom d’utilisateur
- Biographie…
- 

## À faire
- **Penser à tester en IPv6**
- Script test automatique

# Questions

- unicode/ascii ?
  - ASCII
- ordre dans la spec : primitives de service & format apdu
- cookie d’identification
- nouveaux messages en continu
  - attendre sur stdin & sur le réseau en parallèle

- nom de la méthode : create_user
- params:
  - p1
  - p2

- limites dans le nombre de client ? (par exemple avec des fork)
  - ok, 50

