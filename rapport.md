# Introduction

Dans le cadre de ce projet de RSA, nous réalisons une application client/serveur présentant des fonctionnalités
proches de Twitter[^t].

Ce document présente les caractéristiques de notre travail et une capture des
échanges client/serveur avec l’outil  Wireshark[^Wireshark]. Il se termine par
la spécification du protocole utilisé pour la communication client/serveur.

[^t]: https://twitter.com/
[^Wireshark]: https://www.wireshark.org/

# Caractéristiques de notre projet

## Techniques

Nous avons utilisé une base de données et le format de données JSON pour la
communication client/serveur. Ceci a été l’occasion d’apprendre à utiliser de
manière plus approfondie des bibliothèques spécifiques en C comme `sqlite3` ou
`json-c`. En effet, les projets précédents n’autorisaient pas forcément
l’utilisation de bibliothèques externes.

Notre projet présente les fonctionnalités facultatives suivantes :

- Sauvegarde des tweets (appelés par la suite gazouillis) et des comptes utilisateurs (ils ne sont pas perdu au
  redémarrage du serveur) ;
- Fonction « retweet » : un utilisateur peut relayer un gazouilli,
  éventuellement plusieurs fois. Il apparaît alors pour ses propres abonnés,
  inséré parmi les autres gazouillis dans l’ordre chronologique.

Enfin, dans un souci d’optimisation de la charge pour le serveur, l’ensemble
des gazouillis récents sont envoyés au client lorsque celui-ci le lui demande. Le
serveur fournit le nombre de tweet demandé par le client, ce qui simplifie
l’implémentation côté client. Par ailleurs, le client n’a ainsi rien à stocker.

Des précautions ont été prises afin d’éviter les injections SQL. Cependant, les
mots de passe sont stockés et transmis en clair, comme autorisé par le sujet.

## Gestion de projet

Nous avons établi la spécification avant de commencer à écrire le client ou le
serveur, comme en atteste les commits du fichier `spec_protocol.md`. La spécification
comprend les primitives de service nécessaires à la réalisation de l'application ainsi que
la liste et le format des Application Data Units (APDUs), c'est-à-dire la liste et le format des
messages qui vont transiter entre le client et le serveur au niveau applicatif.

Écrire en premier lieu la spécification avant de commencer à proprement parler l'implémentation
nous a permis par la suite de travailler de manière séparée sur le client et sur le serveur, mais aussi
sur des environnements de développement différents. Ce travail préalable nous a grandement facilité
le développement de l'application.

Pour les ajustements éventuels qui se sont avérés nécessaires, la personne souhaitant effectuer une
modification de la spécification la proposait dans une Merge-Request sur
l’instance Gitlab de l’école. Elle était alors discutée puis acceptée ou rejetée.

\newpage

# Capture Wireshark

Nous avons effectué le schéma suivant et capturé les paquets à l’aide Wireshark (dans la fichier `capture_wireshark.pcapng`) :

1. Le client n°1 lance l’application et se connecte en IPv6 (trame 1). Le serveur accepte la connexion (trames 2 et 3).
2. Le client n°1 crée un compte pour l’utilisateur « @alice » avec le mot de passe « alice » (trame 4). Le serveur traite la requête (trame 6).
3. Le client n°1 se connecte avec l’utilisateur « @alice » et le mot de passe « alice » (trame 10). Le serveur traite la requête (trame 11).
1. Le client n°2 lance l’application et se connecte en IPv4 (trame 15). Le serveur accepte la connexion (trames 16 et 17).
2. Le client n°2 crée un compte pour l’utilisateur « @bob » avec le mot de passe « bob » (trame 18). Le serveur traite la requête (trame 20).
3. Le client n°2 se connecte avec l’utilisateur « @bob » et le mot de passe « bob » (trame 24). Le serveur traite la requête (trame 25).
7. Le client n°1 envoie un gazouilli dont le contenu est « Je teste #MyTwitter » (trame 29). Le serveur traite la requête (trame 31).
7. Le client n°2 envoie un gazouilli dont le contenu est « J'aime #MyTwitter » (trame 35). Le serveur traite la requête (trame 36).
7. Le client n°2 demande à suivre le tag « MyTwitter » (trame 40). Le serveur traite la requête et lui indique que le tag est bien suivi (trame 41).
7. Le client n°2 demande à relayer un gazouilli. Pour cela, le logiciel client récupère d’abord les gazouillis relayables (trame 45 et 46) et les affiche. Le client n°2 choisit de relayer le gazouilli dont l’id est 1 (trame 50). Le serveur traite la requête et lui indique que le gazouilli est bien relayé (trame 51).
8. Le client n°2 demande l’affichage des gazouillis auxquels il est abonné (trame 55). Le serveur traite la requête et lui renvoie les gazouillis correspondants (trame 56).
9. Le client n°2 demande à se déconnecter du serveur (trame 60). Le serveur traite la requête (trame 61).
9. Le client n°1 ferme l’application. Il ferme donc la connexion TCP (trames 71 et 72). Le serveur répond et ferme la connexion (trames 76 et 77).
9. Le client n°2 ferme l’application. Il ferme donc la connexion TCP (trames 78 et 79). Le serveur répond et ferme la connexion (trames 83 et 84).


Nous avons vérifié que les APDUs soient conformes à la spécification. Dans un souci de longueur, nous n’avons pas fait apparaître sur la capture toutes les fonctionnalités proposées par MyTwitter.

\newpage

