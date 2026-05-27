# Patch Stack Workflow

This fork keeps local changes as a small commit stack on top of upstream.
Each commit should be reviewable as an independent patch.

The source of truth is the branch history:

```text
upstream/main + local patch commits = main
```

Use `scripts/patch-stack` instead of replaying commits by hand.

Patch files can be exported locally to `.patches/fcitx5-lotus-flower/`, but that
directory is only a local cache. It is intentionally not tracked because storing
generated patch files inside the same patch stack creates self-referential
patches on the next export.

## Patch Standards

- Keep one commit to one intent. A commit may touch multiple files when that is
  required for the same feature.
- Avoid format-only commits before feature patches. Formatting churn makes later
  patches depend on non-functional context and increases cascade conflicts.
- If two commits modify the same code path, either merge them or make the order
  intentional and documented by the commit messages.
- Keep generated or high-conflict files, such as `po/*.po` and `po/*.pot`, in a
  separate patch near the end of the stack. This keeps conflicts localized
  during future upstream syncs.
- Put docs and packaging patches after code patches unless they are required to
  build or test the code patch.

Before pushing a rewritten stack, run:

```bash
scripts/patch-stack check
```

This performs a dry-run replay in a temporary Git worktree, then tests patch pairs
that touch at least one common file. For each pair, it tries moving the later
patch before the earlier patch and moving the earlier patch after the later
patch. The output shows whether the pair is adjacent or farther apart in the
stack. A failed move means the two patches depend on their current order. Use
the suggested `git diff` commands in the output to inspect whether the
dependency is intentional.

By default, dependency findings are warnings. To make them fail the command:

```bash
scripts/patch-stack check --strict
```

For a noisier audit that also reports every file touched by multiple patches:

```bash
scripts/patch-stack check --verbose
```

## Sync With Upstream

Start from a clean worktree:

```bash
git status --short
```

Replay the shared patch stack on top of the latest upstream:

```bash
scripts/patch-stack reapply
```

If all commits apply cleanly, replace `main` with the replayed branch and export
a local patch cache:

```bash
scripts/patch-stack finish
```

Then push the rewritten fork branch:

```bash
git push --force-with-lease origin main
```

## Resolve Conflicts

If `git cherry-pick` stops with a conflict during `scripts/patch-stack reapply`,
resolve the conflict in the affected files, then continue:

```bash
git status
git add <files>
git cherry-pick --continue
```

If upstream already contains a commit or the patch is no longer needed:

```bash
git cherry-pick --skip
```

To cancel the replay:

```bash
git cherry-pick --abort
git switch main
```

After conflicts are resolved and all commits apply, run:

```bash
scripts/patch-stack finish
```

## Export Patch Files

To export the current stack as patch files for inspection or sharing outside
Git:

```bash
scripts/patch-stack export
```

The files are written to:

```text
.patches/fcitx5-lotus-flower/
```

Keep generated or high-conflict files, such as `po/*.po` and `po/*.pot`, in a
separate patch near the end of the stack when possible. This keeps conflicts
localized during future upstream syncs.
