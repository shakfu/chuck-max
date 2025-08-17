//---------------------------------------------------------------------
// name: gen-ckdoc.ck
// desc: generate HTML documentation for Chumpinate
//
// author: Nick Shaheed (https://www.nicholasshaheed.com)
//   date: Fall 2024
//---------------------------------------------------------------------

@import "Chumpinate"

<<< "Generating Chumpinate CKDoc..." >>>;

// instantiate a CKDoc object
CKDoc doc; // documentation orchestra
// set the examples root
"../_examples/" => doc.examplesRoot;

// add group
doc.addGroup(
    // class names
    [
        "Package",
        "PackageVersion"
    ],
    // group name
    "Chumpinate",
    // file name
    "chumpinate", 
    // group description
    "Chumpinate is a helper package for generating packages for ChucK Manager of Packages (ChuMP)."
);

// generate
doc.outputToDir( ".", "Chumpinate API Reference (v0.1.0)" );