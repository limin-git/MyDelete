// MyDelete.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


std::vector<boost::filesystem::path> delete_folder( const std::string& dir_path, const std::vector<std::string>& folders_to_remove )
{
    std::vector<boost::filesystem::path> pathes;

    boost::filesystem::path path( dir_path );

    if ( !exists( path ) )
    {
        std::cout << "error: " << dir_path << " does not exist." << std::endl;
        return pathes;
    }

    std::vector<boost::regex> regexs;

    for ( size_t i = 0; i < folders_to_remove.size(); ++i )
    {
        regexs.push_back( boost::regex( "\\\\" + folders_to_remove[i] + "$" ) );
    }

    int cnt = 0;

    boost::filesystem::recursive_directory_iterator end_itr; // default construction yields past-the-end
    for ( boost::filesystem::recursive_directory_iterator it( path ); it != end_itr; ++it )
    {
        if ( true == is_directory( it->status() ) )
        {
            std::string folder_name = it->path().string();

            for ( size_t i = 0; i < regexs.size(); ++i )
            {
                if ( boost::regex_search( folder_name, regexs[i] ) )
                {
                    pathes.push_back( it->path() );
                    std::cout << "\rfound " << ++cnt << std::flush;
                    break;
                }
            }
        }
    }

    return pathes;
}


int _tmain(int argc, _TCHAR* argv[])
{
    if ( argc < 3 )
    {
        boost::filesystem::path path( argv[0] );

        std::cout
            << "usage:\n"
            << path.stem().string() << " <path> <folder_name>\n";

        return -1;
    }

    std::vector<std::string> folders_to_remove;

    for ( int i = 2; i < argc; ++i )
    {
        folders_to_remove.push_back( argv[i] );
    }

    std::vector<boost::filesystem::path> pathes = delete_folder( argv[1], folders_to_remove );

    if ( pathes.empty() )
    {
        std::cout << "folder not found" << std::endl;
        return 0;
    }

    ::system( "cls" );

    std::cout << "folders to be deleted: " << std::endl;

    for ( size_t i = 0; i < pathes.size(); ++i )
    {
        std::cout << pathes[i].string() << std::endl;
    }

    std::cout << "are you sure(y/n): ";

    std::string command;
    std::cin >> command;

    if ( "y" == boost::algorithm::to_lower_copy(command) )
    {
        for ( size_t i = 0; i < pathes.size(); ++i )
        {
            if ( exists( pathes[i] ) )
            {
                boost::filesystem::recursive_directory_iterator end_itr; // default construction yields past-the-end
                for ( boost::filesystem::recursive_directory_iterator it( pathes[i] ); it != end_itr; ++it )
                {
                    ::SetFileAttributes( it->path().string().c_str(), FILE_ATTRIBUTE_NORMAL );
                }

                boost::system::error_code ec;
                remove_all( pathes[i], ec );

                if ( ec )
                {
                    std::cout << ec.message() << std::endl;
                }       
            }
        }
    }

	return 0;
}
