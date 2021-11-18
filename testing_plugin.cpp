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
    //FC_LOG_AND_RETHROW()
}

void testing_plugin::plugin_startup() {

    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/version", [&](string url, string body, url_response_callback callback) {
            try {
                callback(200, string("v0.1a1"));
            } catch (...) {
                http_plugin::handle_exception("testing", "version", body, callback);
            }
        }}
    });
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/settime", [&](string url, string body, url_response_callback callback) {
            try {
				fc::variant args = fc::json::from_string(body).get_object();

                testing_time_provider::get().set_time(args["time"].as_int64());

                if (args["abort"].as_bool())
                    app().get_plugin<chain_plugin>().chain().abort_block();

                callback(200, string("ok"));
            } catch (...) {
                http_plugin::handle_exception("testing", "settime", body, callback);
            }
        }}
    });
    ilog("testing plugin init.");
}

void testing_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
