# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'AffichageSynchrone'
copyright = '2026, Alain, Robin.M, Robin.C'
author = 'Alain, Robin.M, Robin.C'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

language = 'fr'

extensions = [
    'breathe',
]

# --- Configuration Breathe ---
breathe_projects = {
    "AffichageSynchrone": "../xml"  # Chemin vers le XML généré par Doxygen
}
breathe_default_project = "AffichageSynchrone"

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output
html_theme = 'sphinx_rtd_theme'
html_static_path = ['_static']
