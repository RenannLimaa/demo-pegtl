#include <iostream>
#include <tao/pegtl.hpp>
#include <map>

namespace pegtl = tao::pegtl;

using Section = std::map<std::string, std::map<std::string, std::string>>;

namespace client
{
    struct IniObj 
    {
        Section section;
        std::string section_title;
        std::string current_key_value;
    };

    /* ----- Grammar ----- */

    struct section_title : pegtl::identifier {};
    struct key : pegtl::identifier {};
    struct value : pegtl::plus< pegtl::print > {};

    struct key_value :
        pegtl::seq<
            pegtl::star< pegtl::space >,
            key,
            pegtl::star< pegtl::space >,
            pegtl::one<'='>,
            pegtl::star< pegtl::space >,
            value,
            pegtl::star< pegtl::eol >
        >
    {};

    struct header :
        pegtl::seq<
            pegtl::one<'['>,
            pegtl::star< pegtl::space >,
            section_title,
            pegtl::star< pegtl::space >,
            pegtl::one<']'>,
            pegtl::star< pegtl::space >,
            pegtl::opt< pegtl::eol >
        >
    {};

    struct section :
        pegtl::seq<
            header,
            pegtl::plus< key_value >
        >
    {};


   // struct grammar :
   //    pegtl::must< header, pegtl::eof >
   // {};

    /* ----- Rules -----*/
    
    template< typename Rule >
    struct action
    {};

    template<>
    struct action< section_title >
    {
        template< typename ParseInput >
        static void apply( const ParseInput& in, IniObj& ini)
        {
            ini.section_title = in.string();
        }
    };

    template<>
    struct action< key >
    {
        template< typename ParseInput >
        static void apply(const ParseInput& in, IniObj& ini)
        {
            ini.current_key_value = in.string();
        }
    };

    template <>
    struct action< value >
    {
        template < typename ParseInput >
        static void apply(const ParseInput& in, IniObj& ini)
        {
            ini.section[ini.section_title][ini.current_key_value] = in.string();
        }
    };

}

int main(int argc, char* argv[])
{
    if (argc != 2)
        return 1;

    client::IniObj ini;
    pegtl::argv_input in(argv, 1);
    auto result = pegtl::parse<client::section, client::action>(in, ini);

    if (result)
    {
        for (const auto &[section_header, key_value] : ini.section)
        {
            std::cout << "Section: " << section_header << std::endl;
            for (const auto &[k, v] : key_value)
            {
                std::cout << "\t\t" << k << " = " << v << std::endl;
            }
        }
    }
    else
        std::cout << "Failure\n";

    return 0;
}
