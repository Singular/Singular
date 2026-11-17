using Pkg
using Artifacts
using JLLPrefixes
using Base.BinaryPlatforms

# JLLPrefixes normally reconstructs Julia stdlib JLLs through General. A Julia
# patch release can bundle a newer packaging revision before General indexes it.
# In that case, use older metadata only to discover the dependency graph, then
# replace every stdlib path and JLL edge with the exact artifacts bundled by the
# running Julia.
_singular_ci_stdlib_version(info) = hasproperty(info, :version) ? info.version : info[2]

function _singular_ci_use_current_stdlib_artifacts!(artifact_metas, platform)
  current_stdlibs = Pkg.Types.get_last_stdlibs(VERSION)
  available_uuids = Set(dep.uuid for dep in keys(artifact_metas))
  replaced = String[]

  for (dep, meta) in artifact_metas
    current_info = get(current_stdlibs, dep.uuid, nothing)
    current_info === nothing && continue
    current_version = _singular_ci_stdlib_version(current_info)

    source = Base.locate_package(Base.PkgId(dep.uuid, dep.name))
    source === nothing && error("Unable to locate bundled stdlib $(dep.name)")
    stdlib_dir = dirname(dirname(source))
    artifacts_toml = joinpath(stdlib_dir, "StdlibArtifacts.toml")
    isfile(artifacts_toml) || error("Bundled stdlib $(dep.name) has no StdlibArtifacts.toml")

    selected = Pkg.Artifacts.select_downloadable_artifacts(
      artifacts_toml; platform, pkg_uuid=dep.uuid
    )
    isempty(selected) && error("Bundled stdlib $(dep.name) has no artifact for $(platform)")
    paths = String[]
    for (name, artifact) in selected
      Pkg.Artifacts.ensure_artifact_installed(name, artifact, artifacts_toml; platform)
      push!(paths, artifact_path(Base.SHA1(artifact["git-tree-sha1"])))
    end

    project = Pkg.TOML.parsefile(joinpath(stdlib_dir, "Project.toml"))
    dep_uuids = Base.UUID[
      Base.UUID(uuid) for (name, uuid) in get(project, "deps", Dict())
      if endswith(name, "_jll")
    ]
    missing_uuids = setdiff(dep_uuids, available_uuids)
    isempty(missing_uuids) || error(
      "Bundled stdlib $(dep.name) has JLL dependencies absent from the fallback closure: " *
      join(string.(missing_uuids), ", ")
    )

    meta["paths"] = paths
    meta["dep_uuids"] = dep_uuids
    if dep.version != current_version
      push!(replaced, "$(dep.name) $(dep.version) -> $(current_version)")
    end
  end
  return replaced
end

function singular_ci_collect_artifact_paths(deps)
  try
    return collect_artifact_paths(deps)
  catch err
    if !isempty(VERSION.prerelease) || VERSION.patch == 0 ||
       !occursin("Unable to find dependency", sprint(showerror, err))
      rethrow()
    end

    specs = [Pkg.PackageSpec(; name) for name in deps]
    last_error = err
    for patch in (Int(VERSION.patch) - 1):-1:0
      platform = HostPlatform()
      julia_version = string(VERSION.major, ".", VERSION.minor, ".", patch)
      platform["julia_version"] = julia_version
      local artifact_metas
      try
        artifact_metas = collect_artifact_metas(specs; platform)
      catch candidate_error
        if occursin("Unable to find dependency", sprint(showerror, candidate_error))
          last_error = candidate_error
          continue
        end
        rethrow()
      end

      replaced = _singular_ci_use_current_stdlib_artifacts!(artifact_metas, HostPlatform())
      isempty(replaced) && error(
        "Refusing JLL fallback: no current Julia stdlib artifact replaced fallback metadata"
      )
      warning = "Using current bundled stdlib artifacts with compatible fallback metadata"
      @warn warning julia_version replaced exception = err
      return JLLPrefixes.collect_artifact_paths(artifact_metas, specs)
    end
    throw(last_error)
  end
end
