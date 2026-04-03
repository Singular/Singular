using Test
using Oscar

function _oscar_test_dir()::String
    pkgfile = Base.find_package("Oscar")
    pkgfile === nothing && error("Oscar not found in the active project")
    root = dirname(dirname(pkgfile))
    testdir = joinpath(root, "test")
    isdir(testdir) || error("Oscar test directory not found at: $testdir")
    return testdir
end

function _selected_tests()::Vector{String}
    sel = strip(get(ENV, "OSCAR_SELECTED_TESTS", ""))
    isempty(sel) && return ["Rings/mpoly.jl"]
    return split(sel)
end

testdir = _oscar_test_dir()
tests = _selected_tests()

@info "Running selected Oscar upstream tests" tests=tests

for rel in tests
    path = joinpath(testdir, rel)
    isfile(path) || error("Selected Oscar test file does not exist: $path")
    @info "Including" path
    include(path)
end
