# Projet_RSA_dedonato_joly

Projet de RSA, TELECOM 2A IL, réalisation d'un Twitter

Acteurs :
- Laury de Donato
- Clément Joly

## Rapport du projet

Le rapport du projet contenant l'application est disponible ici : [rapport](rapport_RSA_DE_DONATO_JOLY.pdf)

## Dépendances

Les bibliothèques utilisées et nécessaires à l'application sont :

- [json-c](http://json-c.github.io/json-c/) : version 0.13
- [sqlite](http://sqlite.org/) : version 3.28


## Mode d’emploi

Pour tout ce qui suit, cloner le dépot et aller dans le dossier cloné.

**Les dépendances doivent aussi avoir été installées afin que l'application puisse être compilée et donc fonctionner.**

### Compilation de l'application

Pour compiler l'application, deux choix sont possibles :
- Il est possible de compiler l'application de manière globale. 
- Il est aussi possible de compiler le client et le serveur séparément.

#### Application globale :

Dans le dossier racine effectuer la commande suivante :

```
make
```
  
Cette commande compile à la fois le client et le serveur, et initialise la base
de données si elle ne l'a pas déjà été précédemment.


#### Client uniquement

Dans le dossier racine effectuer la commande suivante :

```
cd src/client
make
```

Ou directement dans le dossier `src/client` la commande suivante :

```
make
```
#### Serveur uniquement

Dans le dossier racine effectuer la commande suivante :

```
cd src/serveur
make
```

Ou directement dans le dossier `src/serveur` la commande suivante :

```
make
```

### Lancement de l'application

Pour le client, deux choix sont possibles :
 - Dans le dossier racine :
   ``` bash
   ./client IP_ou_nom_serveur [port]   #Lien symbolique pointant vers src/client/client
   ```
 - Dans le dossier `src/client` :
  ``` bash
  ./client IP_ou_nom_serveur [port]
  ```
   
Pour le serveur, deux choix sont possibles :
 - Dans le dossier racine :
   ``` bash
   ./server [port]   #Lien symbolique pointant vers src/server/server
   ```
 - Dans le dossier `src/server` :
  ``` bash
  ./server [port]
  ```
   
   
### Suppression des artefacts de compilation : `clean`

Pour supprimer les artefact de compilation, effectuer dans le dossier racine la commande suivante :
``` bash
make clean
```

### Suppression des artefacts de compilation et des données du serveur : `clean_all`

Pour supprimer les artefact de compilation et les données du serveur, effectuer dans le dossier racine la commande suivante :
```
make clean_all
```



