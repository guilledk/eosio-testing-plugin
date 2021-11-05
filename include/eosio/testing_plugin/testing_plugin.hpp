#pragma once
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>

#include <eosio/chain/name.hpp>

#include <fc/io/json.hpp>

#include <appbase/application.hpp>

namespace eosio {

    using chain::digest_type;
    using chain::block_header;
    using chain::block_id_type;
    using chain::signature_type;
    using chain::private_key_type;
    using chain::signed_block_ptr;
    using chain::transaction_trace_ptr;

    template<typename KeyType = fc::ecc::private_key_shim>
    static auto get_private_key(string keyname, string role = "owner") {
        auto secret = fc::sha256::hash(keyname + role);
        return private_key_type::regenerate<KeyType>(secret);
    }

    template<typename KeyType = fc::ecc::private_key_shim>
    static auto get_public_key(string keyname, string role = "owner") {
        return get_private_key<KeyType>(keyname, role).get_public_key();
    }

    class testing_plugin : public appbase::plugin<testing_plugin> {
        public:
            testing_plugin();
            virtual ~testing_plugin();

            signed_block_ptr _produce_block(fc::microseconds skip_time);

            void _start_block(fc::time_point block_time);
            signed_block_ptr _finish_block();

            APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin))
            virtual void set_program_options(options_description&, options_description& cfg) override;
            
            void plugin_initialize(const variables_map& options);
            void plugin_startup();
            void plugin_shutdown();

        private:
            std::unique_ptr<class testing_plugin_impl> my;
            map<account_name, block_id_type> last_produced_block;
            controller* control;
    };

}
