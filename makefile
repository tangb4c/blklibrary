

# Make command to use for dependencies
MAKE=make
RM=rm
MKDIR=mkdir

# -----Begin user-editable area-----
#include $(PATH_QZONEPLATFORM_PRJ)/plib/makefile.plib
QZONEBASE_INC=-I$(PATH_QZONEPLATFORM_BASE_CLASS_PRJ)/include
# -----End user-editable area-----

#
# Configuration: lib
#
OUTDIR=lib
OUTFILE=$(OUTDIR)/libblkbase.a
CFG_INC=-Iinclude $(INC_PDU) $(INC_LOGAPI) $(INC_CSOCKET)
CFG_LIB=lib/libblkbase.a
CFG_OBJ=
COMMON_OBJ=$(OUTDIR)/comm_base.o $(OUTDIR)/comm_config_helper.o \
	$(OUTDIR)/comm_configdocument.o $(OUTDIR)/comm_delay.o \
	$(OUTDIR)/comm_info.o \
	$(OUTDIR)/comm_speed_stat.o $(OUTDIR)/comm_uins.o \
	$(OUTDIR)/comm_utility.o $(OUTDIR)/qza_protocol_parser.o \
	$(OUTDIR)/qzone_protocol_parser.o $(OUTDIR)/tcpclient.o \
	$(OUTDIR)/udpclient.o
OBJ=$(COMMON_OBJ) $(CFG_OBJ)
ALL_OBJ=$(OUTDIR)/comm_base.o $(OUTDIR)/comm_config_helper.o \
	$(OUTDIR)/comm_configdocument.o $(OUTDIR)/comm_delay.o \
	$(OUTDIR)/comm_info.o \
	$(OUTDIR)/comm_speed_stat.o $(OUTDIR)/comm_uins.o \
	$(OUTDIR)/comm_utility.o $(OUTDIR)/qza_protocol_parser.o \
	$(OUTDIR)/qzone_protocol_parser.o $(OUTDIR)/tcpclient.o \
	$(OUTDIR)/udpclient.o \
	lib/libblkbase.a

COMPILE=g++ -c  -pipe  -O2 -g -Wall -fPIC -o "$(OUTDIR)/$(*F).o" $(CFG_INC) $<
LINK=ar -rs  "$(OUTFILE)" $(OBJ)

# Pattern rules
$(OUTDIR)/%.o : comm/%.cpp
	$(COMPILE)

$(OUTDIR)/%.o : comm/%.cpp
	$(COMPILE)

# Build rules
all: prebuildcmds $(OUTFILE)

prebuildcmds:
	#$(PATH_QZONEPLATFORM_PRJ)/makeinclude/make_check .

$(OUTFILE): $(OUTDIR)  $(OBJ)
	$(LINK)

$(OUTDIR):
	$(MKDIR) -p "$(OUTDIR)"

# Rebuild this project
rebuild: cleanall all

# Clean this project
clean:
	$(RM) -f $(OUTFILE)
	$(RM) -f $(OBJ)

# Clean this project and all dependencies
cleanall: clean
