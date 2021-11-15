#pragma once
#include <eosio/http_plugin/http_plugin.hpp>

#include <eosio/chain/name.hpp>

#include <fc/time.hpp>
#include <fc/io/json.hpp>

#include <appbase/application.hpp>


using std::string;
using std::to_string;

using fc::testing_time_provider;

namespace eosio {

    class testing_plugin : public appbase::plugin<testing_plugin> {
        public:
            testing_plugin();
            virtual ~testing_plugin();

            APPBASE_PLUGIN_REQUIRES((http_plugin))
            virtual void set_program_options(options_description&, options_description& cfg) override;
            
            void plugin_initialize(const variables_map& options);
            void plugin_startup();
            void plugin_shutdown();

        private:
            std::unique_ptr<class testing_plugin_impl> my;
    };

}
