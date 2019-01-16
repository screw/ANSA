#!/bin/env bash

# This script runs the fingerprints stage on Travis.
# All arguments are passed to the fingerprinttest script.
#
# The following environment variables must be set when invoked:
#    MODE                   - must be "debug" or "release"
#
#    TRAVIS_REPO_SLUG       - this is provided by Travis, most likely has the
#                             value "inet-framework/inet"


set -e # make the script exit with error if any executed command exits with error

#echo -e "\nccache summary:\n"
#ccache -s
#echo -e ""

export PATH="/root/omnetpp-5.4.1-$TARGET_PLATFORM/bin:/usr/lib/ccache:$PATH"

# INET_BASE is defined in docker
# setenv is invoked during docker build, but the environment variables exist only during the "RUN" statement,
# so we need to invoked it again
cd $INET_BASE
. setenv -f

# this is where the cloned ANSA repo is mounted into the container (as prescribed in /.travis.yml)
cd /$TRAVIS_REPO_SLUG

. setenv -f



#cp -r /root/nsc-0.5.3 3rdparty

#opp_featuretool enable VoIPStream VoIPStream_examples TCP_NSC TCP_lwIP

# We have to explicitly enable diagnostics coloring to make ccache work,
# since we redirect stderr here, but not in the build stage.
# See https://github.com/ccache/ccache/issues/222
echo -e "CFLAGS += -fcolor-diagnostics\n\n$(cat src/makefrag)" > src/makefrag

echo -e "\nBuilding (silently)...\n"
make makefiles > /dev/null 2>&1
make MODE=$MODE USE_PRECOMPILED_HEADER=no -j $(nproc) > /dev/null 2>&1

#echo -e "\nccache summary:\n"
#ccache -s

echo -e "\nBuild finished, starting fingerprint tests..."
echo -e "Additional arguments passed to fingerprint test script: " $@ "\n"

cd tests/fingerprint
if [ "$MODE" = "debug" ]; then
    ./fingerprints ansa.csv -e ansa -d "$@"
else
    ./fingerprints ansa.csv -e ansa "$@"
fi
#