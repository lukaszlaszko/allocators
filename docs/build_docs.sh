#!/usr/bin/env bash

while [[ $# -gt 0 ]]; do
    key="$1"
    case "$key" in
        # This is a flag type option. Will catch either -v or --verbose
        -v|--verbose)
        verbose=1
        ;;
        # This is an arg value type option. Will catch -t value or --targetdir value
        -t|--targetdir)
        shift # past the key and to the value
        target_dir="$1"
        ;;
        *)
        # Do whatever you want with extra options
        echo "Unknown option '$key'"
        ;;
    esac
    # Shift after checking all the cases to get the next option
    shift
done

if [[ ${verbose+x} ]]; then
    set -x
fi
set -o errtrace  # trace ERR through 'time command' and other functions
set -o nounset   ## set -u : exit the script if you try to use an uninitialised variable
set -o errexit   ## set -e : exit the script if any statement returns a non-true return value

echoerr() { printf "%s\n" "$*" >&2; }
echoverbose() { if [[ ${verbose+x} ]]; then printf "%s\n" "$*" >&2; fi }

# verify arguments
if [[ ! ${target_dir+x} ]]; then
    echoerr "-t|--targetdir is missing!"
    exit 1
fi

# directories
export repository_dir=$(git rev-parse --show-toplevel)
export docs_dir=$repository_dir/docs

if ! [[ $target_dir = /*  ]]; then
    target_dir=$repository_dir/$target_dir
fi

# supplement dir with git info
repo_name=$(basename $(git remote show -n origin | grep Fetch | cut -d: -f2-))
tag_name=$(git name-rev --name-only --tags --no-undefined HEAD 2>/dev/null | sed -n 's/^\([^^~]\{1,\}\)\(\^0\)\{0,1\}$/\1/p')
if [[ $tag_name ]]; then
    target_dir=$target_dir/$repo_name/$tag_name
    export version=$tag_name
else
    branch_name=$(git rev-parse --abbrev-ref HEAD)
    target_dir=$target_dir/$repo_name/$branch_name
    export version=$branch_name
fi

if [ ! -d "$target_dir" ]; then
    mkdir -p $target_dir
fi

# verbose info
echoverbose "repository_dir: $repository_dir"
echoverbose "docs_dir: $docs_dir"
echoverbose "target_dir: $target_dir"

# invoke doxygen
pushd $target_dir > /dev/null
doxygen $docs_dir/doxygen.cfg