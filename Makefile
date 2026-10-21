PREFIX ?= /usr/local
CXX    ?= c++

.PHONY: install uninstall test test-installed help

help:
	@echo "make install    install into $(PREFIX) (PREFIX=... to change)"
	@echo "make uninstall  remove it again"
	@echo "make test       run the test suite against this checkout"
	@echo "make test-installed  run the test suite against the installed copy"

install:
	./install.sh --prefix "$(PREFIX)"

uninstall:
	./install.sh --prefix "$(PREFIX)" --uninstall

test:
	CXX="$(CXX)" tests/run.sh

test-installed:
	CXX="$(CXX)" tests/run.sh --installed
