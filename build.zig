const std = @import("std");

const src_files = [_][]const u8{
    // "src/lib.cpp"
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    // Lib
    // =============================
    const lib = b.addStaticLibrary(.{
        .name = "mu",
        .target = target,
        .optimize = optimize,
    });
    lib.linkLibCpp();
    lib.addIncludePath(b.path("include"));
    lib.addCSourceFiles(.{
        .files = &src_files,
        .flags = &.{
            "-std=c++20",
            "-Wall",
            "-Wextra",
            "-Werror",
            "-Weverything",
            "-Wpedantic",
            "-Wshadow",
        },
    });
    b.installArtifact(lib);

    // // Tests
    // // =============================
    // const lib_unit_tests = b.addTest(.{
    //     .target = target,
    //     .optimize = optimize,
    // });
    // const run_lib_unit_tests = b.addRunArtifact(lib_unit_tests);
    // const test_step = b.step("test", "Run unit tests");
    // test_step.dependOn(&run_lib_unit_tests.step);
}
