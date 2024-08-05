#define CATCH_CONFIG_RUNNER
#include <catch2/catch_session.hpp>

int main(int argc, char* argv[]) {
    int res = Catch::Session().run(argc, argv);
    return res;
}
