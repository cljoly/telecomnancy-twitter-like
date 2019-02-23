# MyTwitter : Spécification du protocole de communication client/serveur
Version: 0.1


## Introduction
Dans le cadre du projet de RSA, nous écrivons un service comparable à Twitter[^t].

Ce document décrit le protocole de communication entre un client et un serveur de ce service.

## Forme des APDU
Les APDU seront codés avec le format binaire msgpack[^msgpack]. Il présente l’avantage d’être compact, tout en étant relativement facile à déboguer, puisqu’il peut être converti dans les deux sens en JSON[^json].
Une bibliothèque est disponible en C : https://github.com/msgpack/msgpack-c

Le langage de requête est inspiré du JSON-RPC[^jrpc].

### Requêtes au serveur

| Champs | Rôle |
|----:|:---|
| `method`| Identifie la méthode appelée sur le serveur |
| `params` | Paramètres de la méthode, nommés ou non, voir plus loin |
| `id` | Champs généré par le client, unique pour une requête. Il permet au client d’associer la réponse à la requête initiale |
| `error` | Objet présent en cas d’erreur |
| `result` | Objet présent si la méthode s’est déroulée avec succès |

On préférera nommer explicitement les paramètres, dans un soucis de lisibilité.
``` json
{"method": "update", "params": {"param1": "val1", "param2": "val2"}, id: 3}
```

Si le nombre de paramètre est inconnu, on pourra utiliser un tableau de paramètres.

``` json
{"method": "update", "params": […], id: 9}
```

### Réponses du serveur
#### Réponse d’erreur

``` json
{ "error": {"msg": "Message détaillant l’erreur", "code": 1234}, id: 3}
```
Notes :
- Si l’id n’est pas fourni par le client, la requête est incorrecte et on produit un réponse d’erreur avec un champs `id` contenant la valeur null
- Le `code` de l’erreur doit toujours être associé à la même erreur
## Définitions
### Gazouilli
Message publiés par un utilisateur.
#### Relayer un gazouilli
Retransmettre le message d’un autre utilisateur .
### Abonné
Utilisateurs qui suit un autre utilisateur
### Thématique
Mot d’un message commençant par un `#`. Ces thématiques doivent servir au regropement des messages ayant un thème commun.
## Objets généraux

### Gazouilli

### Utilisateur

## Méthodes

### Création de compte
Hacher le mdp ?
### Authoriser une connexion
Hacher le mdp ?
### Envoie d’un _tweet_

### Demander à suivre des utilisateurs et/ou des thématiques

### Lister les abonnés et thématiques suivis

### Lister les abonnés d’un utilisateur

### Recevoir les messages d’utilisateurs ou de thématiques suivis

À tout moment

### Déconnexion explicite

Réponse : attente de l’accord du serveur.


Pour référence/inspiration

https://medium.com/@ConsenSys/blockchain-underpinnings-2c43ba03ecc9

## Références
[^msgpack]: https://msgpack.org/
[^json]: http://json.org/
[^jrpc]: https://www.jsonrpc.org/specification
