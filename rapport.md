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

TODO
