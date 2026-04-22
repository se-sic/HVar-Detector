configure_lit_site_cfg(
        ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.in
        ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg
        MAIN_CONFIG
        ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg
)

add_lit_testsuite(
        check-hidden-var-finder "Running Hidden Variability regression tests"
        ${CMAKE_CURRENT_BINARY_DIR}
)