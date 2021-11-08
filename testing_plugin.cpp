#include <eosio/testing_plugin/testing_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _testing_plugin = app().register_plugin<testing_plugin>();

class testing_plugin_impl {
   public:
};

testing_plugin::testing_plugin():my(new testing_plugin_impl()){}
testing_plugin::~testing_plugin(){}

void testing_plugin::_produce_block(fc::microseconds skip_time) {
    auto head = control->head_block_state();
    auto head_time = control->head_block_time();
    auto next_time = head_time + skip_time;

    if (control->is_building_block()) {
        vector<private_key_type> keys;
        for (auto producer : control->active_producers().producers)
            keys.push_back(get_private_key(producer.producer_name.to_string(), "active"));
        
        control->finalize_block([&](digest_type d) {
            std::vector<signature_type> result;
            for (auto key : keys)
                result.push_back(key.sign(d));

            return result;
        });
        control->commit_block();
    }
    control->start_block(next_time, 0);
}

void testing_plugin::set_program_options(options_description&, options_description& cfg) {}

void testing_plugin::plugin_initialize(const variables_map& options) {
    //FC_LOG_AND_RETHROW()
}

void testing_plugin::plugin_startup() {
   // Make the magic happen

    control = &app().get_plugin<chain_plugin>().chain();

    ilog("testing plugin init.");
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/version", [&](std::string url, std::string body, url_response_callback callback) {
            try {
                callback(200, std::string("v0.1a0"));
            } catch (...) {
                http_plugin::handle_exception("testing", "version", body, callback);
            }
        }}
    });
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/skiptime", [&](std::string url, std::string body, url_response_callback callback) {
            try {
				fc::variant args = fc::json::from_string(body).get_object();
                fc::microseconds skip_time(args["time"].as_int64());
               
                _produce_block(skip_time);

                callback(200, std::string("ok"));
            } catch (...) {
                http_plugin::handle_exception("testing", "skiptime", body, callback);
            }
        }}
    });
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/debug", [&](std::string url, std::string body, url_response_callback callback) {
            try {
                std::string out;

                out += "head block num: ";
                out += std::to_string(control->head_block_num());
                out += "\n";
                out += "head block time: ";
                out += std::to_string(control->head_block_time().sec_since_epoch());
                out += "\n";
                out += "head block producer: ";
                out += control->head_block_producer().to_string();
                out += "\n";


                callback(200, out);
            } catch (...) {
                http_plugin::handle_exception("testing", "debug", body, callback);
            }
        }}
    });
}

void testing_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
