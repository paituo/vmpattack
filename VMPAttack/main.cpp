#ifdef _WIN32
#include <windows.h>
#endif
#include <cstdint>

#include "vmpattack.hpp"

#include <vtil/compiler>
#include <fstream>
#include <filesystem>

#ifdef _MSC_VER
#pragma comment(linker, "/STACK:34359738368")
#endif

using namespace vtil;
using namespace vtil::optimizer;
using namespace vtil::logger;

namespace vmpattack
{
    using std::uint8_t;

    template <typename T = uint8_t> 
    auto read_file(const char* filepath) -> std::vector<T>
    {
        std::ifstream file(filepath, std::ios::binary);
        std::vector<T> file_buf(std::istreambuf_iterator<char>(file), {});
        return file_buf;
    }

    extern "C" int main( int argc, const char* args[])
    {
        std::filesystem::path input_file_path = { args[1] };

        // Create an output directory.
        //
        std::filesystem::path output_path = input_file_path;
        output_path.remove_filename();
        output_path /= "VMPAttack-Output";

        // Create the directory if it doesn't exist already.
        //
        std::filesystem::create_directory( output_path );

        std::vector<uint8_t> buffer = read_file( input_file_path.string().c_str() );

        log<CON_GRN>( "** Loaded raw image buffer @ 0x%p of size 0x%X\r\n", buffer.data(), buffer.size() );

        vmpattack instance( buffer );
        
        std::vector<scan_result> scan_results = instance.scan_for_vmentry();

        log<CON_GRN>( "** Found %u virtualized routines:\r\n", scan_results.size() );

        for ( const scan_result& scan_result : scan_results )
            log<CON_CYN>( "\t** RVA 0x%X VMEntry 0x%X Stub 0x%X\r\n", scan_result.rva, scan_result.job.vmentry_rva, scan_result.job.entry_stub );

        log( "\r\n" );

        std::vector<vtil::routine*> lifted_routines;

        int i = 0;

        for ( const scan_result& scan_result : scan_results )
        {
            log<CON_YLW>( "** Devirtualizing routine %i/%i @ 0x%X...\r\n", i + 1, scan_results.size(), scan_result.rva );

            std::optional<vtil::routine*> routine = instance.lift( scan_result.job );

            if ( routine )
            {
                log<CON_GRN>( "\t** Lifting success\r\n" );
                lifted_routines.push_back( *routine );

//#ifdef _DEBUG
//				vtil::debug::dump(*routine);
//#endif

                std::wstring save_path = output_path / vtil::format::str( "0x%X.vtil", scan_result.rva );
                vtil::save_routine( *routine, save_path );

                log<CON_GRN>( "\t** Unoptimized Saved to %s\r\n", save_path );

                vtil::optimizer::apply_all_profiled( *routine );

                log<CON_GRN>( "\t** Optimization success\r\n" );

//#ifdef _DEBUG
//                vtil::debug::dump( *routine );
//#endif
//
                std::wstring optimized_save_path = output_path / vtil::format::str( "0x%X-Optimized.vtil", scan_result.rva );
                vtil::save_routine( *routine, optimized_save_path );

                log<CON_GRN>( "\t** Optimized Saved to %s\r\n", save_path );
            }
            else
                log<CON_RED>( "\t** Lifting failed\r\n" );

            i++;
        }

        system( "pause" );
    }
}
