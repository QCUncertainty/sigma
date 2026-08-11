/*
 @licstart  The following is the entire license notice for the JavaScript code in this file.

 The MIT License (MIT)

 Copyright (C) 1997-2020 by Dimitri van Heesch

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute,
 sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all copies or
 substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 @licend  The above is the entire license notice for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "sigma", "index.html", [
    [ "Statement of Need", "index.html#statement-of-need", null ],
    [ "Features", "index.html#features", null ],
    [ "Installation", "index.html#installation", null ],
    [ "Contributing", "index.html#contributing", null ],
    [ "Acknowledgments", "index.html#acknowledgments", null ],
    [ "Affine Arithmetic", "md_docs_2affine.html", [
      [ "Motivation", "md_docs_2affine.html#motivation", null ],
      [ "The affine form", "md_docs_2affine.html#the-affine-form", null ],
      [ "Why sharing symbols solves the dependency problem", "md_docs_2affine.html#why-sharing-symbols-solves-the-dependency-problem", null ],
      [ "Exact operations", "md_docs_2affine.html#exact-operations", null ],
      [ "Where linearization enters", "md_docs_2affine.html#where-linearization-enters", null ],
      [ "Constructing the transform", "md_docs_2affine.html#constructing-the-transform", null ],
      [ "What is gained, and what remains", "md_docs_2affine.html#what-is-gained-and-what-remains", null ],
      [ "Thresholding", "md_docs_2affine.html#thresholding", null ],
      [ "Class design", "md_docs_2affine.html#class-design", null ],
      [ "Next", "md_docs_2affine.html#next", null ]
    ] ],
    [ "Background", "md_docs_2background.html", [
      [ "Linear Error Propagation", "md_docs_2background.html#linear-error-propagation", null ]
    ] ],
    [ "Contributor Covenant Code of Conduct", "md_docs_2code__of__conduct.html", [
      [ "Our Pledge", "md_docs_2code__of__conduct.html#our-pledge", null ],
      [ "Our Standards", "md_docs_2code__of__conduct.html#our-standards", null ],
      [ "Enforcement Responsibilities", "md_docs_2code__of__conduct.html#enforcement-responsibilities", null ],
      [ "Scope", "md_docs_2code__of__conduct.html#scope", null ],
      [ "Enforcement", "md_docs_2code__of__conduct.html#enforcement", null ],
      [ "Enforcement Guidelines", "md_docs_2code__of__conduct.html#enforcement-guidelines", [
        [ "1. Correction", "md_docs_2code__of__conduct.html#autotoc_md1-correction", null ],
        [ "2. Warning", "md_docs_2code__of__conduct.html#autotoc_md2-warning", null ],
        [ "3. Temporary Ban", "md_docs_2code__of__conduct.html#autotoc_md3-temporary-ban", null ],
        [ "4. Permanent Ban", "md_docs_2code__of__conduct.html#autotoc_md4-permanent-ban", null ]
      ] ],
      [ "Attribution", "md_docs_2code__of__conduct.html#attribution", null ]
    ] ],
    [ "Contributor Guidelines", "md_docs_2contributing.html", [
      [ "Asking Questions", "md_docs_2contributing.html#asking-questions", null ],
      [ "Ways to Contribute", "md_docs_2contributing.html#ways-to-contribute", null ],
      [ "Code of Conduct", "md_docs_2contributing.html#code-of-conduct", null ],
      [ "Acknowledgments", "md_docs_2contributing.html#acknowledgments-1", null ]
    ] ],
    [ "Examples", "md_docs_2examples.html", [
      [ "Construction of Uncertain Variables", "md_docs_2examples.html#construction-of-uncertain-variables", null ],
      [ "Element Access", "md_docs_2examples.html#element-access", null ],
      [ "Equality and Comparison", "md_docs_2examples.html#equality-and-comparison", null ],
      [ "Mathematical Operations", "md_docs_2examples.html#mathematical-operations", null ],
      [ "Linear Algebra", "md_docs_2examples.html#linear-algebra", null ]
    ] ],
    [ "Interval Arithmetic", "md_docs_2interval.html", [
      [ "Motivation", "md_docs_2interval.html#motivation-1", null ],
      [ "Definition", "md_docs_2interval.html#definition", null ],
      [ "Arithmetic", "md_docs_2interval.html#arithmetic", null ],
      [ "Elementary functions", "md_docs_2interval.html#elementary-functions", null ],
      [ "Two fundamental limitations", "md_docs_2interval.html#two-fundamental-limitations", null ],
      [ "Class design", "md_docs_2interval.html#class-design-1", null ],
      [ "Next", "md_docs_2interval.html#next-1", null ]
    ] ],
    [ "Building the Documentation", "md_docs_2README.html", [
      [ "Prerequisites", "md_docs_2README.html#prerequisites", null ],
      [ "Building the documentation", "md_docs_2README.html#building-the-documentation-1", null ],
      [ "Adding a page", "md_docs_2README.html#adding-a-page", null ],
      [ "Conventions", "md_docs_2README.html#conventions", null ],
      [ "Two failure modes that produce no warning", "md_docs_2README.html#two-failure-modes-that-produce-no-warning", null ]
    ] ],
    [ "Taylor Models", "md_docs_2taylor.html", [
      [ "Motivation", "md_docs_2taylor.html#motivation-2", null ],
      [ "The Taylor model", "md_docs_2taylor.html#the-taylor-model", null ],
      [ "Truncation and range bounding", "md_docs_2taylor.html#truncation-and-range-bounding", [
        [ "Naive baseline", "md_docs_2taylor.html#naive-baseline", null ],
        [ "Horner form", "md_docs_2taylor.html#horner-form", null ],
        [ "Dominated and fast bounders", "md_docs_2taylor.html#dominated-and-fast-bounders", null ]
      ] ],
      [ "Arithmetic", "md_docs_2taylor.html#arithmetic-1", null ],
      [ "Elementary functions", "md_docs_2taylor.html#elementary-functions-1", null ],
      [ "Order scaling", "md_docs_2taylor.html#order-scaling", null ],
      [ "Design of the <span class=\"tt\">Taylor</span> class", "md_docs_2taylor.html#design-of-the-taylor-class", [
        [ "Internal state", "md_docs_2taylor.html#internal-state", null ],
        [ "Interface", "md_docs_2taylor.html#interface", null ]
      ] ],
      [ "Achieving rigorous bounds", "md_docs_2taylor.html#achieving-rigorous-bounds", null ],
      [ "Future work", "md_docs_2taylor.html#future-work", null ]
    ] ],
    [ "Namespaces", "namespaces.html", [
      [ "Namespace List", "namespaces.html", "namespaces_dup" ],
      [ "Namespace Members", "namespacemembers.html", [
        [ "All", "namespacemembers.html", null ],
        [ "Functions", "namespacemembers_func.html", null ],
        [ "Variables", "namespacemembers_vars.html", null ],
        [ "Typedefs", "namespacemembers_type.html", null ]
      ] ]
    ] ],
    [ "Classes", "annotated.html", [
      [ "Class List", "annotated.html", "annotated_dup" ],
      [ "Class Index", "classes.html", null ],
      [ "Class Hierarchy", "hierarchy.html", "hierarchy" ],
      [ "Class Members", "functions.html", [
        [ "All", "functions.html", "functions_dup" ],
        [ "Functions", "functions_func.html", "functions_func" ],
        [ "Variables", "functions_vars.html", null ],
        [ "Typedefs", "functions_type.html", null ],
        [ "Related Symbols", "functions_rela.html", null ]
      ] ]
    ] ],
    [ "Files", "files.html", [
      [ "File List", "files.html", "files_dup" ],
      [ "File Members", "globals.html", [
        [ "All", "globals.html", null ],
        [ "Macros", "globals_defs.html", null ]
      ] ]
    ] ]
  ] ]
];

var NAVTREEINDEX =
[
"affine_2eigen__compat_8hpp.html",
"classsigma_1_1Taylor.html#a07ed581795d9bb95df704c39ba169b13",
"classsigma_1_1ThresholdedAffine.html#a298214f80037d9585cbc4e09f8eee5b3",
"index.html#statement-of-need",
"structEigen_1_1NumTraits_3_01sigma_1_1Interval_3_01double_01_4_01_4.html#a34ec8dc7ffe77bd62f205e119febece8"
];

var SYNCONMSG = 'click to disable panel synchronization';
var SYNCOFFMSG = 'click to enable panel synchronization';
var LISTOFALLMEMBERS = 'List of all members';