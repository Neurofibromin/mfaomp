#include <catch2/catch_test_macros.hpp>
#include <QApplication>

TEST_CASE("Example Test", "[example]") {
    REQUIRE(1 == 1);
}

TEST_CASE("Example Test 2", "[example]") {
    int x = 2;
    int y = 2;
    REQUIRE(x == y);
}

#include "AddVideo.h"

TEST_CASE("Gui Test example 1", "[gui]") {
    int argc = 0;
    char* argv[] = { nullptr };
    QApplication app(argc, argv);
}