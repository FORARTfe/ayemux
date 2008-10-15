fonts 	        = fonts
images 	        = images
CC		= edje_cc
SRC		= main.edc
OUT		= detour-etk.edj
COMPILE 	= $(CC) $(EDJE_CC_FLAGS)
PATH_INSTALL    = $(HOME)/.e/etk/themes

EDJE_CC_FLAGS = -fd $(fonts)/ -id $(images)/

.SILENT :

all: checkdir clean build install

checkdir:
	if [ ! -d $(PATH_INSTALL) ]; then mkdir -p $(PATH_INSTALL); fi

clean:
	@echo ""
	@echo "make clean"
	if [ -e $(OUT) ]; then rm $(OUT); fi

build: main.edc
	@echo "make"
	$(COMPILE) $(SRC) -o $(OUT)

install: detour-etk.edj
	@echo "make install"
	if [ -e $(OUT) ]; then cp $? $(PATH_INSTALL); fi
	@echo ""
	@echo --------------------------------------------------
	@echo $(OUT) was installed in your
	@echo $(PATH_INSTALL)/ directory.
	@echo --------------------------------------------------
	@echo ""

uninstall: detour-etk.edj
	@echo
	@echo Removing $(OUT)
	if [ -e $(PATH_INSTALL)/$(OUT) ]; then rm $(PATH_INSTALL)/$(OUT); fi
	@echo ""
	@echo --------------------------------------------------
	@echo $(OUT) was removed from your
	@echo $(HOME)/e/etk/themes/ directory.
	@echo --------------------------------------------------
	@echo ""
