// Appended after the MathJax.Hub.Config block that Doxygen generates, so this
// has to be a complete statement rather than a bare property. MathJax merges
// successive Hub.Config calls, so this augments the generated configuration.
//
// autoNumber "AMS" numbers equation/align environments automatically, which
// keeps pages from numbering equations by hand and keeps \eqref in sync.
// Numbering restarts on each generated HTML page.
MathJax.Hub.Config({
    TeX: {
        equationNumbers: {
            autoNumber: "AMS",
            useLabelIds: false
        }
    }
});
