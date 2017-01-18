#!/usr/bin/env bash

while [[ $# -gt 1 ]]
do
key="$1"

case $key in
    -t|--targetdir)
    target_dir="$2"
    shift # past argument
    ;;
    *)
            # unknown option
    ;;
esac
shift # past argument or value
done

set -o pipefail  # trace ERR through pipes
set -o errtrace  # trace ERR through 'time command' and other functions
set -o nounset   ## set -u : exit the script if you try to use an uninitialised variable
set -o errexit   ## set -e : exit the script if any statement returns a non-true return value

echoerr() { printf "%s\n" "$*" >&2; }
# cleanup() 
# {
#     # cleanup code
# }
# trap cleanup 0

# directories
export repository_dir=$(git rev-parse --show-toplevel)
export docs_dir=$repository_dir/docs

if ! [[ $target_dir = /*  ]]; then
    target_dir=$repository_dir/$target_dir
fi

# supplement dir with git info
repo_name=$(basename $(git remote show -n origin | grep Fetch | cut -d: -f2-))
branch_name=$(git rev-parse --abbrev-ref HEAD)
target_dir=$target_dir/$repo_name/$branch_name

if [ ! -d "$target_dir" ]; then
    mkdir -p $target_dir
fi

# invoke doxygen
pushd $target_dir > /dev/null
doxygen $docs_dir/doxygen.cfg