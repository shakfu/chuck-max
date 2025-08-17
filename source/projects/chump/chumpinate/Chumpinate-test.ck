@import "Chumpinate"

// instantiate a Chumpinate package
Package pkg("test-pkg");

// Add our metadata...
"nick" => pkg.authors;

["nick", "ge"] => pkg.authors;

"www.www.com" => pkg.homepage;
"www.www.com" => pkg.repository;

"mit" => pkg.license;
"a description" => pkg.description;

["wow", "much", "keyword"] => pkg.keywords;

// generate a package-definition.json
// This will be stored in "test-pkg/package.json"
"./" => pkg.generatePackageDefinition;


// Now we need to define a specific PackageVersion for test-pkg
PackageVersion ver("test-pkg", "1.0.0");

"10.2" => ver.apiVersion;
<<< ver.apiVersion() >>>;

"1.5.4.2" => ver.languageVersionMin;
// <<< ver.languageVersionMin() >>>;

"1.5.4.3" => ver.languageVersionMax;
<<< ver.languageVersionMax() >>>;

"linux" => ver.os;
<<< ver.os() >>>;

"x86_64" => ver.arch;
<<< ver.arch() >>>;


// Add a bunch of files that are found in this version of the pkg
ver.addFile("./test.txt");
ver.addDataFile("./test.txt");
ver.addExampleFile("./test.txt", "test");
ver.addDocsFile("./test.txt");
ver.addDepsFile("./test.txt");

// wrap up all our files into a zip file, and tell Chumpinate what URL
// this zip file will be located at.
ver.generateVersion("./", "test-pkg_linux", "ccrma.stanford.edu/~nshaheed/test-pkg.zip");

// Generate a version definition json file, stores this in "test-pkg/1.0.0/test-pkg_linux.json"
ver.generateVersionDefinition("test-pkg_linux", "./");


