#!/usr/bin/env bash

#
# entrypoint.sh: entrypoint to lrose-soloii Docker image
#

#
# centos:5.11: GNU bash, version 3.2.25(1)-release (x86_64-redhat-linux-gnu)
#

_help () {
  echo 'start soloii'
  echo 'INPUT (OPTIONAL): directory path from which to invoke soloii'
  exit 0
}

if [[ "$@" == '-h' ]] || [[ "$@" == '--help' ]] ; then
  _help
elif [[ "$@" != '' ]] ; then
  #
  # verify that $@ is a directory
  #
  if [[ -d "$@" ]]; then
    cd $@
  else
    echo "$@ is not a directory"
    echo 'please specify a valid directory'
    exit 1
  fi
fi

soloii
