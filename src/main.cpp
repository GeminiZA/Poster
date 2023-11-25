#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <json.h>
#include <poster.h>

int main()
{
    Request req = Request("GET https://jsonplaceholder.typicode.com /posts/1");
    Response res = req.fetch();
    std::cout << req.toString();
    std::cout << res.toString() << std::endl;
}

// int main(int argc, char *argv[])
// {
//     std::string address = "";
//     std::string file_path = "";
//     std::fstream file;
//     for (int i = 0; i < argc; i++)
//     {
//         std::string arg = argv[i];
//         if (arg == "-h" || arg == "--help")
//         {
//             // Display usage/help information
//             std::cout << "Usage: " << argv[0] << " [options]" << std::endl;
//             std::cout << "Options:" << std::endl;
//             std::cout << "-h, --help\tDisplay this help message" << std::endl;
//             std::cout << "-a\t\tSpecify an address/host to send the request" << std::endl;
//             std::cout << "-f\t\tSpecify a file for the request" << std::endl;
//             // Add more options here
//             return 0;
//         }
//         if (arg == "-a")
//         {
//             address = argv[i + 1];
//         }
//         if (arg == "-f")
//         {
//             file_path = argv[i + 1];
//             file.open(file_path, std::ios::in);
//             if (!file.is_open())
//             {
//                 std::cerr << "Error opening the file" << std::endl;
//                 return 1;
//             }
//         }
//     }
//     if (address == "")
//         address = "http://127.0.0.1:8000";
//     if (file_path == "")
//     {
//         file_path = "req.json";
//         file.open(file_path, std::ios::in);
//         if (!file.is_open())
//         {
//             std::cerr << "Error opening the file" << std::endl;
//             return 1;
//         }
//     }
//     std::cout << "Address: " << address << std::endl;
//     std::cout << "File: " << file_path << std::endl;
// }