#include <eosio/testing_plugin/testing_plugin.hpp>


namespace eosio {
   static appbase::abstract_plugin& _testing_plugin = app().register_plugin<testing_plugin>();

class testing_plugin_impl {
   public:
};

testing_plugin::testing_plugin():my(new testing_plugin_impl()){}
testing_plugin::~testing_plugin(){}

void testing_plugin::_finish_block() {
    control->finalize_block([&](digest_type d) {
        std::vector<signature_type> result;

        result.push_back(eosio_key.sign(d));
        
        return result;
    });
    control->commit_block();
}

void testing_plugin::_start_block(chain::time_point when) {
    auto head_block_number = control->head_block_num();
    auto last_produced_block_num = control->last_irreversible_block_num();
    auto confirm_block_num = head_block_number - last_produced_block_num;

    control->start_block(when, 0);
}

void testing_plugin::_produce_block(fc::microseconds skip_time) {
    auto head = control->head_block_state();
    auto head_time = control->head_block_time();
    auto next_time = head_time + skip_time;

    if (control->is_building_block())
        _finish_block();

    _start_block(next_time);

    _finish_block();

    _start_block(
        next_time + fc::microseconds(chain::config::block_interval_us));
}

void testing_plugin::set_program_options(options_description&, options_description& cfg) {}

void testing_plugin::plugin_initialize(const variables_map& options) {
    //FC_LOG_AND_RETHROW()
}

void testing_plugin::plugin_startup() {
   // Make the magic happen

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
