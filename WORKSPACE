load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# master as of 2019/9/15:
# https://github.com/grailbio/bazel-toolchain/commit/df0f2eb6fe698b4483bb2d5b5670c17ac69c6362
llvm_toolchain_commit = "df0f2eb6fe698b4483bb2d5b5670c17ac69c6362"

http_archive(
    name = "com_grail_bazel_toolchain",
    sha256 = "105bfaf1355fb1d98781000a2986fd50424e1923e951d75aa6d3bbec6dba6907",
    strip_prefix = "bazel-toolchain-" + llvm_toolchain_commit,
    urls = ["https://github.com/grailbio/bazel-toolchain/archive/{}.tar.gz".format(llvm_toolchain_commit)],
)

load("@com_grail_bazel_toolchain//toolchain:rules.bzl", "llvm_toolchain")

llvm_toolchain(
    name = "llvm_toolchain",
    llvm_version = "8.0.0",
)

load("@llvm_toolchain//:toolchains.bzl", "llvm_register_toolchains")

llvm_register_toolchains()
