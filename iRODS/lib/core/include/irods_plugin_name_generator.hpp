/* -*- mode: c++; fill-column: 132; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#ifndef _plugin_name_generator_H_
#define _plugin_name_generator_H_

#include "irods_error.hpp"
#include <vector>
#include <string>

namespace irods {

    /**
     * @brief Functor for generating plugin filenames from the resource name
     */
    class plugin_name_generator {
    public:

        /// @brief Container for plugin names as std::strings. Use stl iterators and consider it read only.
        typedef std::vector<std::string> plugin_list_t;

        /// @brief ctor
        plugin_name_generator( void );
        virtual ~plugin_name_generator( void );

        /// @brief functor for generating the name from a base name and a directory
        virtual error operator()( const std::string& _base_name, const std::string& _dir_name, std::string& _rtn_soname );

        /// @brief Constructs a library name from the plugin name and returns true if the library exists and is readable.
        bool exists( const std::string& _base_name, const std::string& _dir_name );

        /// @brief Searches the specified directory for libraries and returns a list of constructed plugin names.
        error list_plugins( const std::string& _dir_name, plugin_list_t& _list );

    private:
        /// @brief Generates a plugin name from a shared object file name or an empty string if it is not a shared object.
        error generate_plugin_name( const std::string& filename, std::string& _rtn_name );

    };
}; // namespace irods

#endif // _plugin_name_generator_H_
