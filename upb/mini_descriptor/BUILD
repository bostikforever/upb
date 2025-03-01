load(
    "//bazel:build_defs.bzl",
    "UPB_DEFAULT_COPTS",
    "UPB_DEFAULT_CPPOPTS",
)

cc_library(
    name = "internal",
    srcs = [
        "internal/base92.c",
    ],
    hdrs = [
        "internal/base92.h",
        "internal/decoder.h",
        "internal/modifiers.h",
        "internal/wire_constants.h",
    ],
    copts = UPB_DEFAULT_COPTS,
    visibility = ["//:__subpackages__"],
    deps = [
        "//:base",
        "//:base_internal",
        "//:port",
    ],
)

cc_library(
    name = "mini_descriptor",
    srcs = [
        "build_enum.c",
        "decode.c",
        "link.c",
    ],
    hdrs = [
        "build_enum.h",
        "decode.h",
        "link.h",
    ],
    copts = UPB_DEFAULT_COPTS,
    visibility = ["//:friends"],
    deps = [
        ":internal",
        "//:base",
        "//:mem",
        "//:mini_table",
        "//:mini_table_internal",
        "//:port",
    ],
)

cc_library(
    name = "encode_internal",
    srcs = [
        "internal/encode.c",
    ],
    hdrs = [
        "internal/encode.h",
        "internal/encode.hpp",
    ],
    copts = UPB_DEFAULT_COPTS,
    visibility = ["//:__subpackages__"],
    deps = [
        ":internal",
        "//:base",
        "//:base_internal",
        "//:port",
    ],
)

cc_test(
    name = "encode_test",
    srcs = ["internal/encode_test.cc"],
    copts = UPB_DEFAULT_CPPOPTS,
    deps = [
        ":encode_internal",
        ":internal",
        ":mini_descriptor",
        "@com_google_absl//absl/container:flat_hash_set",
        "//:upb",
        "//:message_accessors_internal",
        "//:mini_table",
        "@com_google_googletest//:gtest_main",
        "@com_google_protobuf//:protobuf",
    ],
)

# begin:github_only
filegroup(
    name = "source_files",
    srcs = glob(
        [
            "**/*.c",
            "**/*.h",
        ],
    ),
    visibility = [
        "//cmake:__pkg__",
        "//python/dist:__pkg__",
    ]
)
# end:github_only
