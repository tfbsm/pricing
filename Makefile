MAKEFLAGS += --no-print-directory

build_path := build

override cores:=$(shell if [ "$$(uname)" = "Darwin" ]; then sysctl -n hw.ncpu; elif [ "$$(uname)" = "Linux" ]; then nproc --all; else echo 1; fi)

# All targets are phony in this Makefile
.PHONY : $(shell egrep "^[A-Za-z0-9_-]+\:([^\=]|$$)" $(lastword $(MAKEFILE_LIST)) | sed -E 's/(.*):.*/\1/g')

prebuild:
	@mkdir -p ./$(build_path)

all: build

clean:
	@rm -rf ./$(build_path)/*

configure: prebuild
	cmake -S . -B $(build_path)

format: configure
	cmake --build $(build_path) --target fix-format

build: configure format
	cmake --build $(build_path) -- -j $(cores)

run-test-app: build
	@./$(build_path)/test_app
