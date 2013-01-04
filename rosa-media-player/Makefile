
PREFIX=/usr/local

CONF_PREFIX=$(PREFIX)

DATA_PATH=$(PREFIX)/share/rosa-media-player
DOC_PATH=$(PREFIX)/share/doc/rosa-media-player
TRANSLATION_PATH=$(PREFIX)/share/rosa-media-player/translations
THEMES_PATH=$(PREFIX)/share/rosa-media-player/themes
SHORTCUTS_PATH=$(PREFIX)/share/rosa-media-player/shortcuts

#KDE_PREFIX=`kde-config --prefix`
KDE_PREFIX=$(PREFIX)

KDE_ICONS=$(KDE_PREFIX)/share/icons/hicolor/
KDE_APPLNK=$(KDE_PREFIX)/share/applications/

QMAKE=qmake
LRELEASE=lrelease

DEFS=DATA_PATH=\\\"$(DATA_PATH)\\\" \
     TRANSLATION_PATH=\\\"$(TRANSLATION_PATH)\\\" \
     DOC_PATH=\\\"$(DOC_PATH)\\\" THEMES_PATH=\\\"$(THEMES_PATH)\\\" \
     SHORTCUTS_PATH=\\\"$(SHORTCUTS_PATH)\\\"

ifdef KDE_SUPPORT

# KDE paths, change if necessary

KDE_INCLUDE_PATH=`kde-config --prefix`/include/
KDE_LIB_PATH=`kde-config --prefix`/lib/
KDE_LIBS=-lkio

QMAKE_OPTS=DEFINES+=KDE_SUPPORT INCLUDEPATH+=$(KDE_INCLUDE_PATH) \
           LIBS+="$(KDE_LIBS) -L$(KDE_LIB_PATH)"

endif

src/rosa-media-player:
#	./get_svn_revision.sh
#	./get_romp_version.sh
	+cd src && $(QMAKE) "QMAKE_CFLAGS+=$(CFLAGS)" "QMAKE_CXXFLAGS+=$(CXXFLAGS)" "QMAKE_LFLAGS+=$(LDFLAGS)" $(QMAKE_OPTS) && $(DEFS) make
	cd src && $(LRELEASE) rosa-media-player.pro

clean:
	if [ -f src/Makefile ]; then cd src && make distclean; fi
	-rm src/translations/rosamp_*.qm

install: src/rosa-media-player
	-install -d $(DESTDIR)$(PREFIX)/bin/
	install -m 755 src/rosa-media-player $(DESTDIR)$(PREFIX)/bin/
	-install -d $(DESTDIR)$(DATA_PATH)
	install -m 644 src/input.conf $(DESTDIR)$(DATA_PATH)
	-install -d $(DESTDIR)$(TRANSLATION_PATH)
	install -m 644 src/translations/*.qm $(DESTDIR)$(TRANSLATION_PATH)
	-install -d $(DESTDIR)$(DOC_PATH)
	install -m 644 *.txt src/docs/* $(DESTDIR)$(DOC_PATH)
	-install -d $(DESTDIR)$(DOC_PATH)
	tar -C docs/ --exclude=.svn -c -f - . | tar -C $(DESTDIR)$(DOC_PATH) -x -f -

	-install -d $(DESTDIR)$(SHORTCUTS_PATH)
	cp src/shortcuts/* $(DESTDIR)$(SHORTCUTS_PATH)

	-install -d $(DESTDIR)$(KDE_ICONS)/128x128/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/96x96/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/72x72/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/64x64/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/32x32/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/22x22/apps/
	-install -d $(DESTDIR)$(KDE_ICONS)/16x16/apps/
	install -m 644 icons/rosamp_icon128.png $(DESTDIR)$(KDE_ICONS)/128x128/apps/rosamp.png
	install -m 644 icons/rosamp_icon96.png $(DESTDIR)$(KDE_ICONS)/96x96/apps/rosamp.png
	install -m 644 icons/rosamp_icon72.png $(DESTDIR)$(KDE_ICONS)/72x72/apps/rosamp.png
	install -m 644 icons/rosamp_icon64.png $(DESTDIR)$(KDE_ICONS)/64x64/apps/rosamp.png
	install -m 644 icons/rosamp_icon32.png $(DESTDIR)$(KDE_ICONS)/32x32/apps/rosamp.png
	install -m 644 icons/rosamp_icon22.png $(DESTDIR)$(KDE_ICONS)/22x22/apps/rosamp.png
	install -m 644 icons/rosamp_icon16.png $(DESTDIR)$(KDE_ICONS)/16x16/apps/rosamp.png
	-install -d $(DESTDIR)$(KDE_APPLNK)
	install -m 644 rosa-media-player.desktop $(DESTDIR)$(KDE_APPLNK)
	install -m 644 rosamp_enqueue.desktop $(DESTDIR)$(KDE_APPLNK)

uninstall:
	-rm -f $(PREFIX)/bin/rosa-media-player
	-rm -f $(DATA_PATH)/input.conf
	-rm -f $(TRANSLATION_PATH)/*.qm
	-rm -f $(DOC_PATH)/*.txt
	-rm -f $(DOC_PATH)/*.html
	-rm -f $(DOC_PATH)/images/*
	-rm -f $(SHORTCUTS_PATH)/*.keys
	-rm -f $(KDE_ICONS)/128x128/apps/rosamp.png
	-rm -f $(KDE_ICONS)/96x96/apps/rosamp.png
	-rm -f $(KDE_ICONS)/72x72/apps/rosamp.png
	-rm -f $(KDE_ICONS)/64x64/apps/rosamp.png
	-rm -f $(KDE_ICONS)/32x32/apps/rosamp.png
	-rm -f $(KDE_ICONS)/22x22/apps/rosamp.png
	-rm -f $(KDE_ICONS)/16x16/apps/rosamp.png
	-rm -f $(KDE_APPLNK)/rosa-media-player.desktop
	-rmdir $(SHORTCUTS_PATH)/
	-rmdir $(TRANSLATION_PATH)/
#	-for file in docs/*/*; do \
#	    rm -f $(DOC_PATH)/$${file/docs/}; \
#	done;
#	-for file in docs/*; do \
#	    rmdir $(DOC_PATH)/$${file/docs/}; \
#	done;
	-(cd docs && find -iname '*.html') | (cd $(DESTDIR)$(DOC_PATH) && xargs rm)
	-(cd docs && find -type d -name '??') | (cd $(DESTDIR)$(DOC_PATH) && xargs rmdir)
	-rmdir $(DOC_PATH)/
	-rmdir $(DATA_PATH)/

