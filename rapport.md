% Projet de Réseau et Système Avancé «_MyTwitter_»
% Laury de Donato, Clément Joly
% Mai 2019

\newpage

# Introduction

Dans le cadre du projet de RSA, nous écrivons un service semblable à Twitter[^t].

Ce document présente les caractéristiques de notre travail et une capture des
échanges client/serveur avec l’outil  Wireshark[^Wireshark]. Il se termine par
la spécification du protocole utilisé pour la communication client/serveur.

[^t]: https://twitter.com/
[^Wireshark]: https://www.wireshark.org/

# Caractéristiques de notre projet

## Techniques

Nous avons utilisé une base de données et le format de donnée json pour la
communication client serveur.

Notre projet présente les fonctionnalités facultatives suivantes :

- sauvegarde des tweet et des comptes utilisateurs (il ne sont pas perdu au
  redémarrage du serveur)
- Fonction « retweet » : un utilisateur peut relayer un gazouilli,
  éventuellement plusieurs fois. Il apparaît alors pour ces propres abonnés,
  inséré parmis les autres gazouillis dans l’ordre chronologique.

Enfin, dans un soucis d’optimisation de la charge pour le serveur, l’ensemble
des tweets récent sont envoyés au client lorsque celui-ci le lui demande. Le
serveur fournit le nombre de tweet demandé par le client, ce qui simplifie
l’implémentation côté client. Par ailleurs, le client n’a ainsi rien à stocker.

## Gestion de projet

Nous avons établit la spécification avant de commencer à écrire le client ou le
serveur, comme en atteste les commits du fichier `spec_protocol.md`.

Pour les ajustement qui se sont avérés nécessaire, celui qui voulait une
modification de la spécification la proposait dans une Merge-Request sur
l’instance Gitlab de l’école. Elle était alors discuté et accepté ou rejeté.

# Capture Wireshark

TODO
