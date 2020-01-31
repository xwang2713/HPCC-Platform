#!/bin/bash

VERSION=7.6.28-1
CEDF_DIR=/home/ming/work/HPCC/HPCC_Builds/HPCC-22602-add-dockerfile/containers/docker
LNDF_DIR=/home/ming/work/HPCC/HPCC_Builds/LN-22602-add-dockerfile/containers/docker

cd $CEDF_DIR


# build CE Platform

echo "Build CE Platform"
sudo docker build -t hpccsystemslegacy/platform:${VERSION} --build-arg version=${VERSION} ./platform
sudo docker push hpccsystemslegacy/platform:${VERSION}
sudo docker build -t hpccsystemslegacy/platform:latest --build-arg version=${VERSION} ./platform
sudo docker push hpccsystemslegacy/platform:latest

sudo docker build -t hpccsystems/platform:${VERSION} --build-arg version=${VERSION} ./platform
sudo docker push hpccsystems/platform:${VERSION}
sudo docker build -t hpccsystems/platform:latest --build-arg version=${VERSION} ./platform
sudo docker push hpccsystems/platform:latest

echo "Build CE Clienttools"
sudo docker build -t hpccsystemslegacy/clienttools:${VERSION} --build-arg version=${VERSION} ./clienttools
sudo docker push hpccsystemslegacy/clienttools:${VERSION}
sudo docker build -t hpccsystemslegacy/clienttools:latest --build-arg version=${VERSION} ./clienttools
sudo docker push hpccsystemslegacy/clienttools:latest

sudo docker build -t hpccsystems/clienttools:${VERSION} --build-arg version=${VERSION} ./clienttools
sudo docker push hpccsystems/clienttools:${VERSION}
sudo docker build -t hpccsystems/clienttools:latest --build-arg version=${VERSION} ./clienttools
sudo docker push hpccsystems/clienttools:latest

echo "Build VM"
sudo docker build -t hpccsystemslegacy/vm:${VERSION} --build-arg version=${VERSION} ./vm
sudo docker push hpccsystemslegacy/vm:${VERSION}
sudo docker build -t hpccsystemslegacy/vm:latest --build-arg version=${VERSION} ./vm
sudo docker push hpccsystemslegacy/vm:latest

sudo docker build -t hpccsystems/vm:${VERSION} --build-arg version=${VERSION} ./vm
sudo docker push hpccsystems/vm:${VERSION}
sudo docker build -t hpccsystems/vm:latest --build-arg version=${VERSION} ./vm
sudo docker push hpccsystems/vm:latest

cd $LNDF_DIR

echo "Build LN Platform"
sudo docker build -t gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-platform-wp:${VERSION} --build-arg version=${VERSION} ./platform
sudo docker push gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-platform-wp:${VERSION}
sudo docker build -t gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-platform-wp:latest --build-arg version=${VERSION} ./platform
sudo docker push gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-platform-wp:latest

echo "Build LN Clienttools"
sudo docker build -t gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-clienttools:${VERSION} --build-arg version=${VERSION} ./clienttools
sudo docker push gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-clienttools:${VERSION}
sudo docker build -t gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-clienttools:latest --build-arg version=${VERSION} ./clienttools
sudo docker push gitlab.ins.risk.regn.net:4567/docker-images/hpccsystems/ln-clienttools:latest
