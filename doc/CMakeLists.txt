# Copyright (c) 2018 Institute of Sound and Vibration Research, University of Southampton.
# All rights reserved.

# We want at least to different documents: apidoc (the code documentation, roughly equivalent to the Doxygen document, but with a nicer, human-created structure and some more architecture description) and userdoc (mainly human-written, maybe with some selected parts source from the code documentation). Therefore we should have a structure below $VISR/doc/ . This also means that all doxygen-related stuff should be in a specific place, not $VISR/doc. In the structure of the feature/sphinx-documentation branch, this is doc/doxygen, which is IMHO a sensible choice.
# ·         I think we should have different targets for the doxygen part (even if we don’t publish that) and the sphinx documents which are using the Doxygen-generated XML. I guess it is possible to a define a ‘meta-target’ like ‘all’ that is just a combination of other targets and performs no action on its own. Something alike add_custom_target https://cmake.org/cmake/help/v3.0/command/add_custom_target.html with no COMMAND, just a list of targets after DEPENDS.
# ·         Can you create PDFs as well? Probably that would be separate targets.
# ·         Of course the HTML output of Doxygen could be disabled at some point. But there should be nothing confusing about it if it goes into different build directories.
# ·         I cannot really imagine while separating the Doxygen and sphinx part could be so difficult. Especially if we support different documents, separating the parts is crucial. And after all, doxygen will provide only a certain aspect of the documents, so it should not be mixed up with the document sources if that can be avoided.
#
if (BUILD_DOCUMENTATION)

  find_package(Doxygen)
  if(NOT DOXYGEN_FOUND)
    #TODO add installation line guides for installing Doxygen via website www. ....
    message(FATAL_ERROR "Doxygen is needed to build the documentation. You can install it here at ...")
  endif()

  # Latex is used to create PDFs from the sphinx documentation
  find_package( LATEX COMPONENTS PDFLATEX )

#   ################################################################################
# # Directories setup
# # Do we need to set these directories as variables? TODO Delete if not necessary or not used
#   set( doxygen              ${PROJECT_SOURCE_DIR}/doc/doxygen/ )
#   set( sphinx               ${PROJECT_SOURCE_DIR}/doc/sphinx/ )
#
# # Doxygen
#   ################################################################################
#
# # Doxygen input variables
# # Used variable in Doxyfile.in to configure where the root is.
#   set( doxyfile_in          ${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile.in )
#   set( doxyfile_input       ${PROJECT_SOURCE_DIR}/src/ )
#   set( doxy_extra_files     ${PROJECT_SOURCE_DIR}/doc/doxygen/ )
#
#   ################################################################################
# # Doxygen output variables
#
#   set( doxyfile             ${PROJECT_BINARY_DIR}/doc/doxygen/Doxyfile )
#   set( doxygen_out          ${PROJECT_BINARY_DIR}/doc/doxygen/ )
#   # Used to set the output root of the doxyfile.in when xml/html is written.
#   set( doxy_output_root     ${PROJECT_BINARY_DIR}/doc/doxygen/ )
#   set( doxy_html_index_file ${PROJECT_BINARY_DIR}/doc/doxygen/html/index.html )
#   # Necessary variable for sphinx-breathe-doxygen C++ code generation used in config.py.in

  set( doxyfile_in          ${PROJECT_SOURCE_DIR}/doc/doxygen/Doxyfile.in )
  set( doxyfile             ${PROJECT_BINARY_DIR}/doc/doxygen/Doxyfile )
  set( doxy_html_index_file ${PROJECT_BINARY_DIR}/doc/doxygen/html/index.html )
  set( doxy_output_root     ${PROJECT_BINARY_DIR}/doc/doxygen/ )
  set( doxy_input           ${PROJECT_SOURCE_DIR}/src )
  set( doxy_extra_files     ${PROJECT_SOURCE_DIR}/doc/doxygen/ )
  # Used for checking dependencies when using Sphinx custom commands
  set( doxy_xml_index_file ${PROJECT_BINARY_DIR}/doc/doxygen/xml/index.xml )
  # Used as a varible config.py.in with Sphinx to configure the breathe extension.
  set( doxy_xml_index       ${PROJECT_BINARY_DIR}/doc/doxygen/xml/ )

  configure_file( ${doxyfile_in} ${doxyfile} @ONLY )

# TODO There should be seperate commands for generating html and xml seperately. This however, requires cmake to edit the doxyfile . May have to be done manually.
# TODO Doxygen generates files on any edit to the code source. It will regenerate each time, causing slow downs. The only possible way I can think of regeneration to stop is to check git somehow, if even possible.
# HTML command
add_custom_command( OUTPUT ${doxy_html_index_file}
		  COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
		  # The following should be ${doxyfile} only but it
		  # will break the dependency.
		  # The optimal solution would be creating a
		  # custom_command for ${doxyfile} generation
		  # but I still have to figure out how...
		  MAIN_DEPENDENCY ${doxyfile} ${doxyfile_in}
		  DEPENDS ${doxy_extra_files} # TODO <AF> Decide whether the subprojects should be added as dependency.
		  COMMENT "Generating Doxygen HTML and XML documentation")
		  
  add_custom_target( doxygen-doc DEPENDS ${doxy_html_index_file}  )

  # "Meta-target" to trigger build of all documentation outputs
  add_custom_target( doc DEPENDS doxygen-doc )

  option(BUILD_DOCUMENTATION_SPHINX "Create and install the user and API reference documentation (requires Sphinx and Breathe)" OFF)
  IF(BUILD_DOCUMENTATION_SPHINX )

    find_package(Sphinx)
    if(NOT Sphinx_FOUND)
      message(FATAL_ERROR "Sphinx is needed to build HTML-based documentation and VISR's web-api. You should install it here at ...")
    endif()

    # TODO Do we need this ? Look in virtualenv and node_modules directories of the build directory (as
    # per the hints in the documentation about installing sphinx, breathe,
    # sphinx_js and jsdoc).
    set(BUILD_VIRTUALENV_PATH ${PROJECT_BINARY_DIR}/venv/bin/)
    list(INSERT CMAKE_PROGRAM_PATH 0 ${BUILD_VIRTUALENV_PATH})

    # HTML output directory
    set ( userdoc_html_dir     ${PROJECT_BINARY_DIR}/doc/userdoc/html/)
    # For PDF generation using sphinx pdfbuilder
    set ( userdoc_pdf_output   ${PROJECT_BINARY_DIR}/doc/userdoc/pdf/)
    # Used as dependencies when making custom command directive for Sphinx

    # <af> apparently unused:
    set (userdoc_config_file    ${PROJECT_BINARY_DIR}/doc/userdoc/conf.py)
    set (userdoc_config_in      ${PROJECT_SOURCE_DIR}/doc/userdoc/conf.py.in)
    set (userdoc_source_root    ${PROJECT_SOURCE_DIR}/doc/userdoc/)
    set (apidoc_source_root    ${PROJECT_SOURCE_DIR}/doc/apidoc/)

    set( userdoc_output_root   ${PROJECT_BINARY_DIR}/doc/userdoc )
    set( apidoc_output_root   ${PROJECT_BINARY_DIR}/doc/apidoc )

    if(NOT DEFINED SPHINX_THEME)
        set(SPHINX_THEME default)
    endif()

    if(NOT DEFINED SPHINX_THEME_DIR)
        set(SPHINX_THEME_DIR)
    endif()

    # Function to define a target building HTML documentation from a source directory containing Sphinx rst code.
    function( HtmlDocBuild targetName sourceDir outputDir depends )
      # message( STATUS "configure_file( ${configTemplate} ${outputDir}/html/conf.py @ONLY )" )
      configure_file( ${sourceDir}/conf.py.in ${outputDir}/html/conf.py @ONLY )
      add_custom_target( ${targetName}-html
        ${SPHINX_EXECUTABLE}
                   -q
                   -b html
                   -c ${outputDir}/html
                   -d ${outputDir}/cache
                    ${sourceDir}
                    ${outputDir}/html
      DEPENDS ${depends}
      COMMENT "HTML generation for target ${targetName}-html."
      )
    endfunction(HtmlDocBuild)

    # Function to define a target building PDF documentation from a source directory containing Sphinx rst code.
    function( PdfDocBuild targetName sourceDir outputDir depends )
      #message( STATUS "configure_file( ${configTemplate} ${outputDir}/latex/conf.py @ONLY )" )
      configure_file( ${sourceDir}/conf.py.in ${outputDir}/latex/conf.py @ONLY )

      add_custom_command(
        OUTPUT ${outputDir}/latex/visr.tex
        COMMAND ${SPHINX_EXECUTABLE}
                   -q
                   -b latex
                   -c ${outputDir}/latex
                   -d ${outputDir}/cache
                    ${sourceDir}
                    ${outputDir}/latex
      DEPENDS ${depends}
      COMMENT "LaTeX generation for target ${targetName}-pdf."
      )

      add_custom_target( ${targetName}-pdf
        # Multiple runs to sort undefined reference (very crude at the moment
        COMMAND ${PDFLATEX_COMPILER} visr
        COMMAND ${PDFLATEX_COMPILER} visr
	COMMAND ${PDFLATEX_COMPILER} visr
	COMMAND ${CMAKE_COMMAND} -E rename ${outputDir}/latex/visr.pdf ${outputDir}/${targetName}-${VISR_MAJOR_VERSION}.${VISR_MINOR_VERSION}.${VISR_PATCH_VERSION}.pdf
        WORKING_DIRECTORY ${outputDir}/latex
        DEPENDS ${outputDir}/latex/visr.tex
        COMMENT "PDF compilation for target ${targetName}-pdf." )
    endfunction(PdfDocBuild)

    PdfDocBuild( userdoc ${userdoc_source_root} ${userdoc_output_root} doxygen-doc )
    PdfDocBuild( apidoc ${apidoc_source_root} ${apidoc_output_root} doxygen-doc )

    HtmlDocBuild( userdoc ${userdoc_source_root} ${userdoc_output_root} doxygen-doc )
    HtmlDocBuild( apidoc ${apidoc_source_root} ${apidoc_output_root} doxygen-doc )

    # Add the Sphinx-generated targets to the "doc" meta-target
    add_dependencies( doc apidoc-html apidoc-pdf userdoc-html userdoc-pdf ) 

  endif(BUILD_DOCUMENTATION_SPHINX)

  # Include the Doxygen documentation in the package installer
  # Note: Doxygen as output format is deprecated, so it's not going to be visible externally.
  # install( DIRECTORY ${PROJECT_BINARY_DIR}/doc/doxygen/html DESTINATION ${DOCUMENTATION_INSTALL_DIRECTORY} )

endif(BUILD_DOCUMENTATION)
