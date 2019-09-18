common_defines = [
    "MESHING_ENABLED=1",
    "SHUFFLE_ON_FREE=1",
    "SHUFFLE_ON_INIT=1",
] + select({
    "@bazel_tools//src/conditions:linux_x86_64": [
        # "_FORTIFY_SOURCE=2",  # TODO: only in release
        "_DEFAULT_SOURCE",
        "_BSD_SOURCE",
        "_XOPEN_SOURCE=700",
    ],
})

common_copts = [
    # this doesn't seem to work -- both this and -std=c++11 end up on the commandline
    "-std=c++14",
    "-flto",
    "-Isrc/vendor/Heap-Layers",
    # TODO: have config option to disable this
    "-march=westmere",
    "-mavx",
    # general good stuff for malloc-y libs
    "-fPIC",
    "-fno-builtin-malloc",
    # "-ffunction-sections",
    # "-fdata-sections",
    "-Werror=pointer-arith",
    # warn on lots of stuff; this is cargo-culted from the old Make build system
    "-Wall",
    "-Wextra",
    "-pedantic",
    "-Wno-unused-parameter",
    "-Wno-unused-variable",
    "-Woverloaded-virtual",
    "-Werror=return-type",
    "-Wtype-limits",
    "-Wempty-body",
    "-Winvalid-offsetof",
    "-Wvariadic-macros",
    "-Wcast-align",
    # default to hidden visibility for symbols; we will explicitly export what we want
    "-fvisibility=hidden",
    # "-Wa,--noexecstack", # TODO: only for gcc
] + select({
    "@bazel_tools//src/conditions:linux_x86_64": [
        "-Wundef",
    ],
})

common_linkopts = [
    # TODO: only in release/opt build
    "-O3",
    "-flto",
    "-lm",
    "-lpthread",
    "-ldl",
    "-march=westmere",
    # default to hidden visibility for symbols; we will explicitly export what we want
    "-fvisibility=hidden",
] + select({
    "@bazel_tools//src/conditions:linux_x86_64": [
        "-Wl,--no-as-needed",
        "-Wl,--no-add-needed",
        "-Wl,--sort-common",
        # "-Wl,--gc-sections", # set by toolchain on --opt
        "-Wl,--hash-style=both",
        "-Wl,--no-undefined",
        "-Wl,-Bsymbolic-functions",
        "-Wl,-z,now,-z,relro",
        "-ftls-model=initial-exec",
        "-lrt",
    ],
})

cc_library(
    name = "mesh-core",
    srcs = [
        "d_assert.cc",
        "global_heap.cc",
        "measure_rss.cc",
        "meshable_arena.cc",
        "real.cc",
        "runtime.cc",
        "thread_local_heap.cc",
    ],
    hdrs = glob([
        "*.h",
        "plasma/*.h",
        "rng/*.h",
        "gnu_wrapper.cc",
        "static/*.h",
        "size_classes.def",
        "vendor/Heap-Layers/**/*.h",
    ]),
    copts = common_copts,
    defines = common_defines,
    linkopts = common_linkopts,
    linkstatic = True,
    visibility = ["//visibility:private"],
)

cc_library(
    name = "mesh-lib",
    srcs = [
        "libmesh.cc",
    ],
    hdrs = glob([
        "*.h",
        "plasma/*.h",
        "rng/*.h",
        "gnu_wrapper.cc",
        "static/*.h",
        "size_classes.def",
        "vendor/Heap-Layers/**/*.h",
        "vendor/Heap-Layers/wrappers/gnuwrapper-hooks.cpp",
        "vendor/Heap-Layers/wrappers/macwrapper.cpp",
        "wrapper.cc",
    ]),
    # TODO: config options here too
    copts = common_copts,
    defines = common_defines,
    linkopts = common_linkopts,
    visibility = ["//visibility:private"],
)

cc_binary(
    name = "libmesh.so",
    copts = common_copts,
    defines = common_defines,
    linkopts = common_linkopts,
    linkshared = True,
    visibility = ["//visibility:public"],
    deps = [
        ":mesh-core",
        ":mesh-lib",
    ],
)

cc_test(
    name = "units",
    size = "small",
    srcs = glob([
        "unit/*.cc",
    ]),
    copts = [
        "-Isrc",
        "-Iexternal/gtest/include",
    ] + common_copts,
    linkstatic = True,
    visibility = ["//tools:__pkg__"],
    deps = [
        ":mesh-core",
        "@com_google_googletest//:gtest",
        "@com_google_googletest//:gtest_main",
    ],
)
