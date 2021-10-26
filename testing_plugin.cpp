#include <eosio/testing_plugin/testing_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _testing_plugin = app().register_plugin<testing_plugin>();

class testing_plugin_impl {
   public:
};

testing_plugin::testing_plugin():my(new testing_plugin_impl()){}
testing_plugin::~testing_plugin(){}

void testing_plugin::set_program_options(options_description&, options_description& cfg) {}

void testing_plugin::plugin_initialize(const variables_map& options) {
    // FC_LOG_AND_RETHROW()
}

void testing_plugin::plugin_startup() {
   // Make the magic happen
}

void testing_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
