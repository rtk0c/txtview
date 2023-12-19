const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const exe = b.addExecutable(.{
        .name = "txtview",
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const mach_freetype = b.dependency("mach_freetype", .{
        .target = target,
        .optimize = optimize,
    });
    exe.addModule("mach-freetype", mach_freetype.module("mach-freetype"));
    exe.addModule("mach-harfbuzz", mach_freetype.module("mach-harfbuzz"));
    exe.linkLibrary(mach_freetype.artifact("mach-freetype"));
    exe.linkLibrary(mach_freetype.artifact("mach-harfbuzz"));

    b.installArtifact(exe);

    const run_app = b.addRunArtifact(exe);
    run_app.step.dependOn(b.getInstallStep());
    if (b.args) |args| {
        run_app.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_app.step);

    // Unit tests //

    const unit_tests = b.addTest(.{
        .root_source_file = .{ .path = "src/main.zig" },
        .target = target,
        .optimize = optimize,
    });

    const run_unit_tests = b.addRunArtifact(unit_tests);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_unit_tests.step);
}
