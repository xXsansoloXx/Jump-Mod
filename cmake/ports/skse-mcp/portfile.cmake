# header-only library
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO Quantumyilmaz/SKSE-MCP
    REF 4cf7951c91509d495f38b62f90abb67c5cdfa524
    SHA512 6756a9116511b09b1f684283152d53fc4c9ff07fc539bb9085f606fce5b54f4b70dd42cc5b9c1d9d0dd2f4bce98c743a3a66991b725f449e9aa29b53a3f2a8fc
    HEAD_REF main
)

# Install codes
set(SKSEMCP_SOURCE	${SOURCE_PATH}/include/SKSEMCP)
file(INSTALL ${SKSEMCP_SOURCE} DESTINATION ${CURRENT_PACKAGES_DIR}/include)
vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE")