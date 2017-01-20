#!/usr/bin/env bash

while [[ $# -gt 0 ]]; do
    key="$1"
    case "$key" in
        # This is a flag type option. Will catch either -v or --verbose
        -v|--verbose)
        verbose=1
        ;;
        # This is a flag type option. Will catch either -v or --verbose
        -c|--container-mode)
        container_mode=1
        ;;
        # This is an arg value type option. Will catch -d value or --docdir value
        -d|--docdir)
        shift # past the key and to the value
        doc_dir="$1"
        ;;
        # This is an arg value type option. Will catch -t value or --targetdir value
        -r|--repository)
        shift # past the key and to the value
        repository="$1"
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
cleanup() 
{
    # remove tmp dir
    if [[ ${tmp_dir+x} ]]; then
        rm -rf $tmp_dir
    fi

    #restore path
    cd $current_dir
    echoverbose "* Temp dir removed. Path restored."
}
current_dir=$(realpath .)
trap cleanup 0

echoverbose "* current_dir: $current_dir"

# verify arguments
if [[ ! ${doc_dir+x} ]]; then
    echoerr "-d|--doc_dir argument is missing!"
    exit 1
fi

if [[ ! ${repository+x} ]]; then
    echoerr "-r|--repository argument is missing!"
    exit 2
fi

# for container only when private key specified
if [[ ${container_mode+x} ]]; then
    if [[ ! ${PRIVATE_KEY+x} ]]; then
        echoerr "PRIVATE_KEY with base64 authentication key not set!"
        exit 1
    fi

    mkdir -p ~/.ssh
    echo $PRIVATE_KEY > ~/.ssh/id_rsa.tmp
    base64 -d ~/.ssh/id_rsa.tmp > ~/.ssh/id_rsa
    chmod 600 ~/.ssh/id_rsa
    ssh-keyscan bitbucket.org >> ~/.ssh/known_hosts
    git config --global user.email "push_docs@bitbucket.org"
    git config --global user.name "push_docs"
    git config --global push.default matching
fi

# check out repository
tmp_dir=$(mktemp -d)
cd $tmp_dir

git clone $repository .
echoverbose "* clonned to $PWD"

# copy doc dir & add to git
if ! [[ $doc_dir = /*  ]]; then
    doc_dir=$current_dir/$doc_dir
fi

echoverbose "* copying $doc_dir"
cp -r $doc_dir .
git add --all

echoverbose $(git status)

# commit and push if any changes detected
if [[ ! -z $(git status -uno --porcelain) ]]; then
    if [[ ${BITBUCKET_COMMIT+x} ]]; then
        git commit -m "docs publication $BITBUCKET_COMMIT"
    else
        git commit -m "docs publication - manual run"
    fi
    git push
else
    echo "Nothing to push. Skipping."
fi 