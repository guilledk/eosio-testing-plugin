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
        control->finalize_block([&](digest_type d) {
            std::vector<signature_type> result;

            //auto key = 
            //result.push_back(key.sign(d));

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
    wallet = &app().get_plugin<wallet_plugin>().get_wallet_manager();

    string wallet_dir("/root/eosio-wallet");
    string wallet_name("default");

    wallet->set_dir(wallet_dir);
    wallet->create(wallet_name);

    ilog("testing plugin init.");
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/version", [&](string url, string body, url_response_callback callback) {
            try {
                callback(200, string("v0.1a0"));
            } catch (...) {
                http_plugin::handle_exception("testing", "version", body, callback);
            }
        }}
    });
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/unlock", [&](string url, string body, url_response_callback callback) {
            try {
				fc::variant args = fc::json::from_string(body).get_object();

                wallet->unlock(wallet_name, args["pass"].as_string()); 

                callback(200, string("ok"));
            } catch (...) {
                http_plugin::handle_exception("testing", "version", body, callback);
            }
        }}
    });
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/skiptime", [&](string url, string body, url_response_callback callback) {
            try {
				fc::variant args = fc::json::from_string(body).get_object();
                fc::microseconds skip_time(args["time"].as_int64());
               
                _produce_block(skip_time);

                callback(200, string("ok"));
            } catch (...) {
                http_plugin::handle_exception("testing", "skiptime", body, callback);
            }
        }}
    });
    app().get_plugin<http_plugin>().add_api({
        {"/v1/testing/debug", [&](string url, string body, url_response_callback callback) {
            try {
                string ret;

                ret += "head_block_num: " + std::to_string(control->head_block_num()) + "\n";
                ret += "head_block_id: " + control->head_block_id().str() + "\n";
                ret += "head_block_time: " + std::to_string(control->head_block_time().sec_since_epoch()) + "\n";
                ret += "head_block_producer: " + control->head_block_producer().to_string() + "\n";
                ret += "head_block_prev_id: " + control->head_block_header().previous.str() + "\n";

                //ret += "pending_block_time: " + std::to_string(control->pending_block_time().sec_since_epoch()) + "\n";
                //ret += "pending_block_producer: " + control->pending_block_producer().to_string();

                ret += "is_building_block: " + std::to_string(control->is_building_block()) + "\n";
                ret += "is_producing_block: " + std::to_string(control->is_producing_block()) + "\n";

                ret += "wallets: \n";
                for (string& wallet_name : wallet->list_wallets())
                    ret += "\t" + wallet_name + "\n";

                callback(200, ret);
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
