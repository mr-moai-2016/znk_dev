# Source directory
S = .
# Root path of common libraries
MY_LIBS_ROOT=..
ifndef MKFSYS_DIR
  MKFSYS_DIR=$(MY_LIBS_ROOT)/../mkfsys
endif

GCC_CMD=gcc
PLATFORM=win32
ifeq ($(MACHINE), x64)
  PLATFORM=win64
else
  MACHINE=x86
  PLATFORM=win32
endif
# Output directory
ABINAME=mingw$(MACHINE)$(DEBUG)
O = ./out_dir/$(ABINAME)

ifeq ($(DEBUG), d)
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -g
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)_mingwd
SLIBS_DIR=slib/$(PLATFORM)_mingwd
else
COMPILER=$(GCC_CMD) -Wall -Wstrict-aliasing=2 -O2 -fno-strict-aliasing -Wno-uninitialized -DNDEBUG
LINKER=$(GCC_CMD)
DLIBS_DIR=dlib/$(PLATFORM)
SLIBS_DIR=slib/$(PLATFORM)
endif

CP=cp

INCLUDE_FLAG+=  \
	-I$(MY_LIBS_ROOT)/libZnk \
	-I$(MY_LIBS_ROOT)/libRano \


include Makefile_version.mak

BASENAME0=easter
EXE_FILE0=$O/easter.cgi
OBJS0=\
	$O/Est_assort.o \
	$O/Est_assort_list.o \
	$O/Est_assort_ui.o \
	$O/Est_base.o \
	$O/Est_bat_operator.o \
	$O/Est_box.o \
	$O/Est_boxmap_viewer.o \
	$O/Est_box_base.o \
	$O/Est_box_ui.o \
	$O/Est_cinf_ui.o \
	$O/Est_config.o \
	$O/Est_dir_util.o \
	$O/Est_filter.o \
	$O/Est_finf.o \
	$O/Est_get.o \
	$O/Est_hint_manager.o \
	$O/Est_img_viewer.o \
	$O/Est_linf_list.o \
	$O/Est_link.o \
	$O/Est_link_manager.o \
	$O/Est_parser.o \
	$O/Est_post.o \
	$O/Est_recentory.o \
	$O/Est_record.o \
	$O/Est_search_manager.o \
	$O/Est_sqi.o \
	$O/Est_tag.o \
	$O/Est_tag_manager.o \
	$O/Est_topic.o \
	$O/Est_ui.o \
	$O/Est_ui_base.o \
	$O/Est_unid.o \
	$O/main.o \

BASENAME1=cache_task
EXE_FILE1=$O/cache_task.cgi
OBJS1=\
	$O/Est_assort.o \
	$O/Est_assort_list.o \
	$O/Est_assort_ui.o \
	$O/Est_base.o \
	$O/Est_bat_operator.o \
	$O/Est_box.o \
	$O/Est_boxmap_viewer.o \
	$O/Est_box_base.o \
	$O/Est_box_ui.o \
	$O/Est_cinf_ui.o \
	$O/Est_config.o \
	$O/Est_dir_util.o \
	$O/Est_filter.o \
	$O/Est_finf.o \
	$O/Est_get.o \
	$O/Est_hint_manager.o \
	$O/Est_img_viewer.o \
	$O/Est_linf_list.o \
	$O/Est_link.o \
	$O/Est_link_manager.o \
	$O/Est_parser.o \
	$O/Est_post.o \
	$O/Est_recentory.o \
	$O/Est_record.o \
	$O/Est_search_manager.o \
	$O/Est_sqi.o \
	$O/Est_tag.o \
	$O/Est_tag_manager.o \
	$O/Est_topic.o \
	$O/Est_ui.o \
	$O/Est_ui_base.o \
	$O/Est_unid.o \
	$O/cache_task.o \

BASENAME2=import_favorite
EXE_FILE2=$O/import_favorite.cgi
OBJS2=\
	$O/Est_assort.o \
	$O/Est_assort_list.o \
	$O/Est_assort_ui.o \
	$O/Est_base.o \
	$O/Est_bat_operator.o \
	$O/Est_box.o \
	$O/Est_boxmap_viewer.o \
	$O/Est_box_base.o \
	$O/Est_box_ui.o \
	$O/Est_cinf_ui.o \
	$O/Est_config.o \
	$O/Est_dir_util.o \
	$O/Est_filter.o \
	$O/Est_finf.o \
	$O/Est_get.o \
	$O/Est_hint_manager.o \
	$O/Est_img_viewer.o \
	$O/Est_linf_list.o \
	$O/Est_link.o \
	$O/Est_link_manager.o \
	$O/Est_parser.o \
	$O/Est_post.o \
	$O/Est_recentory.o \
	$O/Est_record.o \
	$O/Est_search_manager.o \
	$O/Est_sqi.o \
	$O/Est_tag.o \
	$O/Est_tag_manager.o \
	$O/Est_topic.o \
	$O/Est_ui.o \
	$O/Est_ui_base.o \
	$O/Est_unid.o \
	$O/import_favorite.o \

SUB_LIBS=\

PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE0) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE1) \
PRODUCT_EXECS= \
	__mkg_sentinel_target__ \
	$(EXE_FILE2) \

RUNTIME_FILES= \
	__mkg_sentinel_target__ \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Znk-$(DL_VER).dll \
	$(MY_LIBS_ROOT)/$(DLIBS_DIR)/Rano-$(DL_VER).dll \



# Entry rule.
all: $O $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2) 

# Mkdir rule.
$O:
	mkdir -p $O


# Product files rule.
$(EXE_FILE0): $(OBJS0)
	$(LINKER) -o $(EXE_FILE0) $(OBJS0) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 

$(EXE_FILE1): $(OBJS1)
	$(LINKER) -o $(EXE_FILE1) $(OBJS1) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 

$(EXE_FILE2): $(OBJS2)
	$(LINKER) -o $(EXE_FILE2) $(OBJS2) $(SUB_LIBS) -Wl,-rpath,. $(MY_LIBS_ROOT)/libZnk/out_dir/$(ABINAME)/libZnk-$(DL_VER).dll.a $(MY_LIBS_ROOT)/libRano/out_dir/$(ABINAME)/libRano-$(DL_VER).dll.a -lws2_32 


##
# Pattern rule.
# We use not suffix rule but pattern rule for dealing flexibly with files in sub-directory.
# In this case, there is very confusing specification, that is :
# '\' to the left hand of ':' works as escape sequence, 
# '\' to the right hand of ':' does not work as escape sequence. 
# Hence, we have to duplicate '\' to the left hand of ':',
# the other way, '\' to the right hand of ':' we have to put only single '\',
# for example $O\\%.o: $S\%.c .
#
$O/%.o: $S/%.c
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<

$O/%.o: $S/%.cpp
	$(COMPILER) -I$S $(INCLUDE_FLAG) -o $@ -c $<


# Rc rule.

# Submkf rule.

# Dummy rule.
__mkg_sentinel_target__:

# Install data rule.
install_data:
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/default 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/templates 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/icons 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/android 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/iphone 
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/agree.5ch.net/js 
	for tgt in default/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/default/ ; fi ; done
	for tgt in templates/*.html ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/templates/ ; fi ; done
	for tgt in templates/*.myf ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/templates/ ; fi ; done
	for tgt in publicbox/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/ ; fi ; done
	for tgt in publicbox/icons/*.png ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/icons/ ; fi ; done
	for tgt in publicbox/alternative/5ch/itest.5ch.net/assets/js/android/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/android/ ; fi ; done
	for tgt in publicbox/alternative/5ch/itest.5ch.net/assets/js/iphone/* ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/itest.5ch.net/assets/js/iphone/ ; fi ; done
	for tgt in publicbox/alternative/5ch/agree.5ch.net/js/*.js ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/publicbox/alternative/5ch/agree.5ch.net/js/ ; fi ; done

# Install exec rule.
install_exec: $(EXE_FILE0) $(EXE_FILE1) $(EXE_FILE2)
	mkdir -p ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter 
	for tgt in $(EXE_FILE0) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done
	for tgt in $(EXE_FILE1) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done
	for tgt in $(EXE_FILE2) ; do if test -e "$$tgt" ; then $(CP) "$$tgt" ../../moai-v$(REL_VER)-$(PLATFORM)/cgis/easter/ ; fi ; done

# Install dlib rule.
install_dlib:

# Install slib rule.
install_slib:

# Install rule.
install: all install_exec install_data 


# Clean rule.
clean:
	rm -r $O/ 

# Src and Headers Dependency
cache_task.o: Est_search_manager.h Est_config.h Est_base.h Est_sqi.h Est_recentory.h Est_box.h
Est_assort.o: Est_assort.h Est_config.h Est_ui.h Est_recentory.h Est_base.h Est_dir_util.h Est_tag.h
Est_assort_list.o: Est_assort_list.h Est_config.h Est_ui.h Est_box.h Est_base.h Est_assort.h Est_assort_ui.h
Est_assort_ui.o: Est_assort_ui.h Est_config.h Est_ui_base.h Est_unid.h
Est_base.o: Est_base.h Est_config.h
Est_bat_operator.o: Est_bat_operator.h
Est_box.o: Est_box.h Est_finf.h Est_recentory.h Est_bat_operator.h Est_assort.h Est_dir_util.h Est_tag.h Est_config.h Est_sqi.h Est_search_manager.h
Est_boxmap_viewer.o: Est_boxmap_viewer.h Est_config.h Est_ui.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_tag.h Est_bat_operator.h Est_box.h Est_box_ui.h Est_assort_list.h
Est_box_base.o: Est_box_base.h Est_config.h Est_base.h
Est_box_ui.o: Est_box_ui.h Est_config.h Est_ui_base.h Est_cinf_ui.h Est_linf_list.h
Est_cinf_ui.o: Est_cinf_ui.h Est_base.h
Est_config.o: Est_config.h Est_base.h Est_hint_manager.h Est_unid.h
Est_dir_util.o: Est_dir_util.h
Est_filter.o: Est_filter.h Est_link.h Est_config.h Est_parser.h Est_base.h Est_hint_manager.h
Est_finf.o: Est_finf.h Est_record.h
Est_get.o: Est_get.h Est_config.h Est_link.h Est_base.h Est_filter.h Est_img_viewer.h
Est_hint_manager.o: Est_hint_manager.h
Est_img_viewer.o: Est_img_viewer.h Est_config.h Est_ui.h Est_ui_base.h Est_base.h Est_dir_util.h Est_assort.h Est_assort_ui.h Est_tag.h Est_finf.h Est_hint_manager.h
Est_linf_list.o: Est_linf_list.h Est_base.h
Est_link.o: Est_link.h Est_parser.h Est_config.h
Est_link_manager.o: Est_link_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_base.h Est_linf_list.h Est_box_ui.h
Est_parser.o: Est_parser.h
Est_post.o: Est_post.h Est_base.h Est_config.h Est_ui.h Est_hint_manager.h
Est_recentory.o: Est_recentory.h Est_search_manager.h Est_finf.h Est_unid.h
Est_record.o: Est_record.h Est_base.h
Est_search_manager.o: Est_search_manager.h Est_boxmap_viewer.h Est_ui.h Est_box_ui.h Est_record.h Est_config.h Est_assort.h Est_assort_ui.h Est_hint_manager.h Est_base.h Est_finf.h Est_box_base.h Est_unid.h
Est_sqi.o: Est_sqi.h Est_tag.h
Est_tag.o: Est_tag.h Est_unid.h
Est_tag_manager.o: Est_tag_manager.h Est_config.h Est_ui.h Est_unid.h Est_hint_manager.h Est_assort_ui.h Est_assort.h Est_tag.h Est_base.h
Est_topic.o: Est_topic.h Est_config.h Est_ui.h Est_ui_base.h Est_box.h Est_box_ui.h Est_base.h Est_assort.h Est_assort_ui.h Est_search_manager.h Est_recentory.h Est_finf.h Est_assort_list.h
Est_ui.o: Est_ui.h Est_ui_base.h Est_config.h Est_assort.h Est_base.h Est_box_ui.h
Est_ui_base.o: Est_ui_base.h Est_box_base.h
Est_unid.o:
import_favorite.o: Est_config.h Est_base.h Est_assort.h
main.o: Est_config.h Est_get.h Est_post.h Est_filter.h Est_link_manager.h Est_boxmap_viewer.h Est_search_manager.h Est_img_viewer.h Est_topic.h Est_tag_manager.h
