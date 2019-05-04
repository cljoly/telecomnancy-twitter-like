TOPTARGETS := all clean

SUBDIRS := $(wildcard src/*/.)

$(TOPTARGETS): $(SUBDIRS) db.sqlite3
$(SUBDIRS):
	$(MAKE) -C $@ $(MAKECMDGOALS)

# Script d’initialisation de la base de donnée
db.sqlite3: init_db.sql
	sqlite3 db.sqlite3 <init_db.sql

clean_all:
	rm db.sqlite3 src/client/client src/server/server
	make clean

rapport:
	pandoc -i rapport.md spec_protocole.md -V geometry:"top=2.5cm, bottom=2.5cm, left=2.5cm, right=2.5cm" -o rapport.pdf
	
.PHONY: $(TOPTARGETS) $(SUBDIRS) rapport
