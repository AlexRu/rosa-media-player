#!/bin/sh

# This file is gets the version and release number as parameters
# and put it to files:
# src/romp_version.h and src/romp_release.h

#romp_version=`sed -n '/Version:/p' rosa-media-player.spec | cut -d ':' -f2 | cut -d ' ' -f1`
#romp_release=`sed -n '/Release:/p' rosa-media-player.spec | cut -d ':' -f2  | cut -d ' ' -f2`

#ROMP_VERSION="#define ROMP_VERSION ${romp_version}"
#echo ROMP Version is $ROMP_VERSION

#ROMP_RELEASE="#define ROMP_RELEASE ${romp_release}"
#echo ROMP Release is $ROMP_RELEASE

ROMP_VERSION="#define ROMP_VERSION \"${1}\""
echo ROMP Version is $ROMP_VERSION

ROMP_RELEASE="#define ROMP_RELEASE \"${2}\""
echo ROMP Release is $ROMP_RELEASE

echo "$ROMP_VERSION" > src/romp_version.h
echo "$ROMP_RELEASE" > src/romp_release.h

