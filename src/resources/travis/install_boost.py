#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import os
import shutil
import subprocess
import sys
import urllib

def is_library_built(boost_root, library, version, stagedir):
    if os.name == 'nt':
        version_stub = version[0:version.rindex('.')].replace('.', '_')
        filename = 'libboost_{}-vc140-mt-s-{}.lib'.format(library, version_stub)
    else:
        filename = 'libboost_{}.a'.format(library)

    return os.path.exists(os.path.join(boost_root, stagedir, 'lib', filename))

def are_libraries_built(boost_root, libraries, version, stagedir):
    return all(map(lambda library: is_library_built(boost_root, library, version, stagedir), libraries))

def get_boost_archive_name(version):
    underscored_version = version.replace('.', '_')

    if os.name == 'nt':
        extension = '.7z'
    else:
        extension = '.tar.bz2'

    return 'boost_{}{}'.format(underscored_version, extension)

def get_extracted_folder_name(archive_path):
    return os.path.splitext(os.path.splitext(archive_path)[0])[0]

def get_boost_url(version):
    return u'https://downloads.sourceforge.net/project/boost/boost/{}/{}'.format(version, get_boost_archive_name(version))

def extract_archive(archive_path):
    print('Extracting {}...'.format(archive_path))

    output_path = os.path.dirname(archive_path)

    extracted_folder = get_extracted_folder_name(archive_path)
    if os.path.exists(extracted_folder):
        shutil.rmtree(extracted_folder)

    if os.name == 'nt':
        subprocess.check_call(['7z', 'x', archive_path, u'-o{}'.format(output_path)], cwd = output_path)
    else:
        subprocess.check_call(['tar', 'xf', archive_path], cwd = output_path)

def download_boost(version, destination_path):
    print('Downloading Boost v{}...'.format(version))

    if os.path.exists(destination_path):
        os.remove(destination_path)

    urllib.urlretrieve(get_boost_url(version), destination_path)

def build_boost(boost_root, address_model, toolchain, stagedir, libraries):
    print('Building Boost libraries {} at {}...'.format(libraries, boost_root))

    if os.name == 'nt':
        extension = '.bat'
        toolset = toolchain
        runtime_link = 'static,shared'
        os_arguments = [
            'threadapi=win32'
        ]
    else:
        extension = '.sh'
        toolset = toolchain
        runtime_link = 'shared'
        os_arguments = [
            'cxxflags=-std=c++14 -fPIC',
            'boost.locale.icu=off'
        ]

    bootstrap = os.path.join(os.path.abspath(boost_root), 'bootstrap{}'.format(extension))
    print('Running {}...'.format(bootstrap))
    subprocess.check_call([bootstrap], cwd = boost_root)

    b2_command = [
        os.path.join(os.path.abspath(boost_root), 'b2'),
        'toolset={}'.format(toolset),
        'link=static',
        'runtime-link={}'.format(runtime_link),
        'variant=release',
        'address-model={}'.format(address_model),
        'define=NO_COMPRESSION=1',
        '--stagedir={}'.format(stagedir)
    ] + os_arguments + [ '--with-{}'.format(library) for library in libraries ]

    print('Running {}...'.format(' '.join(b2_command)))
    subprocess.check_call(b2_command, cwd = boost_root)

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description = 'Download and build Boost.')
    parser.add_argument('--directory', '-d', required = True)
    parser.add_argument('--boost-version', '-b', required = True)
    parser.add_argument('--address-model', '-a', required = True)
    parser.add_argument('--toolchain', '-t', required = True)
    parser.add_argument('--stagedir', '-s', required = False, default=None)
    parser.add_argument('libraries', nargs = '+', metavar = 'library')

    arguments = parser.parse_args()

    if arguments.stagedir:
        stagedir = arguments.stagedir
    else:
        stagedir = os.path.join('stage', arguments.address_model)

    boost_archive_path = os.path.join(arguments.directory, get_boost_archive_name(arguments.boost_version))
    boost_folder_path = os.path.join(arguments.directory, get_extracted_folder_name(boost_archive_path))

    if are_libraries_built(boost_folder_path, arguments.libraries, arguments.boost_version, stagedir):
        sys.exit(0)

    download_boost(arguments.boost_version, boost_archive_path)

    extract_archive(boost_archive_path)

    build_boost(boost_folder_path, arguments.address_model, arguments.toolchain, stagedir, arguments.libraries)