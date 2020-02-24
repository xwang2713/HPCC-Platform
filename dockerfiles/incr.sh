#!/bin/bash

# Use this script to build local images for testing, while leveraging the work already done in the most recent tag
# on the branch that your local branch was based on
#
# Pass in a get commit (that you previously built using this script) if you want to override the default calculation
# of the base build

HEAD=$(git rev-parse --short HEAD)
PREV=$1
[[ -z ${PREV} ]] && PREV=$(git log --format=format:%h $(git describe --abbrev=0 --tags)..HEAD | grep `docker images hpccsystems/platform-core --format {{.Tag}} | head -n 1`)
[[ -z ${PREV} ]] && PREV=$(git describe --abbrev=0 --tags)

BUILD_TYPE=Debug
BUILD_LABEL=${HEAD}-Debug
#GITHUB_USER=richardkchapman
GITHUB_USER=xwang2713

if [[ "$HEAD" == "$PREV$FORCE" ]]  # set environment variable FORCE before running to override this check
then
    echo Docker image hpccsystems/platform-core:${HEAD} already exists
else
    set -e
    
    DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
    pushd $DIR 2>&1 > /dev/null
    
    echo Building local incremental images based on ${PREV}
        
    if [[ -n "$FORCE" ]] ; then
      docker image build -t hpccsystems/platform-build:${BUILD_LABEL} --build-arg PREV_LABEL=${HEAD}-Debug --build-arg BASE_VER=7.8 --build-arg BUILD_TYPE=Debug platform-build/
    else
      docker image build -t hpccsystems/platform-build:${BUILD_LABEL} --build-arg PREV_LABEL=${PREV}-Debug --build-arg COMMIT=${HEAD} --build-arg USER=${GITHUB_USER} platform-build-incremental/
    fi
    docker image build -t hpccsystems/platform-core:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} platform-core-debug/  
    
    docker image build -t hpccsystems/roxie:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} roxie/  
    docker image build -t hpccsystems/dali:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} dali/  
    docker image build -t hpccsystems/esp:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} esp/  
    docker image build -t hpccsystems/eclccserver:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} eclccserver/  
    docker image build -t hpccsystems/eclagent:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} eclagent/  
    docker image build -t hpccsystems/toposerver:${BUILD_LABEL} --build-arg BUILD_LABEL=${BUILD_LABEL} toposerver/  
    
    echo Built hpccsystems/*:${BUILD_LABEL}
fi
