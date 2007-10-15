#!/bin/sh

[ "x$1" = "xclean" ] && {
    rm -rf $(hg status --no-status --unknown) autom4te.cache
    exit 0
}

autoreconf --verbose --install --symlink $@
