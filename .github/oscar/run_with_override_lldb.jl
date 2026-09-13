# Run Singular.jl's tests with a locally built Singular under LLDB.

length(ARGS) >= 1 || error("must provide the Singular override directory")
singular_override = abspath(popfirst!(ARGS))
isdir(singular_override) || error("invalid Singular override directory: $(singular_override)")

singularjl_path = abspath(ENV["SINGULARJL_PATH"])
isdir(singularjl_path) || error("invalid Singular.jl directory: $(singularjl_path)")

using Pkg

Pkg.activate(; temp=true)
Pkg.develop(path=singularjl_path)
Pkg.add("Aqua")
Pkg.add("Singular_jll")
Pkg.instantiate()

function add_jll_override(depot, pkgname, newdir)
    pkgid = Base.identify_package("$(pkgname)_jll")
    pkgid === nothing && error("package $(pkgname)_jll is not installed")

    mkpath(joinpath(depot, "artifacts"))
    open(joinpath(depot, "artifacts", "Overrides.toml"), "a") do io
        println(io, "[$(pkgid.uuid)]")
        println(io, "$(pkgname) = \"$(newdir)\"")
    end

    # Force libsingular_julia to be rebuilt with the overridden Singular.
    run(`touch $(Base.locate_package(pkgid))`)
end

tmpdepot = mktempdir(; cleanup=true)
add_jll_override(tmpdepot, "Singular", singular_override)

libsingular_src_dir = joinpath(singularjl_path, "deps", "src")
isdir(libsingular_src_dir) || error("could not find $(libsingular_src_dir)")

marker_prefix = ".recompile-libsingular-julia-"
for filename in readdir(libsingular_src_dir)
    startswith(filename, marker_prefix) || continue
    rm(joinpath(libsingular_src_dir, filename); force=true)
end

marker = joinpath(libsingular_src_dir, marker_prefix * string(time_ns()) * ".txt")
write(marker, "force recompilation marker\n")

singular_libdir = joinpath(singular_override, "lib")
dyld_fallback = let existing = get(ENV, "DYLD_FALLBACK_LIBRARY_PATH", "")
    isempty(existing) ? singular_libdir : existing * ":" * singular_libdir
end

try
    withenv(
        "JULIA_DEPOT_PATH" => tmpdepot * ":" * join(DEPOT_PATH, ":"),
        "DYLD_FALLBACK_LIBRARY_PATH" => dyld_fallback,
    ) do
        julia = `$(Base.julia_cmd()) --project=$(Base.active_project()) $(ARGS)`
        status_file = tempname()
        write(status_file, "1\n")
        status_command = "script import lldb; p = lldb.debugger.GetSelectedTarget().GetProcess(); open($(repr(status_file)), 'w').write(str(p.GetExitStatus() if p.GetState() == lldb.eStateExited else 1))"

        try
            lldb = run(ignorestatus(`lldb --batch --no-lldbinit
                                         -o run
                                         -o $status_command
                                         -k "process status"
                                         -k "thread backtrace all"
                                         -k "register read"
                                         -k "image list"
                                         -k "quit 1"
                                         -- $julia`))
            success(lldb) || error("LLDB failed with status $(lldb.exitcode)")

            julia_status = tryparse(Int, strip(read(status_file, String)))
            julia_status == 0 || error("debugged Julia failed with status $(something(julia_status, "unknown"))")
        finally
            rm(status_file; force=true)
        end
    end
finally
    rm(marker; force=true)
end
