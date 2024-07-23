add_test( HttpTest.GetRequest /home/angela/42/WebServ/build/webservtest [==[--gtest_filter=HttpTest.GetRequest]==] --gtest_also_run_disabled_tests)
set_tests_properties( HttpTest.GetRequest PROPERTIES WORKING_DIRECTORY /home/angela/42/WebServ/build SKIP_REGULAR_EXPRESSION [==[\[  SKIPPED \]]==])
set( webservtest_TESTS HttpTest.GetRequest)
