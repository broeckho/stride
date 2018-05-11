#!/bin/sh
################################################################################
# Notes: Originally from Jeroen de Bruijn
# See https://gist.github.com/vidavidorra/548ffbcdae99d752da02
# Also a number of improvements in de comments section have been incorporated.
#
# Preconditions:
# - Packages: doxygen doxygen-doc doxygen-latex doxygen-gui graphviz
# - An gh-pages branch should already exist. See below for mor info on hoe to
#   create a gh-pages branch.
#
# Required global variables:
# - GH_REPO_TOKEN       : Secure token to the github repository.
################################################################################

################################################################################
##### Setup this script and get the current gh-pages branch.               #####
################################################################################
##### Exit with nonzero exit code if anything fails; set up variables.
set -e
GH_REPO_ORG=$(echo $TRAVIS_REPO_SLUG | cut -d "/" -f 1)
GH_REPO_NAME=$(echo $TRAVIS_REPO_SLUG | cut -d "/" -f 2)
GH_REPO_REF="github.com/$GH_REPO_ORG/$GH_REPO_NAME.git"

##### Create a clean working directory with gh-pages branch
git clone -b gh-pages https://git@$GH_REPO_REF code_docs
cd code_docs

##### Configure git.
# Set push default to simple (push only current branch). Pretend to be user Travis CI.
git config --global push.default simple
git config user.name "Travis CI"
git config user.email "travis@travis-ci.org"

##### Remove everything currently in the gh-pages branch.
# GitHub is smart enough to know which files have changed and which not and will
# only update the changed files. So the gh-pages branch can be safely cleaned, and
# it is sure that everything pushed later is the new documentation.
CURRENTCOMMIT=$(git rev-parse HEAD)
##### Reset working tree to initial commit.
git reset --hard $(git rev-list HEAD | tail -n 1)
git reset --soft $CURRENTCOMMIT # Move HEAD back to where it was

##### Need to create a .nojekyll file to allow filenames starting with an underscore
# to be seen on the gh-pages site. Therefore creating an empty .nojekyll file.
# Presumably this is only needed when the SHORT_NAMES option in Doxygen is set
# to NO, which it is by default. So creating the file just in case.
echo "" > .nojekyll

################################################################################
##### Generate the Doxygen code documentation and log the output.          #####
################################################################################
echo 'Generating Doxygen code documentation...'
cd $TRAVIS_BUILD_DIR && make STRIDE_INCLUDE_DOC=TRUE configure
cd cmake-build-release/doc/doxygen && make all && mv html $TRAVIS_BUILD_DIR/code_docs
cd $TRAVIS_BUILD_DIR && mv doc/doxygen/index.html code_docs

################################################################################
##### Upload the documentation to the gh-pages branch of the repository.   #####
# Only upload if Doxygen successfully created the documentation.
# Check this by verifying that the html directory and the file html/index.html
# both exist. This is a good indication that Doxygen did it's work.
################################################################################
cd code_docs
if [ -d "html" ] && [ -f "html/index.html" ]; then

    echo 'Uploading documentation to the gh-pages branch...'

    ##### Add everything in this directory (the Doxygen doc) to gh-pages branch.
    # GitHub is smart and knows which files have changed and which have not,
    # and will only update changed files.
    git add --all

    ##### Commit the added files with a title and description containing the Travis CI
    # build number and the GitHub commit reference that issued this build.
    git commit -m "Deploy to GitHub Pages Travis build: ${TRAVIS_BUILD_NUMBER}" -m "Commit: ${TRAVIS_COMMIT}"

    ##### Force push to remote gh-pages branch. Output redirected to /dev/null
    # to hide any sensitive credential data that might otherwise be exposed.
    git push --force "https://${GH_REPO_TOKEN}@${GH_REPO_REF}" > /dev/null 2>&1
else
    echo '' >&2
    echo 'Warning: No documentation (html) files have been found!' >&2
    exit 1
fi
