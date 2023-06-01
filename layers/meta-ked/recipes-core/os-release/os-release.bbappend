OS_RELEASE_FIELDS="ID NAME VERSION BUILD_ID BUILD_VERSION BUILD_USER BUILD_HOST BUILD_BB_VERSION BUILD_GCC_VERSION BUILD_GLIBC_VERSION"

BUILD_DIR="${TOPDIR}"
BUILD_BB_VERSION="${BB_VERSION}"
BUILD_GCC_VERSION="${GCCVERSION}"
BUILD_GLIBC_VERSION="${GLIBCVERSION}"

do_compile[nostamp]="1"

python do_compile:prepend() {
    import subprocess
    import os

    builddir=d.getVar('BUILD_DIR', True)
    szGitCmd="git --git-dir=%s/.git rev-parse --verify --short HEAD" % builddir

    process = subprocess.Popen(szGitCmd, shell=True, stdout=subprocess.PIPE)
    out = process.communicate()[0].decode("utf-8")
    out = out[:-1]
    d.setVar("BUILD_VERSION", out)

    process = subprocess.Popen("whoami", shell=True, stdout=subprocess.PIPE)
    out = process.communicate()[0].decode("utf-8")
    out = out[:-1]
    d.setVar("BUILD_USER", out)

    process = subprocess.Popen("hostname", shell=True, stdout=subprocess.PIPE)
    out = process.communicate()[0].decode("utf-8")
    out = out[:-1]
    d.setVar("BUILD_HOST", out)
}
