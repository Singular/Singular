# Copyright Spack Project Developers. See COPYRIGHT file for details.
#
# SPDX-License-Identifier: (Apache-2.0 OR MIT)

from spack_repo.builtin.build_systems.autotools import AutotoolsPackage
from spack.package import *


class Singular(AutotoolsPackage):
    """Computer algebra system for polynomial computations."""

    homepage = "https://www.singular.uni-kl.de/"
    url = (
        "https://www.singular.uni-kl.de/ftp/pub/Math/Singular/"
        "SOURCES/4-4-1/singular-4.4.1p5.tar.gz"
    )
    git = "https://github.com/Singular/Singular.git"

    license("GPL-2.0-or-later")

    version("develop", branch="spielwiese")
    version(
        "4.4.1p5",
        sha256="bce5a40bd10b6e9fe991de97e6284f62cdb566c8aef4b2836b4d1307eb7d9edf",
    )

    depends_on("c", type="build")
    depends_on("cxx", type="build")
    depends_on("autoconf", type="build")
    depends_on("automake", type="build")
    depends_on("bison", type="build")
    depends_on("libtool", type="build")
    depends_on("perl", type="build")
    depends_on("pkgconfig", type="build")

    depends_on("cddlib+gmp")
    depends_on("flint")
    depends_on("gmp")
    depends_on("mpfr")
    depends_on("ntl+gmp")
    depends_on("readline")

    def autoreconf(self, spec, prefix):
        bash = which("bash", required=True)
        bash("./autogen.sh")

    def configure_args(self):
        spec = self.spec
        return [
            "--enable-gfanlib",
            "--with-gmp={0}".format(spec["gmp"].prefix),
            "--with-ntl={0}".format(spec["ntl"].prefix),
            "--with-flint={0}".format(spec["flint"].prefix),
            "--with-readline=static",
            "--without-python",
            "--disable-python",
        ]
