BEGIN TRANSACTION;
CREATE TABLE IF NOT EXISTS "tag_subscription" (
	"tag"	TEXT,
	"follower"	TEXT,
	UNIQUE("follower","tag"),
	FOREIGN KEY("follower") REFERENCES "user"("name")
);
CREATE TABLE IF NOT EXISTS "user_subscription" (
	"followed"	TEXT,
	"follower"	TEXT,
	UNIQUE("follower","followed"),
	FOREIGN KEY("follower") REFERENCES "user"("name"),
	FOREIGN KEY("followed") REFERENCES "user"("name")
);
CREATE TABLE IF NOT EXISTS "gazou_tag" (
	"gazou_id"	INT,
	"tag"	TEXT,
	UNIQUE("gazou_id","tag"),
	FOREIGN KEY("gazou_id") REFERENCES "gazou"("id"),
	FOREIGN KEY("tag") REFERENCES "tag"("name")
);
CREATE TABLE IF NOT EXISTS "tag" (
	"name"	TEXT UNIQUE,
	PRIMARY KEY("name")
);
CREATE TABLE IF NOT EXISTS "gazou" (
	"id"	INT,
	"date"	TEXT,
	"author"	TEXT,
	"content"	TEXT,
	UNIQUE("date","author","content"),
	PRIMARY KEY("id"),
	FOREIGN KEY("author") REFERENCES "user"("name")
);
CREATE TABLE IF NOT EXISTS "user" (
	"name"	TEXT UNIQUE,
	"password"	TEXT,
	"cookie"	INT UNIQUE,
	PRIMARY KEY("name")
);
INSERT INTO "tag_subscription" ("tag","follower") VALUES ('Nothing','me'),
 ('proud','me'),
 ('Morning','me'),
 ('JustSaying','me'),
 ('hello','me');
INSERT INTO "user_subscription" ("followed","follower") VALUES ('star','me'),
 ('me','a'),
 ('elysee','me'),
 ('elysee','star');
INSERT INTO "gazou_tag" ("gazou_id","tag") VALUES (0,'proud'),
 (2,'hello'),
 (3,'JustSaying'),
 (2,'Morning');
INSERT INTO "tag" ("name") VALUES ('proud'),
 ('hello'),
 ('JustSaying'),
 ('Morning');
INSERT INTO "gazou" ("id","date","author","content") VALUES (0,'2019-04-09 11:27:18','star','Je suis une star #proud'),
 (1,'2019-03-21 01:31:50','aaaaa','Retrouvez le communiqué de presse'),
 (2,'2019-01-16 13:16:32','me','Good #Morning Everyone , Happy Nice Day :D #hello'),
 (3,'2019-04-11 08:38:32','aaaaa','You should do a concert in Greece #JustSaying'),
 (4,'2019-03-24 15:46:43','elysee','Les Français attendent de l''État qu''il soit plus proche de leurs réalités quotidiennes, et plus humain pour leur apporter des solutions concrètes');
INSERT INTO "user" ("name","password","cookie") VALUES ('a','a',482706814),
 ('star','super_password',750623043),
 ('aaaaa','a',1771855935),
 ('me','auie',1),
 ('elysee','palais',83247);
COMMIT;
