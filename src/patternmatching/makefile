#--------------------------------------------------------------------------
# Benchmark makefile.
#
# Created on: 2011-02-28
# Author: Abdullah Gharaibeh
#--------------------------------------------------------------------------
TARGET  = patternmatching
ROOTDIR = ..
include $(ROOTDIR)/make.defs

DEPS := $(DEPS) $(TOTEM_DEPS)
INC  := $(INC) -I$(TOTEM_PATH)
LIBS = -l$(TARGET) -ltotem -L$(LIBDIR)

all: $(BINDIR)/$(TARGET)

$(BINDIR)/$(TARGET): $(BINDIR) lib libtotem 
	@printf "\nLinking %s executable ...\n" $(BINDIR)/$(TARGET)
	$(NVCC) -o $(OBJDIR)/$(TARGET).cuo -dlink $(CUBINOBJS) $(LIBS) $(LNVCCFLAGS)
	$(CC) -o $@ $(CBINOBJS) $(CCBINOBJS) $(CUBINOBJS) $(OBJDIR)/$(TARGET).cuo $(LIBS) $(LFLAGS)

include $(ROOTDIR)/make.rules
