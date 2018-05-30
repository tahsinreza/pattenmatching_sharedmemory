#--------------------------------------------------------------------------
# Benchmark makefile.
#
# Created on: 2011-02-28
# Author: Abdullah Gharaibeh
#--------------------------------------------------------------------------
TARGET  = patternmatching
ROOTDIR = ..
include $(ROOTDIR)/make.defs

DEPS := $(DEPS) $(TOTEM_DEPS) $(PATTERNMATCHING_DEPS)
INC  := $(INC) -I$(TOTEM_PATH)
LIBS = -l$(TARGET) -ltotem -L$(LIBDIR)

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(BINDIR) lib libtotem libpatternmatching
	@printf "\nLinking %s executable ...\n" $(BINDIR)/$(TARGET)
	$(NVCC) -o $@ $(CBINOBJS) $(CCBINOBJS) $(CUBINOBJS) $(LIBS) $(LNVCCFLAGS)



include $(ROOTDIR)/make.rules
