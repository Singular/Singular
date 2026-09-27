# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack_repo.builtin.build_systems.autotools import AutotoolsPackage
from spack.package import *


class Spasm(AutotoolsPackage):
    """Sparse direct Solver Modulo p."""

    homepage = "https://github.com/cbouilla/spasm"
    url = "https://github.com/cbouilla/spasm/archive/refs/tags/v1.2.tar.gz"

    license("GPL-3.0-or-later")

    version(
        "1.2",
        sha256="e01947316c177ac2084a4fe587e06f33377a196f711613e9d4a7b1b1c7cec000",
    )

    depends_on("c", type="build")
    depends_on("cxx", type="build")
    depends_on("autoconf", type="build")
    depends_on("automake", type="build")
    depends_on("libtool", type="build")

    def autoreconf(self, spec, prefix):
        autoreconf = which("autoreconf", required=True)
        autoreconf("-fi")

    def configure_args(self):
        return ["--disable-openmp"]

    def build(self, spec, prefix):
        make("-C", "src")

    def install(self, spec, prefix):
        make("-C", "src", "install")
