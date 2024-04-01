# Note that this script can accept some limited command-line arguments, run
# `julia build_tarballs.jl --help` to see a usage message.
using BinaryBuilder, Pkg

# # See https://github.com/JuliaLang/Pkg.jl/issues/2942
# # Once this Pkg issue is resolved, this must be removed
# uuid = Base.UUID("a83860b7-747b-57cf-bf1f-3e79990d037f")
# delete!(Pkg.Types.get_last_stdlibs(v"1.6.3"), uuid)


name = "hydra"
version = v"0.2.0"

# Collection of sources required to complete build
sources = [
    GitSource("https://github.com/awietek/hydra.git", "c385004c73658dbd43b6c67fda51ec2a424817ee")
]

# Bash recipe for building across all platforms
script = raw"""
cd $WORKSPACE/srcdir
cd hydra

# Redefining BLAS_LAPACK symbols to use with OpenBLAS
SYMB_DEFS=()
for sym in sasum dasum snrm2 dnrm2 sdot ddot sgemv dgemv cgemv zgemv sgemm dgemm cgemm zgemm ssyrk dsyrk cherk zherk; do
    SYMB_DEFS+=("-D${sym}=${sym}_64")
done

for sym in cgbcon cgbsv cgbsvx cgbtrf cgbtrs cgecon cgees cgeev cgeevx cgehrd cgels cgelsd cgemm cgemv cgeqp3 cgeqrf cgesdd cgesv cgesvd cgesvx cgetrf cgetri cgetrs cgges cggev cgtsv cgtsvx cheev cheevd cherk clangb clange clanhe clansy cpbtrf cpocon cposv cposvx cpotrf cpotri cpotrs cpstrf ctrcon ctrsyl ctrtri ctrtrs cungqr dasum ddot dgbcon dgbsv dgbsvx dgbtrf dgbtrs dgecon dgees dgeev dgeevx dgehrd dgels dgelsd dgemm dgemv dgeqp3 dgeqrf dgesdd dgesv dgesvd dgesvx dgetrf dgetri dgetrs dgges dggev dgtsv dgtsvx dlahqr dlangb dlange dlansy dlarnv dnrm2 dorgqr dpbtrf dpocon dposv dposvx dpotrf dpotri dpotrs dpstrf dstedc dsyev dsyevd dsyrk dtrcon dtrevc dtrsyl dtrtri dtrtrs ilaenv sasum sdot sgbcon sgbsv sgbsvx sgbtrf sgbtrs sgecon sgees sgeev sgeevx sgehrd sgels sgelsd sgemm sgemv sgeqrf sgeqp3 sgesdd sgesv sgesvd sgesvx sgetrf sgetri sgetrs sgges sggev sgtsv sgtsvx slahqr slangb slange slansy slarnv snrm2 sorgqr spbtrf spocon sposv sposvx spotrf spotri spotrs spstrf sstedc ssyev ssyevd ssyrk strcon strevc strsyl strtri strtrs zgbcon zgbsv zgbsvx zgbtrf zgbtrs zgecon zgees zgeev zgeevx zgehrd zgels zgelsd zgemm zgemv zgeqp3 zgeqrf zgesdd zgesv zgesvd zgesvx zgetrf zgetri zgetrs zgges zggev zgtsv zgtsvx zheev zheevd zherk zlangb zlange zlanhe zlansy zpbtrf zpocon zposv zposvx zpotrf zpotri zpotrs zpstrf ztrcon ztrsyl ztrtri ztrtrs zungqr; do
    SYMB_DEFS+=("-D${sym}=${sym}_64")
done

export CXXFLAGS="${SYMB_DEFS[@]}"


if [[ "${target}" == *-apple-* ]]; then

    # Needed to get std::visit working  
    export MACOSX_DEPLOYMENT_TARGET=10.14


    # Finding OpenMP is a bit complicated
    cmake -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} -DCMAKE_BUILD_TYPE=Release -DHYDRA_JULIA_WRAPPER=On -DJlCxx_DIR=$prefix/lib/cmake -DBLAS_LIBRARIES=${libdir}/libopenblas64_.${dlext} -DLAPACK_LIBRARIES=${libdir}/libopenblas64_.${dlext} -DOpenMP_libgomp_LIBRARY=${libdir}/libgomp.dylib -DOpenMP_ROOT=${libdir} -D OpenMP_CXX_LIB_NAMES="libgomp" -DOpenMP_CXX_FLAGS="-fopenmp=libgomp -Wno-unused-command-line-argument"  -S . -B build

else

    cmake -DCMAKE_INSTALL_PREFIX=$prefix -DCMAKE_TOOLCHAIN_FILE=${CMAKE_TARGET_TOOLCHAIN} -DCMAKE_BUILD_TYPE=Release -DHYDRA_JULIA_WRAPPER=On -DJlCxx_DIR=$prefix/lib/cmake -DBLAS_LIBRARIES=${libdir}/libopenblas64_.${dlext} -DLAPACK_LIBRARIES=${libdir}/libopenblas64_.${dlext}-S . -B build

fi

cmake --build build -j${nproc}
cmake --install build

"""

# These are the platforms we will build for by default, unless further
# platforms are passed in on the command line
platforms = supported_platforms()

# The products that we will ensure are always built
products = [
    ExecutableProduct("hydrajl", :hydrajl)
]

# Dependencies that must be installed before this package can be built
dependencies = [
    Dependency(PackageSpec(name="libjulia_jll", uuid="5ad3ddd2-0711-543a-b040-befd59781bbf"))
    Dependency(PackageSpec(name="libcxxwrap_julia_jll", uuid="3eaa8342-bff7-56a5-9981-c04077f7cee7"))
    Dependency(PackageSpec(name="OpenBLAS_jll", uuid="4536629a-c528-5b80-bd46-f80d51c5b363"))
    Dependency(PackageSpec(name="HDF5_jll", uuid="0234f1f7-429e-5d53-9886-15a909be8d59"))
    Dependency(PackageSpec(name="CompilerSupportLibraries_jll", uuid="e66e0078-7015-5450-92f7-15fbd957f2ae"))
]

# Build the tarballs, and possibly a `build.jl` as well.
build_tarballs(ARGS, name, version, sources, script, platforms, products, dependencies; julia_compat="1.6", preferred_gcc_version = v"12.1.0")
