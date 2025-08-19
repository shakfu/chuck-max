/*
ChuGL Integration Test Runner
*/
FileIO dir;
dir.open(me.dir() + "./unit");

dir.dirList() @=> string tests[];

// chuck string sorting is broken (sorts by pointer, not strcmp)
// workaround: put strings in assoc array, somehow getKeys returns them in order
for (auto test : tests) {
    "" => tests[test];
}
string keys[0];
tests.getKeys(keys);

for (auto test : keys) {
    if (test == "all.ck") continue; // ignore test runner
    if (test == "tester.ck") continue; // ignore self
    if (test == "T.ck") continue; // ignore test harness
    // run test
    T.println("Running test: " + test + " --------------------");
    Machine.add(me.dir() + "./unit/" + test);
}