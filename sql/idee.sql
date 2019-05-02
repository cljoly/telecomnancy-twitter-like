INSERT OR IGNORE INTO bookmarks(users_id, lessoninfo_id) VALUES(123, 456)

INSERT INTO EVENTTYPE (EventTypeName)
SELECT 'ANI Received'
WHERE NOT EXISTS (SELECT 1 FROM EVENTTYPE WHERE EventTypeName = 'ANI Received');

-- Pour les abonnements
UNIQUE
ORDER BY date

-- Session dbeaver pour établir get_gazou sans retweet

SELECT * FROM gazou
WHERE author IN (SELECT followed FROM user_subscription WHERE follower = 'me');
--OR tag IN (SELECT tag FROM tag_subscription WHERE follower = 'me');

SELECT * FROM gazou, gazou_tag
WHERE gazou.id = gazou_tag.gazou_id;

SELECT * FROM gazou
LEFT JOIN gazou_tag ON gazou.id = gazou_tag.gazou_id;

SELECT * FROM gazou, gazou_tag
WHERE
	gazou.id = gazou_tag.gazou_id
AND (
	author IN (SELECT followed FROM user_subscription WHERE follower = 'me')
);

-- Solution
SELECT DISTINCT id FROM gazou
LEFT JOIN gazou_tag ON gazou.id = gazou_tag.gazou_id	
WHERE author IN (SELECT followed FROM user_subscription WHERE follower = 'me')
OR tag IN (SELECT tag FROM tag_subscription WHERE follower = 'me')
ORDER BY date
LIMIT 20;
-- Puis itérer sur chaque message pour compléter l’objet JSON

-- Avec retweet
SELECT DISTINCT id FROM gazou
LEFT JOIN gazou_tag ON gazou.id = gazou_tag.gazou_id
LEFT JOIN relay ON gazou.id = relay.gazou_id
WHERE author IN (SELECT followed FROM user_subscription WHERE follower = 'me')
OR tag IN (SELECT tag FROM tag_subscription WHERE follower = 'me')
OR retweeter IN (SELECT followed FROM user_subscription WHERE follower = 'me')
ORDER BY date
LIMIT 20;


-- Version avec les tweet à NULL
SELECT * 
FROM gazou 
LEFT JOIN gazou_tag ON gazou.id = gazou_tag.gazou_id 
INNER JOIN relay ON gazou.id = relay.gazou_id 
WHERE
	author IN (SELECT followed FROM user_subscription WHERE follower = 'tutu')
	OR tag IN (SELECT tag FROM tag_subscription WHERE follower = 'tutu')
	OR retweeter IN (SELECT followed FROM user_subscription WHERE follower = 'tutu')
UNION
SELECT *, NULL, NULL 
FROM gazou 
LEFT JOIN gazou_tag ON gazou.id = gazou_tag.gazou_id
WHERE
	author IN (SELECT followed FROM user_subscription WHERE follower = 'tutu')
	OR tag IN (SELECT tag FROM tag_subscription WHERE follower = 'tutu')
ORDER BY date DESC
LIMIT 20
;
