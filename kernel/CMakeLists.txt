# -*- mode: cmake; -*-
#
#

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/mod2.h.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/mod2.h)

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_BINARY_DIR})
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/factory)
include_directories(${CMAKE_SOURCE_DIR}/ntl/include)

include(VersionMacros)
include(KernelMacros)
create_version(KVERSION)

set(ADD_DEFS "NOSTREAMIO;${_OSDEF};HAVE_CONFIG_H;SINGULAR")
mk_prcopy(PRCOPY ${CMAKE_CURRENT_SOURCE_DIR}/prCopy.pl)

# C++ sources which are used if part of the kernel are dynamically linked
#if(${DL_KERNEL})
set(proc_dyn_srcs p_Procs_Generate.cc p_Procs_Impl.h mod2.h)
add_executable(p_Procs_Dynamic ${proc_dyn_srcs})
set_target_properties(p_Procs_Dynamic PROPERTIES
  COMPILE_DEFINITIONS "p_Procs_Dynamic")
mk_procs(DL_KERNEL_SRCS ${CMAKE_CURRENT_BINARY_DIR}/p_Procs_Dynamic "Dynamic")
#else(${DL_KERNEL})
#  set(DL_KERNEL_SRCS "")
#endif(${DL_KERNEL})

# C++ sources which are used for dynamic libraries if part of the kernel is
# dynamically linked
set(proc_lib_srcs ${DL_KERNEL_SRCS} p_Procs_Generate.cc)
add_executable(p_Procs_Lib ${proc_lib_srcs})
set_target_properties(p_Procs_Dynamic PROPERTIES
  COMPILE_DEFINITIONS "p_Procs_Lib")
mk_procs(DL_LIB_SRCS ${CMAKE_CURRENT_BINARY_DIR}/p_Procs_Lib "Lib")

# C++ sources which are used if no parts of the kernel are dynamically linked
set(proc_static_srcs p_Procs_Generate.cc p_Procs_Impl.h mod2.h)
add_executable(p_Procs_Static ${proc_static_srcs})
set_target_properties(p_Procs_Static PROPERTIES
  COMPILE_DEFINITIONS "p_Procs_Static")
mk_procs(STATIC_SRCS ${CMAKE_CURRENT_BINARY_DIR}/p_Procs_Static "Static")

#
set(kernel_dl_srcs     ${DL_LIB_SRCS} p_Procs_Dynamic.cc)
set(dl_lib_srcs        ${DL_LIB_SRCS} p_Procs_Lib.cc)
set(kernel_static_srcs ${STATIC_SRCS} p_Procs_Static.cc)

#
#
#
# normal C++ source files
set(kernel_srcs
  ${PRCOPY}
  clapconv.cc
  clapsing.cc
    febase.cc feread.cc feResource.cc
    ffields.cc hdegree.cc hilb.cc hutil.cc
    sca.cc gring.cc gr_kstd2.cc summator.cc ncSAMult.cc ncSACache.cc ncSAFormula.cc
    ideals.cc intvec.cc int64vec.cc
    khstd.cc kstdfac.cc
    kstd1.cc kstd2.cc kutil.cc
    kutil2.cc shiftDVec.cc shiftDVec2.cc
    longalg.cc longrat.cc longrat0.cc
    maps.cc matpol.cc misc.cc gnumpfl.cc gnumpc.cc
    rmodulo2m.cc rmodulon.cc rintegers.cc
    modulop.cc mpr_complex.cc sparsmat.cc fast_maps.cc
    fglmzero.cc fglmvec.cc fglmgauss.cc fglmhom.cc fglmcomb.cc
    numbers.cc polys.cc p_polys.cc polys0.cc polys1.cc polys-impl.cc
    ring.cc shortfl.cc kspoly.cc
    syz.cc syz0.cc syz1.cc syz2.cc syz3.cc
    timer.cc weight.cc
    GMPrat.cc multicnt.cc npolygon.cc semic.cc spectrum.cc splist.cc
    mod_raw.cc walkProc.cc walkMain.cc walkSupport.cc
    eigenval.cc units.cc kbuckets.cc sbuckets.cc
    mpr_inout.cc mpr_base.cc mpr_numeric.cc
    prCopy.cc p_Mult_q.cc
    pDebug.cc pInline2.cc pInline1.cc pInline0.cc
    pShallowCopyDelete.cc fast_mult.cc digitech.cc
    tgb.cc tgbgauss.cc ringgb.cc f5data.cc f5lists.cc f5gb.cc
    f5c.cc F5cLists.cc ratgring.cc shiftgb.cc gfan.cc linearAlgebra.cc
    ${KVERSION}
    weight0.c fegetopt.c fereadl.c dError.c mmstd.c
)


set(kernel_hdrs hutil.h stairc.h ideals.h
        longalg.h structs.h intvec.h int64vec.h longrat.h
        numbers.h stype.h clapconv.h maps.h
        clapsing.h matpol.h polys.h p_polys.h syz.h
        ring.h timer.h dError.h fast_maps.h
        febase.h shortfl.h mpr_complex.h mpr_global.h
        summator.h ncSAMult.h ncSACache.h sca.h gring.h ncSAFormula.h
        walkProc.h walkMain.h walkSupport.h
        ffields.h khstd.h sparsmat.h gnumpfl.h gnumpc.h
        fglm.h kstd1.h
        rmodulo2m.h rmodulon.h rintegers.h modulop.h sing_dbm.h weight.h
        fglmgauss.h fglmvec.h
        kstdfac.h kmatrix.h
        kutil.h kutil2.h shiftDVec.h
        dbm_sl.h polys-impl.h
        GMPrat.h multicnt.h npolygon.h semic.h spectrum.h splist.h multicnt.h
        eigenval.h units.h mod_raw.h kbuckets.h sbuckets.h
        mpr_global.h mpr_inout.h mpr_base.h mpr_numeric.h
        fegetopt.h distrib.h
        ratgring.h shiftgb.h prCopy.h prCopyMacros.h
        p_MemAdd.h p_MemCopy.h p_MemCmp.h p_Numbers.h
        pInline2.h pInline1.h pInline0.h mmalloc.h
        omSingularConfig.h pShallowCopyDelete.h
        p_Procs_Dynamic.h p_Procs_Impl.h p_Procs_Set.h
        p_Procs.h p_Procs_Static.h p_Mult_q.h gfan.h
        )

# define the libraries to be created
add_library(kernel STATIC ${kernel_static_srcs} ${kernel_srcs})
set_target_properties(kernel PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS}")

if(${DL_KERNEL})
  add_library(kernel_so SHARED ${kernel_dl_srcs} ${kernel_cxx_srcs} ${kernel_c_srcs})
  set_target_properties(kernel_so PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS}")
endif(${DL_KERNEL})

add_library(kernel_mmalloc STATIC mmalloc.cc)
set_target_properties(kernel_mmalloc PROPERTIES COMPILE_DEFINITIONS "${ADD_DEFS} ${CMAKE_CXX_FLAGS_DEBUG}")

add_library(sg-fegetopt fegetopt.c)
