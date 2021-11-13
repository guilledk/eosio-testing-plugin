#pragma once
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <eosio/producer_plugin/producer_plugin.hpp>

#include <eosio/chain/name.hpp>

#include <fc/io/json.hpp>

#include <appbase/application.hpp>


using std::string;
using std::to_string;

namespace eosio {

    using chain::digest_type;
    using chain::block_header;
    using chain::block_id_type;
    using chain::signature_type;
    using chain::private_key_type;
    using chain::signed_block_ptr;
    using chain::transaction_trace_ptr;

    class testing_plugin : public appbase::plugin<testing_plugin> {
        public:
            testing_plugin();
            virtual ~testing_plugin();

            void _skip_time(fc::microseconds time);

            APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin)(producer_plugin))
            virtual void set_program_options(options_description&, options_description& cfg) override;
            
            void plugin_initialize(const variables_map& options);
            void plugin_startup();
            void plugin_shutdown();

        private:
            std::unique_ptr<class testing_plugin_impl> my;
            private_key_type eosio_key;
            producer_plugin* production;
            controller* control;
    };

}
