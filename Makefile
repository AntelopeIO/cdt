DEV_DOCKER_IMAGE=waxteam/cdt-dev:v4.0.1wax01-v4.0.0
DEV_DOCKER_CONTAINER=waxcdt-dev
DEV_DOCKER_COMMON=-v `pwd`:/tmp/cdt \
			--name $(DEV_DOCKER_CONTAINER) -w /tmp/cdt $(DEV_DOCKER_IMAGE)
nproc := $(shell grep -c ^processor /proc/cpuinfo)

get-latest:
	docker pull $(DEV_DOCKER_IMAGE)

.PHONY: safe-git-path
safe-git-path:
	git config --global --add safe.directory `pwd`

build: safe-git-path
	-mkdir build && \
	cd build && \
	cmake -DBOOST_ROOT=${HOME}/boost1.79 .. && \
	make -j $(nproc)

.PHONY: compile
rebuild: safe-git-path
	cd build && make -j$(nproc)

.PHONY: clean
clean:
	-rm -rf build

.PHONY: test
test: build
	cd build && ctest

.PHONY: test-integration
test-integration: build
	cd build/tests/integration && ctest

.PHONY:dev-docker-stop
dev-docker-stop:
	-docker rm -f $(DEV_DOCKER_CONTAINER)

.PHONY:dev-docker-start
dev-docker-start: dev-docker-stop get-latest
	docker run -it $(DEV_DOCKER_COMMON) bash

# Useful for travis CI
.PHONY:dev-docker-all
dev-docker-all: dev-docker-stop get-latest
	docker run $(DEV_DOCKER_COMMON) bash -c "make clean test"