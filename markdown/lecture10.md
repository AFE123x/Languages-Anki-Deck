# 10. Licensing Report

## Buildroot - license compliance

- The key aspect of embedded linux is license compliance.
- embedded linux will usually integrate a number of open source components, with their own license.
- Each license will have different requirements which need to be met before the linux system starts shipping.
- buildroot offers a license compliance tool.

- the license report can be generated with `make legal-info`

## `.mk` - license variables

- `<pkg>_LICENSE`: comma separated list of licenses.must use SPDX license codes
    - you can also indicate which part of the package is under the license (programs, tests, libraries, etc.)
- `<pkg>_LICENSE_FILES`: space separated list of file paths from package source code containing the license text and copyright information.
- `<pkg>_REDISTRIBUTE`: indicated whether the package source code can be redistributed or not.
    - default is YES, but can be overwritten to NO.