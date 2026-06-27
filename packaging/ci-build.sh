#!/bin/sh
# Build ten64-controller (.deb) inside a debian:trixie arm64 container. Invoked by
# .github/workflows/deb.yml as:
#   docker run --rm -v "$PWD:/src" -w /src debian:trixie sh packaging/ci-build.sh
# Writes the resulting .deb files to /src/built-debs (bind-mounted to the runner).
set -eux

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install -y --no-install-recommends \
  build-essential ca-certificates git cmake dpkg-dev debhelper python3

git config --global --add safe.directory /src
python3 packaging/deb-version.py --write-changelog
dpkg-buildpackage -b -us -uc

mkdir -p built-debs
cp ../*.deb built-debs/
ls -l built-debs/
