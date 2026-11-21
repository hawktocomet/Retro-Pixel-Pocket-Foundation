# Contributing to Retro Pixel Pocket Foundation

Thank you for your interest in contributing.

Retro Pixel Pocket Foundation aims to make the FunnyPlaying Retro Pixel Pocket an understandable, customizable, open-source, and reproducible platform.

Contributions should make the platform more useful, maintainable, transparent, and accessible to other developers.

## Core principles

### Open development

- Prefer publicly accessible source code and documentation.
- Document the origin and version of third-party dependencies.
- Identify applicable licenses and redistribution restrictions.
- Do not introduce undocumented dependencies on personal files, private archives, or local machine state.
- Preserve required third-party notices and attribution.

### Reproducibility

A developer starting with a clean checkout should be able to follow documented instructions to obtain the required dependencies and reproduce the build.

Build instructions should identify:

- Host operating system and prerequisites
- Required toolchains and versions
- Dependency sources and retrieval instructions
- Configuration and build commands
- Expected outputs
- Any hardware-specific requirements

If a dependency cannot be publicly obtained or redistributed, document the limitation and explain the available alternatives.

### Upstream first

Where practical:

- Prefer upstream source projects over private copies.
- Contribute general fixes to their upstream projects.
- Keep local patches focused and documented.
- Record the upstream revision on which changes are based.

### Keep the Foundation independent

Foundation contributions must not introduce dependencies on Retro KiddOS or another particular frontend.

Shared interfaces should represent general device capabilities rather than application-specific behavior.

## Licensing and provenance

Before incorporating third-party code, firmware, binaries, patches, or other materials:

1. Identify the original source.
2. Record the version or revision.
3. Identify the applicable license.
4. Check modification and redistribution requirements.
5. Preserve required notices and attribution.
6. Document any restrictions or unresolved questions.

Do not assume that material found in a firmware image or vendor archive can be redistributed.

When licensing is unclear, raise the issue before incorporating the material into a distributable release.

## Code and documentation contributions

- Keep changes focused and reviewable.
- Follow the conventions of the component being modified.
- Document non-obvious hardware behavior and compatibility requirements.
- Include tests where practical.
- Verify changes against the actual target hardware when necessary.
- Clearly distinguish tested behavior from assumptions or incomplete work.

Documentation should explain not only what to do, but why a particular dependency or procedure is required.

## Reporting issues

When reporting a problem, include as much relevant information as possible:

- Device and hardware revision, if known
- Firmware or software version
- Host operating system and toolchain versions
- Steps to reproduce the issue
- Relevant logs and error messages
- Whether the problem occurs on stock firmware or a Foundation build

Avoid including personal information, credentials, or unrelated private files in reports.

## Development direction

The long-term goal is a firmware platform that developers can independently obtain, understand, build, modify, and extend.

Contributions that improve transparency, hardware compatibility, documentation, licensing clarity, and reproducibility are especially valuable.
