.PHONY: all deploy build clean

CC  = ${HOME}/.everdev/clang/opt/work/llvm/install/bin/clang

all: build

deploy:
	./deploy.sh

build: factory microwave

%: contracts/%.cpp
	$(CC) $? -o $@.tvc

clean:
	rm -f *.tvc *.abi
