CMAKE ?= cmake
CFLAGS = -Wall -Wextra
GLFW_CMAKE_FLAGS = -D GLFW_BUILD_TESTS=0 -D GLFW_BUILD_EXAMPLES=0 

SRCDIR = ./src
GLAD_SRCDIR = ./third-party/glad
GLFW_SRCDIR = ./third-party/glfw
BUILDDIR = ./build
# No rules needed, cmake will automatically create the build dirs 
GLAD_BUILDDIR = $(BUILDDIR)/glad
GLFW_BUILDDIR = $(BUILDDIR)/glfw
GLAD_LIBRARY = $(GLAD_BUILDDIR)/libglad.a 
GLFW_LIBRARY = $(GLFW_BUILDDIR)/src/libglfw3.a 
LDFLAGS = -lGL -L$(GLAD_BUILDDIR) -L$(GLFW_BUILDDIR)/src -lglfw3 -lglad -lX11 -lm
# USE_WAYLAND is set through the command line 
ifeq ($(USE_WAYLAND), 1)
	GLFW_CMAKE_FLAGS += -D GLFW_USE_WAYLAND=1
endif
INCLUDE_PATHS = -I$(GLAD_SRCDIR)/include -I$(GLFW_SRCDIR)/include
# Now, unlike the library build directories above, $(OBJDIR) actually needs a rule to be made 
OBJDIR = $(BUILDDIR)/objs
vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)
SRCFILES = main.c
HEADERFILES = moltenterm.h
OBJFILES = $(SRCFILES:%.c=$(OBJDIR)/%.o)
EXECDIR = ./bin
SIGHTERM = sighterm

all: $(EXECDIR)/$(SIGHTERM)

$(EXECDIR)/$(SIGHTERM): $(GLAD_LIBRARY) $(GLFW_LIBRARY) $(HEADERFILES) $(OBJFILES) | $(EXECDIR)
	$(CC) $(OBJFILES) -o $@ $(LDFLAGS)

$(EXECDIR) $(OBJDIR) $(BUILDDIR):
	mkdir -p $@

$(GLAD_LIBRARY): 
	$(CMAKE) -S $(GLAD_SRCDIR) -B $(GLAD_BUILDDIR)
	cd $(GLAD_BUILDDIR) && $(MAKE)

$(GLFW_LIBRARY):
	$(CMAKE) -S $(GLFW_SRCDIR) -B $(GLFW_BUILDDIR) $(GLFW_CMAKE_FLAGS)
	cd $(GLFW_BUILDDIR) && $(MAKE)


$(OBJDIR)/%.o: %.c | $(OBJDIR) 
	$(CC) $(CFLAGS) $(INCLUDE_PATHS) -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(EXECDIR) $(GLFW_LIBRARY) $(GLAD_LIBRARY) $(GLAD_BUILDDIR) $(GLFW_BUILDDIR) $(BUILDDIR)

.PHONY: all clean
