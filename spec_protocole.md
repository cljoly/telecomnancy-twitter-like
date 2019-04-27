# MyTwitter : Spécification du protocole de communication client/serveur

Version: 1.0


## Introduction

Dans le cadre du projet de RSA, nous écrivons un service semblable à Twitter[^t].

Ce document décrit le protocole de communication entre un client et un serveur de ce service.


## Définitions

Dans cette partie nous définissons tous les termes utiles à notre application.

### Gazouilli
Message publié par un utilisateur. C'est un message textuel d'un maximum de 140 caractères. Le message peut référencer une thématique particulière,
mais pas un autre utilisateur. Les thématiques sont comptées dans la limite des 140 caractères. Les caractères seront encodés en ASCII.

Le nom d'auteur du gazouilli ne fait pas partie de la limite des 140 caractères d'un gazouilli.


### Thématique
Mot d’un message commençant par un `#`. Ces thématiques doivent servir au regroupement des messages ayant un thème commun.
Deux thématiques sont séparées par un espace. Les thématiques sont séparées du corps du message par un espace. Les thématiques peuvent
se trouver à n'importe quelle position dans le message. Deux thématiques identiques n'ayant pas la meme casse seront considérées comme deux thématiques
différentes. Ainsi : `#Twitter` et `#twitter` seront considérées comme deux thématiques différentes.

Chaque caractère de la thématique compte pour un caractère du gazouilli.

### Relayer un gazouilli
Retransmettre le message d’un autre utilisateur à ses abonnés. La mention `relayé par <nom_relayeur>` est alors indiquée.

### Abonné
Utilisateur qui suit un autre utilisateur. Un abonné recevra tous les gazouillis postés par les utilisateurs qu'il suit.


## Fonctionnalités :

- Fonctionnalité 1 : Une personne doit pouvoir s'inscrire sur MyTwitter, elle devient alors un utilisateur.
- Fonctionnalité 2 : Un utilisateur doit pouvoir se connecter sur MyTwitter.
- Fonctionnalité 3 : Un utilisateur connecté doit pouvoir envoyer un gazouilli.
- Fonctionnalité 4 : Un utilisateur connecté doit pouvoir demander de suivre un autre utilisateur.
- Fonctionnalité 5 : Un utilisateur connecté doit pouvoir demander de suivre une thématique donnée.
- Fonctionnalité 6 : Un utilisateur connecté doit pouvoir afficher la liste des utilisateurs qu'il suit
- Fonctionnalité 7 : Un utilisateur connecté doit pouvoir afficher la liste des thématiques qu'il suit
- Fonctionnalité 8 : Un utilisateur connecté doit pouvoir afficher la lsite des utilisateurs qui le suivent
- Fonctionnalité 9 : Un utilisateur connecté doit pouvoir recevoir à tout moment les messages liés aux utilisateurs et/ou aux thématiques qu'il suit.
- Fonctionnalité 10 : Un utilisateur connecté doit pouvoir relayer un message d'un autre utilisateur qu'il a reçu à ses propres abonnés
- Fonctionnalité 11 : Un utilisateur connecté doit pouvoir se déconnecter


## Forme des APDU
Pour coder les APDU, nous avons tout d’abord envisagé d’utiliser le format binaire msgpack[^msgpack]. Il présente l’avantage d’être compact, 
tout en étant relativement facile à déboguer, puisqu’il peut être converti dans les deux sens en JSON[^json].

Cependant, après réflexion, nous avons décidé de travailler directement avec un format de données JSON, dans un souci de simplicité de débuggage.
C'est un format moins compact mais qui peut être débuggé directement (sans avoir à être converti). 

La spécification des requêtes est inspirée du JSON-RPC[^jrpc].

La taille maximum d’une APDU est de **1500 octets**.

### Format du dialogue client/serveur
#### Requêtes


Une requête doit avoir le format suivant :

| Champs | Rôle |
|----:|:---|
| `request`| Identifie la méthode appelée |
| `params` | Paramètres de la méthode, nommés ou non, voir plus loin |
| `id` | Champs généré par le client, unique pour une requête. Il permet au client d’associer la réponse à la requête initiale |

Une réponse correcte à une requête doit avoir le format suivant :

| Champs | Rôle |
|----:|:---|
| `result` | Nom de la méthode réalisée avec succès |
| `params` | Objet contenant les informations de la réponse |
| `id` | Comme précédemment |


Une réponse d'erreur à une requête doit avoir le format suivant :

| Champs | Rôle |
|----:|:---|
| `error` | Nom de la méthode réalisée avec erreur |
| `error_code` | Numéro de l'erreur, est toujours un entier. Il est unique pour une méthode donnée |
| `id` | Comme précédemment |


Si le nombre de paramètre est connu, on pourra nommer directement les paramètres.

``` json
{"request": "nom_methode", "params": {"param1": "val1", "param2": "val2"}, "id": 3}
```

Si le nombre de paramètre est inconnu, on pourra utiliser un tableau de paramètres.

``` json
{"resquest": "nom_methode", "params": […], "id": 9}
```

On préférera nommer explicitement les paramètres, dans un soucis de lisibilité.

#### Réponse sans erreur

``` json
{"result": "nom_methode", "params": {"param1": "val1", "param2": "val2"}, "id": 3}
```

#### Réponse d’erreur

``` json
{ "error": "nom_methode", "error_code": 1234, "id": 3}
```

Notes :
- Si la requete possède un format incorrect, un message d'erreur sera renvoyé. Par exemple, si l’id n’est pas fourni par le client, la requête est incorrecte et on produit un réponse d’erreur avec un champs `id` contenant la valeur null
- Le `code` de l’erreur doit toujours être associé à la même erreur. On définit :
    - Le code `10` pour un message dont le format n'est pas correct (par exemple s’il est trop long ou ne respecte pas le format JSON)
    - Le code `11` pour une erreur interne du côté serveur
    - Le code `12` pour une erreur interne du côté client
    - Le code `13` pour une erreur liée à une fonctionnalité non implémentée côté client ou serveur.



## Objets généraux

### Gazouilli

Voici un exemple de la structure JSON d'un Gazouilli. Elle comporte trois champs : `id` qui est un entier, `content` qui est un String et `tags` qui est un tableau de String. Chaque `id` sera unique.
``` Json
{
    "id": 3,
    "content": "Bonjour #MyTwitter et #HelloWorld !",
    "author": "JeanDupont",
    "list_of_tags": ["MyTwitter", "HelloWorld"],
    "date": "2019-03-18T17:15:00"
}
```

### Utilisateur

Voici un exemple de la structure JSON d'un utilisateur. Elle comporte un champs : `username`.
``` Json
{
    "username": "LouisSchmit"
}
```

## Méthodes

### Création de compte : `create_account`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `username` | String | Nom d'utilisateur |
| `password` | String | Mot de passe |

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom d'utilisateur déjà pris |


### Autoriser une connexion : `connect`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `username` | String | Nom d'utilisateur |
| `password` | String | Mot de passe |

#### Retour sans erreur :

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `cookie` | int | Identifiant de connexion unique, valable tout le temps de la connexion |

Dans le cas où l'utilisateur est déjà connecté, on définit un nouveau cookie de manière à ne pas bloquer un utilisateur qui aurait mal fermé une session précédente.

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom d'utilisateur non enregistré |
| 2 | Mot de passe incorrect |


### Envoi d’un gazouilli : `send_gazou`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `gazouilli` | Gazouilli | Objet de type Gazouilli |


Un message sans thématique aura le tableau de tags vide, et ne sera envoyé qu'aux abonnés de l'utilisateur envoyant le gazouilli.

Un même tag présent X fois dans le gazouilli ne provoquera qu'un seul envoi de ce gazouilli pour cette thématique.

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Message comportant un/des caractère(s) non supporté(s) |
| 2 | Message trop long |

### Relayer un gazouilli : `relay_gazou`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `id_gazouilli` | entier | Identifiant unique du gazouilli |


#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom d'utilisateur inconnu |
| 2 | Déjà abonné |

### Demander à suivre un utilisateur : `follow_user`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `username` | String | Nom d'utilisateur à suivre |


#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom d'utilisateur inconnu |
| 2 | Déjà abonné |

### Demander à suivre une thématique : `follow_tag`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `tag` | String | Nom de la thématique à suivre |

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom d'utilisateur inconnu |
| 2 | Déjà abonné |

### Demander à ne plus suivre un utilisateur : `unfollow_user`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `username` | String | Nom de l'utilisateur à ne plus suivre |

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom d'utilisateur inconnu |
| 2 | Non abonné |

### Demander à ne plus suivre une thématique : `unfollow_tag`

Sens : Client - Serveur

Paramètres :

| Nom | Type | Description |
|----:|:---:|:---|
| `tag` | String | Nom de la thématique à ne plus suivre |

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur :

| Valeur | Description |
|----:|:---|
| 1 | Nom de la thématique inconnue |
| 2 | Non abonné |

### Lister les utilisateurs suivis : `list_followed_users`

Sens : Client - Serveur

Paramètres : aucun

#### Retour sans erreur :

Paramètres :
| Nom | Type | Description |
|----:|:---:|:---|
| `list_of_users` | Tableau d'utilisateurs | Liste d'objets de type utilisateur qui référence tous les utilisateurs suivis |

#### Retour avec erreur :

Valeur des codes d'erreur : aucun, hormis les codes globaux définis plus haut

### Lister les thématiques suivies : `list_followed_tags`

Sens : Client - Serveur

Paramètres : aucun

#### Retour sans erreur :

Paramètres :
| Nom | Type | Description |
|----:|:---:|:---|
| `list_of_tags` | Tableau de String | Liste des thématiques suivies |

#### Retour avec erreur :

Valeur des codes d'erreur : aucun, hormis les codes globaux définis plus haut

### Lister les abonnés d’un utilisateur : `list_my_followers`

Sens : Client - Serveur

Paramètres : aucun

#### Retour sans erreur :

Paramètres :
| Nom | Type | Description |
|----:|:---:|:---|
| `list_of_followers` | Tableau d'utilisateurs | Liste d'objets de type utilisateur qui référence tous les utilisateurs auquels on est abonné |

#### Retour avec erreur :

Valeur des codes d'erreur : aucun, hormis les codes globaux définis plus haut

### Déconnexion du client : `disconnect`

Sens : Client - Serveur

Paramètres : aucun

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur : aucun, hormis les codes globaux définis plus haut

### Transférer un message reçu à tous les abonnés de l'auteur du gazouilli et aux utilisateurs suivants les thématiques mentionnées dans le gazouilli : `update`

Sens : Serveur - Client

**Information :** si un utilisateur receveur du gazouilli est à la fois un abonné de l'utilisateur auteur du gazouilli et abonné d'au moins une thématique contenue dans le gazouilli, le serveur devra veiller à n'envoyer le gazouilli qu'une et une seule fois.

Paramètres :
| Nom | Type | Description |
|----:|:---:|:---|
| `gazouilli` | Gazouilli | Objet Gazouilli |

#### Retour sans erreur :

Paramètres : aucun

#### Retour avec erreur :

Valeur des codes d'erreur : aucun, hormis les codes globaux définis plus haut



## Références
[^t]: https://twitter.com/
[^msgpack]: https://msgpack.org/
[^json]: http://json.org/
[^jrpc]: https://www.jsonrpc.org/specification
