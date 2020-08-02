# -- Project information -----------------------------------------------------
import time

project = 'implicit_octree_nns'
author = 's-nandi'
copyright = str(time.localtime().tm_year) + ', ' + author

# -- General configuration ---------------------------------------------------

# Add any Sphinx extension module names here, as strings. They can be
# extensions coming with Sphinx (named 'sphinx.ext.*') or your custom
# ones.
extensions = ['breathe', 'recommonmark']

# Add any paths that contain templates here, relative to this directory.
templates_path = ['_templates']

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = ['_build', 'Thumbs.db', '.DS_Store']

# -- Options for HTML output -------------------------------------------------

# The theme to use for HTML and HTML Help pages.  See the documentation for
# a list of builtin themes.
#
html_theme = 'sphinx_rtd_theme'

# Add any paths that contain custom static files (such as style sheets) here,
# relative to this directory. They are copied after the builtin static files,
# so a file named "default.css" will overwrite the builtin "default.css".
html_static_path = ['_static']

# -- Breathe configuration ---------------------------------------------------
breathe_default_project = project

# -- Read the Docs configuration ---------------------------------------------
import os
import subprocess
from pathlib import Path


def configure_doxyfile(doxyfile_path, input_path, output_path):
    print("Doxyfile path", doxyfile_path)
    with open(doxyfile_path / 'Doxyfile.in', 'r') as file:
        filedata = file.read()

    filedata = filedata.replace('@DOXYGEN_INPUT_DIR@', str(input_path))
    filedata = filedata.replace('@DOXYGEN_OUTPUT_DIR@', str(output_path))

    with open(doxyfile_path / 'Doxyfile', 'w') as file:
        file.write(filedata)


read_the_docs_build = os.environ.get('READTHEDOCS', None) == 'True'

if read_the_docs_build:
    conf_dir = Path(__file__).parent

    doxyfile_path = conf_dir.parent
    input_path = conf_dir.parent.parent.joinpath('include', project)
    output_path = conf_dir.parent.parent.joinpath('build', 'docs',
                                                  'doxygen')
    output_path.mkdir(parents=True, exist_ok=True)
    configure_doxyfile(doxyfile_path, input_path, output_path)
    subprocess.call('cd ..; doxygen', shell=True)
    breathe_projects = {}
    breathe_projects[project] = output_path.joinpath('xml')
