CMAKE ?= cmake
CFLAGS = -Wall -Wextra
LDFLAGS = -lGL
GLFW_CMAKE_FLAGS = -D GLFW_BUILD_TESTS=0 -D GLFW_BUILD_EXAMPLES=0 
# USE_WAYLAND is set through the command line 
ifeq ($(USE_WAYLAND), 1)
	GLFW_CMAKE_FLAGS += -D GLFW_USE_WAYLAND=1
endif
SRCDIR = ./src
GLAD_SRCDIR = ./third-party/glad
GLFW_SRCDIR = ./third-party/glfw
# No rules needed, cmake will automatically create the build dirs 
GLAD_BUILDDIR = ./build/glad
GLFW_BUILDDIR = ./build/glfw
GLAD_LIBRARY = $(GLAD_BUILDDIR)/libglad.a 
GLFW_LIBRARY = $(GLFW_BUILDDIR)/src/libglfw3.a 
# Now, unlike the library build directories above, $(OBJDIR) actually needs a rule to be made 
OBJDIR = ./build/objs
vpath %.c %.h $(SRCDIR)
SRCFILES = main.c
OBJFILES = $(SRCFILES:%.c=$(OBJDIR)/%.o)
SIGHTERM = ./build/sighterm

all: $(SIGHTERM)

$(SIGHTERM): $(GLAD_LIBRARY) $(GLFW_LIBRARY) $(OBJFILES) | $(EXECDIR)
	$(CC) $(LDFLAGS) $(GLAD_LIBRARY) $(GLFW_LIBRARY) $(OBJFILES) -o $@

$(OBJDIR):
	mkdir -p $@

$(GLAD_LIBRARY): 
	$(CMAKE) -S $(GLAD_SRCDIR) -B $(GLAD_BUILDDIR)
	cd $(GLAD_BUILDDIR) && $(MAKE)

$(GLFW_LIBRARY):
	$(CMAKE) -S $(GLFW_SRCDIR) -B $(GLFW_BUILDDIR) $(GLFW_CMAKE_FLAGS)
	cd $(GLFW_BUILDDIR) && $(MAKE)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
