const Builder = @import("std").build.Builder;

pub fn build(b: *Builder) void {
    const mode = b.standardReleaseOptions();
    const exe = b.addExecutable("bald_snake", "zig_version/main.zig");
    
    exe.linkSystemLibrary("SDL2");
    exe.linkSystemLibrary("c");
    exe.linkSystemLibrary("SDL2_TTF");
    exe.linkSystemLibrary("SDL2_image");

    exe.setBuildMode(mode);
    exe.install();

    const run_cmd = exe.run();
    run_cmd.step.dependOn(b.getInstallStep());

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);
}
