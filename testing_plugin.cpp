#include <eosio/testing_plugin/testing_plugin.hpp>


namespace eosio {
   static appbase::abstract_plugin& _testing_plugin = app().register_plugin<testing_plugin>();

class testing_plugin_impl {
   public:
};

testing_plugin::testing_plugin():my(new testing_plugin_impl()){}
testing_plugin::~testing_plugin(){}

void testing_plugin::_skip_time(fc::microseconds time) {

    production->pause();
    
    if (control->is_building_block())
        control->abort_block();

    control->start_block(time_point::now() + time, 0);

    control->finalize_block([&](digest_type d) {
        std::vector<signature_type> result;

        result.push_back(eosio_key.sign(d));
        
        return result;
    });
    control->commit_block();

    block_state_ptr new_bs = control->head_block_state();

    ilog(
        "Produced skip time block ${id}... #${n} @ ${t} signed by ${p} [trxs: ${count}, lib: ${lib}, confirmed: ${confs}]",
        ("p", new_bs->header.producer)
        ("id", new_bs->id.str().substr(8,16))
        ("n", new_bs->block_num)
        ("t", new_bs->header.timestamp)
        ("count", new_bs->block->transactions.size())
        ("lib", control->last_irreversible_block_num())
        ("confs", new_bs->header.confirmed));

    production->resume();

}

void testing_plugin::set_program_options(options_description&, options_description& cfg) {}

void testing_plugin::plugin_initialize(const variables_map& options) {
    //FC_LOG_AND_RETHROW()
}

void testing_plugin::plugin_startup() {
   // Make the magic happen
    production = &app().get_plugin<producer_plugin>();
    control = &app().get_plugin<chain_plugin>().chain();

    eosio_key = private_key_type(
        string("5KQwrPbwdL6PhXujxW37FSSQZ1JiwsST4cqQzDeyXtP79zkvFD3"));

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
        {"/v1/testing/skiptime", [&](string url, string body, url_response_callback callback) {
            try {
				fc::variant args = fc::json::from_string(body).get_object();
                fc::microseconds time(args["time"].as_int64());
               
                _skip_time(time);

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

                callback(200, ret);
            } catch (...) {
                http_plugin::handle_exception("testing", "debug", body, callback);
            }
        }}
    });
    ilog("testing plugin init.");
}

void testing_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
