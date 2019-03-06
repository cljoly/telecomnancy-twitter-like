# MyTwitter : Spécification du protocole de communication client/serveur

Version: 0.1


## Introduction

Dans le cadre du projet de RSA, nous écrivons un service semblable à Twitter[^t]. 

Ce document décrit le protocole de communication entre un client et un serveur de ce service.


## Définitions

Dans cette partie nous définissons tous les termes utiles à notre application.

### Gazouilli
Message publié par un utilisateur. C'est un message textuel d'un maximum de 140 caractères. Le message peut référencer une thématique particulière,
 mais pas un autre utilisateur. Les thématiques sont comptées dans la limite des 140 caractères. **ASCII, unicode ?**
 
### Thématique
Mot d’un message commençant par un `#`. Ces thématiques doivent servir au regroupement des messages ayant un thème commun. 
Deux thématiques sont séparées par un espace. Les thématiques sont séparées du corps du message par un espace. Les thématiques peuvent
se trouver à n'importe quelle position dans le message. Deux thématiques identiques n'ayant pas la meme casse seront considérées comme deux thématiques 
différentes. Ainsi : `#Twitter` et `#twitter` seront considérées comme deux thématiques différentes.

### Relayer un gazouilli
Retransmettre le message d’un autre utilisateur à ses abonnés. La mention `relayé par <nom_relayeur>` est alors indiquée.

### Abonné
Utilisateur qui suit un autre utilisateur. Un abonné recevra tous les gazouillis postés par les utilisateurs qu'il suit.


## Fonctionnalités :

**à reprendre du sujet**









## Forme des APDU
Les APDU seront codés avec le format binaire msgpack[^msgpack]. Il présente l’avantage d’être compact, 
tout en étant relativement facile à déboguer, puisqu’il peut être converti dans les deux sens en JSON[^json].

Après réflexion, nous décidons de travailler directement avec un format de données en JSON, dans un souci de simplicité de débuggage.
C'est un format moins compact mais qui peut ^etre débuggé directement (sans avoir à être converti). 

La spécification des requêtes est inspirée du JSON-RPC[^jrpc].

### Format du dialogue client/serveur
#### Requêtes


Une requ^ete doit avoir le format suivant :

| Champs | Rôle |
|----:|:---|
| `request`| Identifie la méthode appelée |
| `params` | Paramètres de la méthode, nommés ou non, voir plus loin |
| `id` | Champs généré par le client, unique pour une requête. Il permet au client d’associer la réponse à la requête initiale |

Une réponse correcte à une requ^ete doit avoir le format suivant :

| Champs | Rôle |
|----:|:---|
| `result` | Nom de la méthode réalisée avec succès |
| `param` | Objet contenant les informations de la réponse |
| `id` | Comme précédemment |


Une réponse d'erreur à une requ^ete doit avoir le format suivant :

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
    - Le code `10` pour un message dont le format n'est pas correct



## Objets généraux

### Gazouilli

### Utilisateur

### Erreur



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


### Envoi d’un gazouilli

### Demander à suivre des utilisateurs et/ou des thématiques

### Lister les utilisateurs et thématiques suivis

#### Requête

``` json
{ "method": "list", "params": {"abonnes": true, "thématiques": true}, id: 398 }
```

#### Réponse

``` json
{ "method": "list", "result": [], id: 398 }
```

### Lister les abonnés d’un utilisateur

### Recevoir les messages d’utilisateurs ou de thématiques suivis

À tout moment

### Déconnexion explicite

Réponse : attente de l’accord du serveur.




Pour référence/inspiration

https://medium.com/@ConsenSys/blockchain-underpinnings-2c43ba03ecc9

## Références
[^t]: https://twitter.com/
[^msgpack]: https://msgpack.org/
[^json]: http://json.org/
[^jrpc]: https://www.jsonrpc.org/specification
