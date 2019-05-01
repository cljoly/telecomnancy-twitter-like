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

	
.PHONY: $(TOPTARGETS) $(SUBDIRS)
