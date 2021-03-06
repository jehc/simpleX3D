void runX3DTest(const char* filename) {

    // load the world
    World* world = World::read(browser(), filename);

    // get the test suite
    TestSuite* suite = browser()->getFirst<TestSuite>(); // ("TestSuite");
    ASSERT_THAT(suite, NotNull());

    // run the tests
    suite->realize(); // TODO: root nodes should be realized automatically...
    suite->runTests();

    // print the failed tests' failure reasons
    MFNode<TestNode>& failed = suite->failed();
    MFNode<TestNode>::iterator it;
    if (!failed.empty())
        cout << "Some tests for '" << suite->desc() << "' failed:" << endl;
    for (it = failed.begin(); it != failed.end(); it++) {
        TestNode* test = *it;
        cout << "  " << test->getName();
        if (!test->desc().empty())
            cout << ": " << test->desc();
        cout << endl;
        MFString& reasons = test->reasons();
        MFString::iterator r_it;
        for (r_it = reasons.begin(); r_it != reasons.end(); r_it++)
            cout << "    " << *r_it << endl;
    }

    int numFailed = suite->numFailed();

    // clean up
    delete world;
    browser()->reset();

    // check correct num passed/failed
    if (numFailed > 0)
        FAIL();
    else
        SUCCEED();
}
