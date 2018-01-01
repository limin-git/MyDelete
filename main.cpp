// MyDelete.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

const char* LINE_BEGIN = "\r                                                                                                                        \r";

std::vector<boost::regex> build_regexs(const std::vector<std::string>& folder_names)
{
    std::vector<boost::regex> regexs;
    std::vector<std::string>& names = const_cast< std::vector<std::string>& >(folder_names);

    for (std::string& name : names)
    {
        static const std::vector<std::pair<const char*, const char*>> items =
        {
            {"/", "\\" },
            {"\\", "\\\\" },
            {" ", "[ ]" }, // (?x) ×ª»»¿Õ¸ñ
            {"(", "\\(" },
            {")", "\\)" },
            {".", "\\." },
            {"*", ".*?" },
            {"?", "." }
        };

        for (auto pair : items)
        {
            boost::replace_all(name, pair.first, pair.second);
        }

        regexs.push_back(boost::regex("(?ix) \\\\" + name + "$"));
    }

    return regexs;
}


std::vector<boost::filesystem::path> find_folders(const std::string& dir_path, std::vector<std::string>& folders_to_remove)
{
    std::vector<boost::filesystem::path> pathes;
    boost::filesystem::path path(dir_path);

    if (!exists(path))
    {
        std::cout << "error: " << dir_path << " does not exist." << std::endl;
        return pathes;
    }

    const std::vector<boost::regex>& regexs = build_regexs(folders_to_remove);

    int cnt = 0;

    boost::filesystem::recursive_directory_iterator end_itr; // default construction yields past-the-end

    for (boost::filesystem::recursive_directory_iterator it(path); it != end_itr; ++it)
    {
        if (true == boost::filesystem::is_directory(it->status()))
        {
            const std::string& folder_name = it->path().string();

            for (size_t i = 0; i < regexs.size(); ++i)
            {
                if (boost::regex_search(folder_name, regexs[i]))
                {
                    pathes.push_back(it->path());
                    it.no_push();
                    std::cout << std::setw(3) << std::setfill('0') << ++cnt << ": " << folder_name << std::endl;
                    break;
                }
            }
        }
    }

    return pathes;
}


BOOL remove_attribute(const std::string& file_name, const DWORD attribute = FILE_ATTRIBUTE_READONLY)
{
    DWORD dwAttrs = ::GetFileAttributes(file_name.c_str());

    if (INVALID_FILE_ATTRIBUTES == dwAttrs)
    {
        std::cout << "failed to get attribute for " << file_name << std::endl;
        return FALSE;
    }

    if (dwAttrs & attribute)
    {
        return ::SetFileAttributes(file_name.c_str(), dwAttrs & ~attribute);
    }

    return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
    if (argc < 3)
    {
        std::cout
                << "Usage:\n\t"
                << boost::filesystem::path(argv[0]).stem().string() << " <path> <folder_name>\n";

        return 0;
    }

    std::vector<std::string> folders_to_remove(argv + 2, argv + argc);
    const std::vector<boost::filesystem::path>& pathes = find_folders(argv[1], folders_to_remove);

    if (pathes.empty())
    {
        system("cls");
        std::cout << "folder not found" << std::endl;
        return 0;
    }

    std::cout << "type 'yes' for delete: ";

    std::string command;
    std::cin >> command;

    if (boost::iequals("yes", command))
    {
        for (size_t i = 0; i < pathes.size(); ++i)
        {
            if (exists(pathes[i]))
            {
                boost::filesystem::recursive_directory_iterator end_itr; // default construction yields past-the-end

                for (boost::filesystem::recursive_directory_iterator it(pathes[i]); it != end_itr; ++it)
                {
                    remove_attribute(it->path().string(), FILE_ATTRIBUTE_READONLY);
                }

                boost::system::error_code ec;
                remove_all(pathes[i], ec);

                if (ec)
                {
                    std::cout << pathes[i].string() << ": " << ec.message() << std::endl;
                }
            }
        }
    }

    return 0;
}
