# Run Pkg's Singular.jl test subprocess under LLDB.

using Pkg

const singularjl_path = abspath(ENV["SINGULARJL_PATH"])
isdir(singularjl_path) || error("invalid Singular.jl directory: $(singularjl_path)")

struct LLDBRunComplete <: Exception end

function run_under_lldb(flags::Cmd)
    code = Pkg.Operations.gen_test_code(singularjl_path; test_args = ``)
    threads = `--threads=$(Pkg.Operations.get_threads_spec())`
    code_env = "SINGULARJL_PKG_TEST_CODE"
    eval_code = "include_string(Main, ENV[$(repr(code_env))], \"Pkg.test\")"
    julia = `$(Base.julia_cmd()) $threads $flags --eval $eval_code`
    @info "Launching Pkg test process under LLDB" julia

    status_file = tempname()
    write(status_file, "1\n")
    status_command = "script import lldb; p = lldb.debugger.GetSelectedTarget().GetProcess(); open($(repr(status_file)), 'w').write(str(p.GetExitStatus() if p.GetState() == lldb.eStateExited else 1))"

    try
        lldb = withenv(code_env => code) do
            run(ignorestatus(`lldb --batch --no-lldbinit
                                  -o "settings set target.disable-aslr false"
                                  -o "process launch"
                                  -o $status_command
                                  -k $status_command
                                  -k "process status"
                                  -k "thread backtrace all"
                                  -k "register read"
                                  -k "image list"
                                  -k "quit 1"
                                  -- $julia`))
        end

        julia_status = tryparse(Int, strip(read(status_file, String)))
        julia_status == 0 || error(
            "debugged Julia failed with status $(something(julia_status, "unknown"))",
        )
        success(lldb) || error("LLDB failed with status $(lldb.exitcode)")
        return lldb, false
    finally
        rm(status_file; force = true)
    end
end

function run_pkg_test_under_lldb()
    ctx = Pkg.Types.Context()
    pkg = Pkg.Types.PackageSpec(name = "Singular")
    failures = Tuple{String, Base.Process}[]

    Pkg.Operations.precompile_and_run_tests!(
        ctx,
        pkg,
        singularjl_path,
        false,
        ``,
        failures;
        run_subprocess = (_, flags) -> run_under_lldb(flags),
    )
    isempty(failures) || error("Singular.jl tests failed under LLDB")

    # Pkg.test would otherwise launch its usual test subprocess after this callback.
    throw(LLDBRunComplete())
end

try
    Pkg.test("Singular"; test_fn = run_pkg_test_under_lldb)
catch err
    err isa LLDBRunComplete || rethrow()
end
