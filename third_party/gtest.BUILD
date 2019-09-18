# Adapted from Sorbet, adapted from https://github.com/mzhaom/trunk/blob/master/third_party/gtest/BUILD
licenses(["notice"])

cc_library(
    name = "gtest",
    testonly = 1,
    srcs = glob([
        "googletest/include/gtest/internal/**/*.h",
    ]) + [
        "googletest/src/gtest-internal-inl.h",
        "googletest/src/gtest-death-test.cc",
        "googletest/src/gtest-filepath.cc",
        "googletest/src/gtest-port.cc",
        "googletest/src/gtest-printers.cc",
        "googletest/src/gtest-test-part.cc",
        "googletest/src/gtest-typed-test.cc",
        "googletest/src/gtest.cc",
    ],
    hdrs = [
        "googletest/include/gtest/gtest.h",
        "googletest/include/gtest/gtest-death-test.h",
        "googletest/include/gtest/gtest-message.h",
        "googletest/include/gtest/gtest-param-test.h",
        "googletest/include/gtest/gtest-printers.h",
        "googletest/include/gtest/gtest-spi.h",
        "googletest/include/gtest/gtest-test-part.h",
        "googletest/include/gtest/gtest-typed-test.h",
        "googletest/include/gtest/gtest_pred_impl.h",
    ],
    copts = [
        "-Iexternal/gtest/googletest",
    ],
    includes = [
        "googletest/include",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":gtest_prod",
    ],
    linkstatic = select({
            "@com_stripe_ruby_typer//tools/config:linkshared": 0,
            "//conditions:default": 1,
        }),
)

cc_library(
    name = "gtest_main",
    testonly = 1,
    srcs = [
        "googletest/src/gtest_main.cc",
    ],
    visibility = ["//visibility:public"],
    deps = [
        ":gtest",
    ],
    # linkstatic = select({
    #         "@com_stripe_ruby_typer//tools/config:linkshared": 0,
    #         "//conditions:default": 1,
    #     }),
)

cc_library(
    name = "gtest_prod",
    hdrs = [
        "googletest/include/gtest/gtest_prod.h",
    ],
    visibility = ["//visibility:public"],
    # linkstatic = select({
    #         "@com_stripe_ruby_typer//tools/config:linkshared": 0,
    #         "//conditions:default": 1,
    #     }),
)
