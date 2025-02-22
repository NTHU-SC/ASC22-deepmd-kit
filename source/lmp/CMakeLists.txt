add_subdirectory(plugin)

file(GLOB LMP_HEADER *.h)
file(GLOB LMP_SRC *.cpp)
file(GLOB LMP_SHSCRIPT *.sh)

unset(LMP_INSTALL_FILES)
list(APPEND LMP_INSTALL_FILES ${LMP_HEADER})
list(APPEND LMP_INSTALL_FILES ${LMP_SRC})
list(APPEND LMP_INSTALL_FILES ${LMP_SHSCRIPT})

function(_add_lmp_variant variant_name prec_def)
  if(TENSORFLOW_LINK_LIBPYTHON)
    configure_file("env_py.sh.in" "env${variant_name}.sh" @ONLY)
  else()
    configure_file("env.sh.in" "env${variant_name}.sh" @ONLY)
  endif()
endfunction()
_add_lmp_variant("${HIGH_PREC_VARIANT}" "${HIGH_PREC_DEF}")
_add_lmp_variant("${LOW_PREC_VARIANT}" "${LOW_PREC_DEF}")

configure_file("pair_deepmd.h.in" "pair_deepmd.h" @ONLY)
configure_file("lammps_install_list.txt.in" "lammps_install_list.txt" @ONLY)
