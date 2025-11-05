// HuorongBlockingListGenerator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//


#include <chrono>
#include <codecvt>
#include <fstream>
#include <iostream>
#include <locale>
#include <vector>

#include <Windows.h>
#include <io.h>

#include <nlohmann/json.hpp>

#define _UNICODE

void _dfs_retrieve_directories(std::vector<std::wstring> current, std::vector<std::wstring>& dir_container)
{
    for (std::wstring dir : current) {
        intptr_t hdl;
        _wfinddata_t info;
        std::vector<std::wstring> dirs_found;
        size_t pos = dir.find_last_of('\\');
        std::wstring dir_without_star = dir.substr(0, pos);

        hdl = _wfindfirst(dir.c_str(), &info);
        std::cout << dir.c_str() << std::endl;
        if (hdl != -1) {
            int flag = 0;
            while (flag != -1) {
                if ((info.attrib & _A_SUBDIR) > 0) {
                    std::wstring name = info.name;
                    if (name != L"." && name != L"..") {
                        std::wstring tmp = dir_without_star + L"\\" + name + L"\\*";
                        dirs_found.push_back(tmp);
                        dir_container.push_back(tmp);
                    }
                }
                flag = _wfindnext(hdl, &info);
            }
            _findclose(hdl); 
            _dfs_retrieve_directories(dirs_found, dir_container);
        }
        else {
            std::cout << "Directory find failed" << std::endl;
            _findclose(hdl);
            return;
        }
    }
}

std::vector<std::wstring> find_excs(const wchar_t* pattern) 
{
    std::vector<std::wstring> res;
    std::wstring ptn = pattern;
    ptn = ptn + L".exe";
    std::wstring dir = pattern;
    size_t pos = dir.find_last_of(L'\\');
    dir = dir.substr(0, pos+1);
    intptr_t hdl;
    _wfinddata_t info;

    hdl = _wfindfirst(ptn.c_str(), &info);
    if (hdl != -1) {
        int flag = 0;
        while (flag != -1) {
            res.push_back(dir + info.name);
            flag = _wfindnext(hdl, &info);
        }
    }
    _findclose(hdl);
    return res;
}

std::wstring create_block(const std::wstring&& file_name) 
{
    std::wstring_convert<std::codecvt_utf8<wchar_t>> cvt;

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(
                         now.time_since_epoch())
                         .count();   

    nlohmann::json json;
    json["procname"] = cvt.to_bytes(file_name);
    json["block"] = true;
    json["time"] = timestamp;
    auto utf8 = json.dump();
    return cvt.from_bytes(utf8);
}

std::wstring to_json(const std::vector<std::wstring>& file_names) 
{
    const wchar_t* start = L"{\"ver\":\"5.0\", \"tag\":\"appnetctrl\", \"data\": [ ";
    const wchar_t* end = L"]}";
     std::wstring res = start;
    for (std::wstring file : file_names) 
    {
        res.append(create_block(std::move(file)) + L",");
    }
    res = res.substr(0, res.length() - 1);
    res += end;
    return res;
}


int main()
{
    wchar_t buff[FILENAME_MAX];
    auto a = _wgetcwd(buff, FILENAME_MAX);
    if (a == NULL)
    {
        std::cout << "Permission denied.." << std::endl;
        return 0;
    }

    std::wcout << L"Working directory: " << a << std::endl;
    std::wstring dir_to_search = std::wstring(buff) + L"\\*";
    std::vector<std::wstring> sv;
    std::vector<std::wstring> container;
    std::vector<std::wstring> files;
    sv.push_back(dir_to_search);

    _dfs_retrieve_directories(sv, container);
    std::cout << "A total of " << container.size() << " directory(s) had been found." << std::endl;
    sv.clear();

    container.push_back(std::wstring(a));
    for (auto d : container) 
    {
        for (auto f : find_excs(d.c_str())) 
        {
            if (f.find(L"HuorongBlocking") != std::wstring::npos)
                continue;
            std::wcout << f << std::endl;
            files.push_back(f);
        }
    }
    
    for (auto ssss : files) 
    {
        std::wcout << L"Writing block exe: " << ssss << std::endl;
    }

    std::wofstream opt;
    opt.open(dir_to_search.substr(0, dir_to_search.find_last_of('\\')) + L"\\Rules.json");
    opt << to_json(files) << std::endl;
    opt.close();

    std::cout << "A total of " << files.size() << " file(s) had been found." << std::endl;

    wchar_t buffer[256];
    swprintf_s(buffer, L"A total of %d file(s) had been written to Rule.json.", (int)files.size());
    MessageBoxW(0, buffer, 0, 0);
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start De bugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
