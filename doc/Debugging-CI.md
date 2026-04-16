Debugging GitHub CI runs {#debugging_ci_page}
========================

The `Run tests` workflow (`.github/workflows/runtests.yml`) runs
automatically on every push and pull request. It can also be invoked
manually ("workflow dispatch") from the GitHub web UI or from the
command line, with a few extra options that are useful for debugging a
CI failure or experimenting with a build configuration.

Triggering a manual run
-----------------------

From the web UI: open <https://github.com/Singular/Singular/actions>,
select `Run tests`, click `Run workflow`, and fill in the form.

From the command line (using [`gh`](https://cli.github.com/)):

    gh workflow run runtests.yml --ref <branch-to-run-from> \
        [-f key=value ...]

The `--ref` argument selects the branch whose workflow file is used
for the run. Unless overridden by inputs (below), the same ref is also
checked out and tested.

Available inputs
----------------

* `repo` — repository to check out (`owner/name`). Defaults to the
  repository the run is executing in. Useful for testing upstream
  branches from a fork without having to push them to the fork first.

* `ref` — branch, tag, or SHA to check out. Defaults to the ref the
  workflow was dispatched from. Lets a tmate-enabled workflow file on
  one branch debug a build of any other branch.

* `tmate` — if `true`, open an ssh-accessible shell on the runner
  (using [`mxschmitt/action-tmate`](https://github.com/mxschmitt/action-tmate))
  after the test steps finish, whether they passed or failed. Default
  `false`.

* `tmate_timeout_minutes` — how long to wait for an ssh connection
  before the session is killed. Default 10. Applies per matrix job, so
  a dispatch that no one connects to will cost at most
  `timeout × matrix-size` runner minutes.

* `tmate_limit_access_to_actor` — if `true` (the default), only ssh
  keys listed at `https://github.com/<dispatching-user>.keys` are
  accepted. If `false`, the session token printed in the live log is
  the only credential, and anyone with a GitHub account who can read
  the log can connect.

Why `nyc1.tmate.io`?
--------------------

GitHub-hosted runners sit behind NAT with no inbound connectivity, so
you cannot ssh to them directly. `tmate` solves this by having the
runner make an **outbound** connection to a public relay server
(`*.tmate.io`, operated by the tmate project) and register a session
there. Your ssh client then connects to the relay, which proxies the
bytes back down the runner's pre-established connection. The relay is
a third-party rendezvous point; the session contents pass through it.

If the tmate project's relay is not an acceptable trust dependency,
`mxschmitt/action-tmate` supports pointing at a self-hosted
[`tmate-ssh-server`](https://github.com/tmate-io/tmate-ssh-server)
instance via its `tmate-server-host`, `tmate-server-port`, and
fingerprint inputs. This workflow does not expose those inputs
currently; add them to `runtests.yml` if needed.

Connecting to a tmate session
-----------------------------

The ssh connection string (`ssh <token>@nyc1.tmate.io` or similar) is
printed every 5 seconds in the live log of the `Create ssh target on
test runner for debugging` step. Copy it from there.

On first connect, tmate shows a read-only info window with the session
addresses. Press `q` (or `Ctrl-c`) to dismiss it and drop into the
shell.

When you are done:

* `touch /continue && exit` — lets the remaining workflow steps run
  and marks the step successful.
* `touch /fail && exit` — ends the session and marks the step failed.
* plain `exit` — ends your ssh session, but the action keeps polling
  until the timeout or a `/continue` sentinel. Prefer one of the
  above.

The live log of a GitHub Actions run requires an authenticated GitHub
account (any account, not just a repo collaborator) to view. API
access to a job's log is blocked until the job completes, so the
ssh string cannot be scraped programmatically during the session.

Security considerations
-----------------------

When `tmate_limit_access_to_actor` is `false`, the session token in
the log is a bearer credential. Anyone who sees the live log while
the session is open can connect to the runner and read its
environment, including `GITHUB_TOKEN`. The default (`true`) adds a
public-key check on top of the token, so a leaked log alone is not
sufficient to connect — the attacker would also need a matching
private key.

Note that `tmate` upstream is no longer maintained and the [Homebrew
formula](https://formulae.brew.sh/formula/tmate) is scheduled to be
disabled on 2026-12-11. When that happens the macOS jobs will no
longer be able to install `tmate` and this workflow will need to
switch to a maintained alternative.

See also
--------

* [mxschmitt/action-tmate](https://github.com/mxschmitt/action-tmate) —
  the underlying action, including documentation for inputs not
  exposed by this workflow (e.g. custom tmate relay host).
