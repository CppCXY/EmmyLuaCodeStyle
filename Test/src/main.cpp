#include <gtest/gtest.h>
#include "TestHelper.h"
int main(int argc, char **argv) {
    if(argc > 1){
        TestHelper::ScriptBase = argv[1];
    }

    printf("Running main() from %s\n", __FILE__);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}