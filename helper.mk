#!/usr/bin/make -f
# -*- makefile -*-
# ex: set tabstop=4 noexpandtab:
# -*- coding: utf-8 -*

default: help all
	@date

SELF?=${CURDIR}/helper.mk

build_dir?=build

packages?=make cmake time file git sudo
packages+=build-essential pkg-config bison flex python
packages+=libusb-1.0-0-dev libssl-dev libxml2-dev libjson-c-dev
packages+=doxygen xsltproc plantuml
packages+=radvd parprouted bridge-utils net-tools zip unzip
# TODO: https://www.tcpdump.org/release/libpcap-1.5.3.tar.gz

PLANTUML_JAR_PATH?=/usr/share/plantuml/plantuml.jar
export PLANTUML_JAR_PATH

exe?=${CURDIR}/${build_dir}/files/zipgateway
# exe_args?=--help

all_rules?=all tests

help: ${SELF}
	grep -o '^[^ ]*:' ${SELF} | grep -v '\$$' | grep -v '^#'
	@echo "# PATH=${PATH}"

configure: ${build_dir}/CMakeCache.txt
	ls $<

${build_dir}/CMakeCache.txt: CMakeLists.txt
	mkdir -p ${@D}
	cmake --version
	cmake --help
	cd ${@D} && cmake ${cmake_options} ${CURDIR}
	ls ${CURDIR}/$@

.PHONY: build

build: ${build_dir}/CMakeCache.txt
	cmake --build ${build_dir}

all: ${all_rules}
	date -u

setup/debian/stretch: /etc/apt/sources.list
	sed -e 's|\(http://\)\(.*\)\(.debian.org\)|\1archive\3|g' -i $<
	sed -e 's|stretch-updates|stretch-proposed-updates|g' -i $<
	echo "deb http://archive.debian.org/debian stretch-backports main contrib non-free" \
| ${sudo} tee "$<.d/backports.list"
	${sudo} apt-get update
	${sudo} apt-get install -y ${packages}

setup/debian: setup/debian/stretch

/etc/apt/sources.list:
	@echo "error: Distro unsuported please use debian"
	exit 1

install: ${build_dir}
	make -C $< V=1 install

/usr/local/sbin/zipgateway /etc/init.d/zipgateway: install
	ls $@

init/%: /etc/init.d/zipgateway
	$< ${@F}

run: /usr/local/sbin/zipgateway
	@echo "# log: $@ $<"
	-$< --help
	file $<
	$< ${exe_args}

start: /etc/init.d/zipgateway
	@echo "log: $@ $< (setup before start)"
	$< restart
	sleep 1
	$< stop
	${SELF} run

tests: ${build_dir}
	cd ${build_dir} && ctest V=1

docker-compose/up: docker-compose.yml
	docker-compose --version
	docker-compose up --build

docker/run: docker-compose/up
