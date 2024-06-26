PROJECT_NAME     := qlight
SOURCE_DIRECTORY := src
BUILD_DIRECTORY  := build
OUTPUT_DIRECTORY := $(BUILD_DIRECTORY)/makefile_$(PROJECT_NAME)_Debug_x64
OBJECT_DIRECTORY := $(OUTPUT_DIRECTORY)_obj

INCLUDE_PATHS_BY_SPACE := libs
LIBRARY_PATHS_BY_SPACE := libs/GLFW/linux
LIBRARIES_BY_SPACE     := glfw3 vulkan


OUTPUT_EXECUTABLE := $(OUTPUT_DIRECTORY)/$(PROJECT_NAME)
SOURCE_FILES      = $(wildcard $(SOURCE_DIRECTORY)/*.cpp)
SOURCE_FILES_TEMP := $(SOURCE_FILES)
SOURCE_FILES      = $(filter-out $(SOURCE_DIRECTORY)/platform_windows.cpp, $(SOURCE_FILES_TEMP))
OBJECT_FILES      = $(SOURCE_FILES:$(SOURCE_DIRECTORY)/%.cpp=$(OBJECT_DIRECTORY)/%.o)
OBJECT_FILES_TEMP := $(OBJECT_FILES)
OBJECT_FILES      = $(OBJECT_FILES_TEMP:%platform_linux.o=%platform.o)


INCLUDE_PATHS := $(foreach include_path, $(INCLUDE_PATHS_BY_SPACE), -I $(include_path))
LIBRARY_PATHS := $(foreach library_path, $(LIBRARY_PATHS_BY_SPACE), -L $(library_path))
LIBRARIES     := $(foreach library, $(LIBRARIES_BY_SPACE), -l $(library))


CPP_COMPILER        := clang++
CPP_COMPILER_FLAGS  := -D QLIGHT_DEBUG
PREPROCESSOR_FLAGS  := $(INCLUDE_PATHS) -MMD -MP
LD_LINKER_FLAGS     := $(LIBRARY_PATHS) -g -gdwarf
LD_LINKER_LIBRARIES := $(LIBRARIES)


.PHONY: debug
debug: $(OUTPUT_EXECUTABLE)

$(OUTPUT_EXECUTABLE): $(OBJECT_FILES) | $(OBJECT_DIRECTORY) $(OUTPUT_DIRECTORY) copy_resources
	@echo "Source Directory: $(SOURCE_DIRECTORY)"
	@echo "Source Files: $(SOURCE_FILES)"
	@echo "Object Files: $(OBJECT_FILES)"
	$(CPP_COMPILER) $(LD_LINKER_FLAGS) $^ $(LD_LINKER_LIBRARIES) -o $@

$(OBJECT_DIRECTORY) $(BUILD_DIRECTORY) $(OUTPUT_DIRECTORY):
	mkdir -p $@

$(OBJECT_DIRECTORY)/platform.o: $(SOURCE_DIRECTORY)/platform_linux.cpp $(OBJECT_DIRECTORY)
	$(CPP_COMPILER) $(PREPROCESSOR_FLAGS) $(CPP_COMPILER_FLAGS) -c $< -o $@


$(OBJECT_DIRECTORY)/%.o: $(SOURCE_DIRECTORY)/%.cpp $(OBJECT_DIRECTORY)
	$(CPP_COMPILER) $(PREPROCESSOR_FLAGS) $(CPP_COMPILER_FLAGS) -c $< -o $@


.PHONY: copy_resources
copy_resources:
	cp -r resources $(OUTPUT_DIRECTORY)


.PHONY: clean
clean:
	rm -f $(OBJECT_DIRECTORY)/*.o
	rm -f $(OUTPUT_EXECUTABLE)

-include $(OBJECT_FILES:.o=.d)
