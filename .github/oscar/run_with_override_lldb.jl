# Run Singular.jl's tests with a locally built Singular under LLDB.

length(ARGS) >= 1 || error("must provide the Singular override directory")
singular_override = abspath(popfirst!(ARGS))
isdir(singular_override) || error("invalid Singular override directory: $(singular_override)")

singularjl_path = abspath(ENV["SINGULARJL_PATH"])
isdir(singularjl_path) || error("invalid Singular.jl directory: $(singularjl_path)")

using Pkg

Pkg.activate(; temp=true)
Pkg.develop(path=singularjl_path)
Pkg.add(Pkg.PackageSpec(name="Aqua", version="0.8.2"))
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
        run(`lldb --batch --no-lldbinit
                  -o run
                  -k "process status"
                  -k "thread backtrace all"
                  -k "register read"
                  -k "image list"
                  -- $julia`)
    end
finally
    rm(marker; force=true)
end
