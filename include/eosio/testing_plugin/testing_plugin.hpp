#pragma once
#include <eosio/chain_plugin/chain_plugin.hpp>
#include <eosio/http_plugin/http_plugin.hpp>
#include <appbase/application.hpp>

namespace eosio {

    using namespace appbase;

    class testing_plugin : public appbase::plugin<testing_plugin> {
        public:
            testing_plugin();
            virtual ~testing_plugin();
            
            APPBASE_PLUGIN_REQUIRES((chain_plugin)(http_plugin))
            virtual void set_program_options(options_description&, options_description& cfg) override;
            
            void plugin_initialize(const variables_map& options);
            void plugin_startup();
            void plugin_shutdown();

        private:
            std::unique_ptr<class testing_plugin_impl> my;
    };

}
