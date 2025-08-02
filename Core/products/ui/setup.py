import os
import setuptools
import sys

import builraui

# setuptools expects to be invoked from within the directory of setup.py, but it
# is nice to allow:
#   python path/to/setup.py install
# to work (for scripts, etc.)
os.chdir(os.path.dirname(os.path.abspath(__file__)))

setuptools.setup(
    name = "builra-ui",
    version = builraui.__version__,

    author = builraui.__author__,
    author_email = builraui.__email__,
    url = 'http://github.com/apple/swift-builra',
    license = 'Apache 2.0',

    description = "builra User Interface",
    keywords = 'buildsystem build systems builra swift',
    zip_safe = False,

    packages = setuptools.find_packages(),

    install_requires=['SQLAlchemy', 'Flask'],
)
