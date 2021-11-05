#include <eosio/testing_plugin/testing_plugin.hpp>

namespace eosio {
   static appbase::abstract_plugin& _testing_plugin = app().register_plugin<testing_plugin>();

class testing_plugin_impl {
   public:
};

testing_plugin::testing_plugin():my(new testing_plugin_impl()){}
testing_plugin::~testing_plugin(){}

signed_block_ptr testing_plugin::_produce_block(fc::microseconds skip_time) {
    auto head = control->head_block_state();
    auto head_time = control->head_block_time();
    auto next_time = head_time + skip_time;

    if(!control->is_building_block() || control->pending_block_time() != next_time) {
        _start_block( next_time );
    }

    auto head_block = _finish_block();

    _start_block( next_time + fc::microseconds(chain::config::block_interval_us));
    return head_block;
}

void testing_plugin::_start_block(fc::time_point block_time) {
    auto head_block_number = control->head_block_num();
    auto producer = control->head_block_state()->get_scheduled_producer(block_time);

    auto last_produced_block_num = control->last_irreversible_block_num();
    auto itr = last_produced_block.find(producer.producer_name);
    if (itr != last_produced_block.end()) {
        last_produced_block_num = std::max(control->last_irreversible_block_num(), block_header::num_from_id(itr->second));
    }

    control->start_block(
        block_time,
        head_block_number - last_produced_block_num,
        control->head_block_state()->get_new_protocol_feature_activations());

}

signed_block_ptr testing_plugin::_finish_block() {
    FC_ASSERT(control->is_building_block(), "must first start a block before it can be finished");

    auto producer = control->head_block_state()->get_scheduled_producer(control->pending_block_time());
    vector<private_key_type> signing_keys;
    signing_keys.push_back(get_private_key("eosio", "active"));

    control->finalize_block([&](digest_type d) {
        std::vector<signature_type> result;
        result.reserve(signing_keys.size());
        for (const auto& k: signing_keys)
            result.emplace_back(k.sign(d));

        return result;
    });

    control->commit_block();
    last_produced_block[control->head_block_state()->header.producer] = control->head_block_state()->id;

    return control->head_block_state()->block;
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
}

void testing_plugin::plugin_shutdown() {
   // OK, that's enough magic
}

}
